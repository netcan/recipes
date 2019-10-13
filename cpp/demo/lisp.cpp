// https://leetcode-cn.com/problems/parse-lisp-expression/
// Author: netcan
#include <iostream>
#include <string>
#include <cassert>
#include <gtest/gtest.h>
#include <map>
using namespace std;

// LISP: (Expr) | CONST
// Expr: add Lisp Lisp | mult Lisp Lisp | let (Var Lisp)+ Lisp
// Var: [a-z][a-z0-9]*
// CONST: -?[0-9]+

class Lisp {
public:
    explicit Lisp(const char* lisp): curT(Token::INVALID), p(lisp) { getToken(); }
    operator int() { return lisp(); }
private:
    int lisp(std::map<string, int>* symTab = nullptr);
    int expr(std::map<string, int>* symTab = nullptr);

    enum class Token {
        VAR,
        CONST,
        PAREN,
        ADD,
        MULT,
        LET,
        INVALID,
    };
    void getToken();
    Token curT;
    int curVal;
    string curVar;
    const char* p;
};

void Lisp::getToken() {
    while(*p == ' ') ++p; // eat ' '

    if (isdigit(*p) || *p == '-') {
        bool sign = false;
        if(*p == '-') { sign = true; ++p; }
        curT = Token::CONST;
        curVal = 0;
        while(isdigit(*p)) {
            curVal = curVal * 10 + (sign ? -(*p - '0') : (*p - '0'));
            ++p;
        }
        return;
    }

    if (isalpha(*p)) {
        curT = Token::VAR;
        curVar.clear();
        while(isalnum(*p)) { curVar += *p++; }
        if(curVar == "mult") { curT = Token::MULT; }
        if(curVar == "add") { curT = Token::ADD; }
        if(curVar == "let") { curT = Token::LET; }
        return;
    }

    if (*p == '(' || *p == ')') {
        curT = Token::PAREN;
        curVal = *p++;
        return;
    }
}

// LISP: (Expr) | CONST | VAR
//
int Lisp::lisp(std::map<string, int>* symTab) {
    if(curT == Token::CONST) {
        int val = curVal;
        getToken(); // eat var
        return val;
    }
    if(curT == Token::VAR) {
        int val = symTab->at(curVar);
        getToken(); // eat var
        return val;
    }
    if(curT == Token::PAREN && curVal == '(') {
        getToken(); // eat '('
        int val = expr(symTab);
        getToken(); // eat ')'
        return val;
    }
    return -1;
}

// Expr: add Lisp Lisp | mult Lisp Lisp | let (Var Lisp)* Lisp
int Lisp::expr(std::map<string, int>* symTab) {
    // getToken(); // eat op
    if(curT == Token::ADD) {
        getToken(); // eat Add
        int lhs = lisp(symTab);
        int rhs = lisp(symTab);
        return lhs + rhs;
    }
    if(curT == Token::MULT) {
        getToken(); // eat Mult
        int lhs = lisp(symTab);
        int rhs = lisp(symTab);
        return lhs * rhs;
    }
    if(curT == Token::LET) {
        std::map<string, int> st;
        if(symTab != nullptr) { st.insert(symTab->begin(), symTab->end()); }
        getToken(); // eat let
        string lastVar;
        while(curT == Token::VAR) {
            string var = curVar;
            lastVar = curVar;
            getToken(); // eat varname
            if(curT == Token::CONST) {
                st[var] = curVal;
                getToken(); // eat varval
            } else if(curT == Token::VAR) {
                st[var] = st[curVar];
                getToken(); // eat varName
            } else if(curT == Token::PAREN) {
                if(curVal == '(') {
                    st[var] = lisp(&st);
                } else {
                    break;
                }
            }
        }
        if(curT == Token::PAREN) {
            if(curVal == '(') return lisp(&st);
            else if(curVal == ')') return st.at(lastVar);
        }
        return lisp(&st);
    }
    return lisp(symTab);
}

TEST(lispcpp, test) {
    EXPECT_EQ(Lisp("5")                                                  == 5,    true);
    EXPECT_EQ(Lisp("(5)")                                                == 5,    true);
    EXPECT_EQ(Lisp("((5))")                                              == 5,    true);
    EXPECT_EQ(Lisp("(add 5 6)")                                          == 11,   true);
    EXPECT_EQ(Lisp("(let x 2 x)")                                        == 2,    true);
    EXPECT_EQ(Lisp("(mult 3 (add 2 3))")                                 == 15,   true);
    EXPECT_EQ(Lisp("(let x 2 (mult x 5))")                               == 10,   true);
    EXPECT_EQ(Lisp("(let x 2 (mult x (let x 3 y 4 (add x y))))")         == 14,   true);
    EXPECT_EQ(Lisp("(let x 3 x 2 x)")                                    == 2,    true);
    EXPECT_EQ(Lisp("(let x 1 y 2 x (add x y) (add x y))")                == 5,    true);
    EXPECT_EQ(Lisp("(let x 2 (add (let x 3 (let x 4 x)) x))")            == 6,    true);
    EXPECT_EQ(Lisp("(let a1 3 b2 (add a1 1) b2)")                        == 4,    true);
    EXPECT_EQ(Lisp("(let x -2 y x y)")                                   == -2,   true);
    EXPECT_EQ(Lisp("(let var 78 b 77 (let c 33 (add c (mult var 66))))") == 5181, true);
}
