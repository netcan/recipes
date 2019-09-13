#include <gtest/gtest.h>
#include "lisp.h"

TEST(LispCTest, lispc) {
    char result[10240];
    EXPECT_NE(lispc("list 1 2 3 4", result, 0), 0);
    EXPECT_STREQ(result, "{1 2 3 4}");

    EXPECT_NE(lispc("{head (list 1 2 3 4)}", result, 0), 0);
    EXPECT_STREQ(result, "{head (list 1 2 3 4)}");

    EXPECT_NE(lispc("eval {head (list 1 2 3 4)}", result, 0), 0);
    EXPECT_STREQ(result, "{1}");

    EXPECT_NE(lispc("tail {tail tail tail}", result, 0), 0);
    EXPECT_STREQ(result, "{tail tail}");

    EXPECT_NE(lispc("eval (tail {tail tail {5 6 7}})", result, 0), 0);
    EXPECT_STREQ(result, "{6 7}");

    EXPECT_NE(lispc("eval (head {(+ 1 2) (+ 10 20)})", result, 0), 0);
    EXPECT_STREQ(result, "3");

    EXPECT_NE(lispc("/ 5 2", result, 0), 0);
    EXPECT_STREQ(result, "2");

    EXPECT_NE(lispc("/ 5 0", result, 0), 0);
    EXPECT_STREQ(result, "Errors: Division By Zero!");

    EXPECT_NE(lispc("% 5 0", result, 0), 0);
    EXPECT_STREQ(result, "Errors: Division By Zero!");

    EXPECT_NE(lispc("2 2", result, 0), 0);
    EXPECT_STREQ(result, "Errors: S-expression Does not start with symbol!");

    EXPECT_NE(lispc("- (* 10 10) (+ 1 1 1)", result, 0), 0);
    EXPECT_STREQ(result, "97");

    EXPECT_NE(lispc("+ 1 (* 7 5) 3", result, 0), 0);
    EXPECT_STREQ(result, "39");

    EXPECT_NE(lispc("-100", result, 0), 0);
    EXPECT_STREQ(result, "-100");


}
