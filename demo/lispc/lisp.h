/* author: netcan
 * date: 2019/09/13 18:04
 * @shanghai
 */
#pragma once
enum { LVAL_ERR,  LVAL_NUM,   LVAL_SYM,
       LVAL_FUNC, LVAL_SEXPR, LVAL_QEXPR };

enum {
    LISPC_RET_OK        = 0,
    LISPC_PARSE_ERROR   = 1,
    LISPC_RUNTIME_ERROR = 2
};

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;
typedef lval* (*lbuiltin)(lenv*, lval*);

struct lval {
    int type;
    union {
        long num;
        char* err;
        char* sym;
        struct {
            int cell_count;
            struct lval **cell;
        };
        struct {
            char* fname;
            lbuiltin func;
        };
    };
};

struct lenv {
    int count;
    char** syms;
    lval** vals;
};

#define LASSERT(v, cond, fmt, ...) do {           \
    if( !(cond)) {                                \
        lval* err = lval_err(fmt, ##__VA_ARGS__); \
        lval_del(v);                              \
        return err;                               \
    }                                             \
} while(0)

#define LASSERT_ARG_NUM(func, v, count) do {                                            \
    LASSERT(v, v->cell_count == count,                                                  \
            "Function `%s' passed incorrect number of arguments. Got %d, Expected %d.", \
            func, v->cell_count, count);                                                \
} while(0)

#define LASSERT_TYPE(func, v, index, expect) do {                   \
    LASSERT(v, v->cell[index]->type == expect,                      \
            "Function `%s' passed incorrect type for argument %d. " \
            "Got %s, Expected %s.", func, index,                    \
            ltype_name[v->cell[index]->type], ltype_name[expect]    \
            );                                                      \
} while(0)

#define LASSERT_NOT_EMPTY(func, v, index) do {                        \
    LASSERT(v, v->cell[index]->cell_count != 0,                       \
            "Function `%s' passed {} for argument %i.", func, index); \
} while(0)

#ifdef __cplusplus
extern "C" {
#endif

int lispc(const char *, char*, int);
extern int lispc_runing;

#ifdef __cplusplus
}
#endif
