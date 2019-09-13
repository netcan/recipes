/* author: netcan
 * date: 2019/09/13 18:04
 * @shanghai
 */
#pragma once
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

typedef struct lval {
    int type;
    union {
        long num;
        char* err;
        char* sym;
        struct {
            int cell_count;
            struct lval **cell;
        };
    };
} lval;

#ifdef __cplusplus
extern "C" {
#endif

int lispc(const char *, char*, int);

#ifdef __cplusplus
}
#endif
