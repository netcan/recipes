/*************************************************************************
    > File Name: RefactorDeclMain.cpp
    > Author: Netcan
    > Descripton: RefactorDeclMain.cpp
    > Blog: https://netcan.github.io/
    > Mail: 1469709759@qq.com
    > Created Time: 2020-08-09 16:11
************************************************************************/
#include "RefactorDecl.hpp"

int main(int argc, const char** argv) {
    static llvm::cl::OptionCategory RewriteDeclCategory("RewriteDecl");
    auto OptionsParser = clang::tooling::CommonOptionsParser::create(argc, argv,
            RewriteDeclCategory);
    clang::tooling::RefactoringTool Tool(OptionsParser.get().getCompilations(),
            OptionsParser.get().getSourcePathList());
    return Tool.runAndSave(
            clang::tooling::newFrontendActionFactory<RewriteDecl>()
            .get());

    return 0;
}
