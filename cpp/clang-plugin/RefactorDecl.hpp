/*************************************************************************
  > File Name: RefactorDecl.cpp
  > Author: Netcan
  > Descripton: A simple refactor plugin for clang
  > Blog: https://netcan.github.io/
  > Mail: 1469709759@qq.com
  > Created Time: 2020-08-07 22:12
 ************************************************************************/
#pragma once
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Tooling/Refactoring/Rename/RenamingAction.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Rewrite/Frontend/FixItRewriter.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace ast_matchers;

struct RewriteDeclMatcher: MatchFinder::MatchCallback {
    RewriteDeclMatcher(Rewriter &Rewriter) : RewriteDeclWriter(Rewriter) {}
    void run(const MatchFinder::MatchResult &) override;
    void onEndOfTranslationUnit() override {
        // Output to stdout
        RewriteDeclWriter.getEditBuffer(RewriteDeclWriter.getSourceMgr().getMainFileID())
            .write(llvm::outs());
        // Replace in place
        // RewriteDeclWriter.overwriteChangedFiles();
    }
    private:
    Rewriter RewriteDeclWriter;
};

struct RewriteDeclConsumer: ASTConsumer {
    RewriteDeclConsumer(Rewriter &R) : RDHandler(R) {
        DeclarationMatcher DeclMatcher = cxxRecordDecl(
                isClass(),
                isDefinition(),
                unless(isDerivedFrom(anything())),
                unless(hasParent(cxxRecordDecl())),
                forEach(cxxMethodDecl(isPure(), unless(hasTrailingReturn())))
                ).bind("classdef");
        Finder.addMatcher(DeclMatcher, &RDHandler);
    }
    void HandleTranslationUnit(ASTContext &Ctx) override {
        Finder.matchAST(Ctx);
    }
    private:
    MatchFinder Finder;
    RewriteDeclMatcher RDHandler;
};

void RewriteDeclMatcher::run(const MatchFinder::MatchResult& Result) {
    // ASTContext is used to retrieve the source location
    ASTContext *Ctx = Result.Context;
    const SourceManager& SM = Ctx->getSourceManager();
    Rewriter::RewriteOptions rmOps;
    rmOps.RemoveLineIfEmpty = true; // RemoveText的时候若空行，则删除整

    // check if class is abstract
    const auto ClassDecl = Result.Nodes.getNodeAs<CXXRecordDecl>("classdef");
    for (auto&& F: ClassDecl->methods()) {
        if (dyn_cast<CXXConstructorDecl>(F) ||
                dyn_cast<CXXDestructorDecl>(F) ||
                F->isImplicit()) {
            continue;
        }
        if (! F->isPure()) return;
    }

    {
        auto ClassName = ClassDecl->getDeclName();
        RewriteDeclWriter.ReplaceText(ClassDecl->getBeginLoc(), strlen("class "), "DEFINE_ROLE(");
        RewriteDeclWriter.InsertText(
                ClassDecl->getLocation().getLocWithOffset(ClassName.getAsString().length()), ")");
    }

    for (auto&& decls: ClassDecl->decls()) {
        if (auto Access = dyn_cast<AccessSpecDecl>(decls); Access) {
            RewriteDeclWriter.RemoveText(Access->getSourceRange(), rmOps);
        }
        if (auto F = dyn_cast<CXXMethodDecl>(decls); F) {
            if (!F->isImplicit() && (dyn_cast<CXXDestructorDecl>(F) || dyn_cast<CXXConstructorDecl>(F))) {
                auto R = F->getSourceRange();
                R.setEnd(R.getEnd().getLocWithOffset(1)); // eat ';'
                RewriteDeclWriter.RemoveText(R, rmOps);
            }
            if (F->isPure()) {
                const TypeSourceInfo* TSI = F->getTypeSourceInfo();
                if (auto Proto = TSI->getType()->getAs<FunctionProtoType>();
                        Proto && Proto->hasTrailingReturn()) { continue; }
                auto FTL = TSI->getTypeLoc().getAs<FunctionTypeLoc>();
                auto TrailingPos = Lexer::getLocForEndOfToken(FTL.getRParenLoc(), 0, SM, Ctx->getLangOpts());
                SourceRange EqualZero;
                // skip trailing cv && noexception
                {
                    auto&& [FileId, offset] = SM.getDecomposedLoc(TrailingPos);
                    StringRef File = SM.getBufferData(FileId);
                    Lexer lexer(SM.getLocForStartOfFile(FileId),
                            Ctx->getLangOpts(),
                            File.begin(),
                            File.data() + offset,
                            File.end());
                    Token T;
                    bool find = false;
                    while (! lexer.LexFromRawLexer(T) && !find) {
                        if (T.is(tok::raw_identifier)) {
                            auto& Info = Ctx->Idents.get(StringRef(SM.getCharacterData(T.getLocation()), T.getLength()));
                            T.setKind(Info.getTokenID());
                        }
                        switch (T.getKind()) {
                            case tok::amp:
                            case tok::ampamp:
                            case tok::kw_const:
                            case tok::kw_volatile:
                            case tok::kw_noexcept:
                                TrailingPos = T.getEndLoc();
                                break;
                            case tok::equal:
                                EqualZero.setBegin(T.getLocation().getLocWithOffset(T.hasLeadingSpace() ? -1 : 0));
                                break;
                            case tok::numeric_constant:
                                EqualZero.setEnd(T.getLocation());
                                find = true;
                                break;
                            default:
                                break;
                        }
                    }
                }
                auto ReturnTypeRange = F->getReturnTypeSourceRange();
                SourceLocation VirtualLoc = F->getBeginLoc();
                ReturnTypeRange.setBegin(VirtualLoc.getLocWithOffset(strlen("virtual")));
                StringRef ReturnType = Lexer::getSourceText(
                        Lexer::getAsCharRange(ReturnTypeRange, SM, Ctx->getLangOpts()),
                        SM, Ctx->getLangOpts());
                RewriteDeclWriter.ReplaceText(ReturnTypeRange, "");
                RewriteDeclWriter.ReplaceText(VirtualLoc, strlen("virtual "), "ABSTRACT(");
                RewriteDeclWriter.ReplaceText(EqualZero, ")");
                RewriteDeclWriter.InsertText(TrailingPos, " ->" + ReturnType.str());
            }
        }
    }
}

struct RewriteDecl: PluginASTAction {
    bool ParseArgs(const CompilerInstance &,
            const std::vector<std::string> &) override {
        return true;
    }
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
            StringRef file) override {
        RewriteDeclWriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<RewriteDeclConsumer>(RewriteDeclWriter);
    }
    private:
    Rewriter RewriteDeclWriter;
};

static FrontendPluginRegistry::Add<RewriteDecl> X
("RewriteDecl", "Refactor Interface by DEFINE_ROLE");

