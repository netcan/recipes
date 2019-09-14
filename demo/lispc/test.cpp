/* author: netcan
 * date: 2019/09/14 16:12
 * @shanghai
 */
#include <gtest/gtest.h>
#include "lisp.h"

TEST(LispCTest, lispc) {
    char result[10240];
    EXPECT_EQ(lispc("list 1 2 3 4", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{1 2 3 4}");

    EXPECT_EQ(lispc("{head (list 1 2 3 4)}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{head (list 1 2 3 4)}");

    EXPECT_EQ(lispc("eval {head (list 1 2 3 4)}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{1}");

    EXPECT_EQ(lispc("tail {tail tail tail}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{tail tail}");

    EXPECT_EQ(lispc("eval (tail {tail tail {5 6 7}})", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{6 7}");

    EXPECT_EQ(lispc("eval (head {(+ 1 2) (+ 10 20)})", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "3");

    EXPECT_EQ(lispc("join {1 2 3} {4 5 6} {2}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{1 2 3 4 5 6 2}");

    EXPECT_EQ(lispc("+ 5 2", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "7");

    EXPECT_EQ(lispc("- 5 2", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "3");

    EXPECT_EQ(lispc("* 5 2", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "10");

    EXPECT_EQ(lispc("/ 5 2", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "2");

    EXPECT_EQ(lispc("^ 5 2", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "25");

    EXPECT_EQ(lispc("min (max 1 2 1 4) 2", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "2");

    EXPECT_EQ(lispc("/ 5 0", result, 0), LISPC_RUNTIME_ERROR);
    EXPECT_EQ(lispc("% 5 0", result, 0), LISPC_RUNTIME_ERROR);

    EXPECT_EQ(lispc("2 2", result, 0), LISPC_RUNTIME_ERROR);

    EXPECT_EQ(lispc("- (* 10 10) (+ 1 1 1)", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "97");

    EXPECT_EQ(lispc("+ 1 (* 7 5) 3", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "39");

    EXPECT_EQ(lispc("-100", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "-100");

    EXPECT_EQ(lispc("cons 1 2", result, 0), LISPC_RUNTIME_ERROR);

    EXPECT_EQ(lispc("cons", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "<builtin-in function cons>");

    EXPECT_EQ(lispc("cons 1 {2}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{1 2}");

    EXPECT_EQ(lispc("cons 1 {}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{1}");

    EXPECT_EQ(lispc("cons (+ 1 3) {2 {3 4}}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{4 2 {3 4}}");

    EXPECT_EQ(lispc("(cons 1 (cons 1 {23 4 5 6 6}))", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{1 1 23 4 5 6 6}");

    EXPECT_EQ(lispc("eval (cons + {1 2 3})", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "6");

    EXPECT_EQ(lispc("len {1 2 3 4}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "4");

    EXPECT_EQ(lispc("len {}", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "0");

    EXPECT_EQ(lispc("(eval (car {+ - + - * /})) 10 20", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "30");

    EXPECT_EQ(lispc("let {x} 100", result, 0), LISPC_RET_OK);
    EXPECT_EQ(lispc("let {y} 200", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "()");
    EXPECT_EQ(lispc("x", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "100");
    EXPECT_EQ(lispc("+ x y", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "300");

    EXPECT_EQ(lispc("let {arglist} {a b x y}", result, 0), LISPC_RET_OK);
    EXPECT_EQ(lispc("arglist", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "{a b x y}");

    EXPECT_EQ(lispc("let arglist 1 2 3 4", result, 0), LISPC_RET_OK);
    EXPECT_EQ(lispc("x", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "3");

    EXPECT_EQ(lispc("", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "()");

    EXPECT_EQ(lispc("", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "()");

    EXPECT_EQ(lispc("", result, 0), LISPC_RET_OK);
    EXPECT_STREQ(result, "()");


}
