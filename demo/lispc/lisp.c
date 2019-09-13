/* author: netcan
 * date: 2019/09/13 18:04
 * @shanghai
 */
#include "lisp.h"
#include <mpc.h>
#include <stdio.h>
#include <stdlib.h>
lval* lval_num(long x) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval* lval_err(const char* m) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(strlen(m) + 1);
    strcpy(v->err, m);
    return v;
}

lval* lval_sym(const char* s) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_sexpr(void) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SEXPR;
    v->cell_count = 0;
    v->cell = NULL;
    return v;
}

lval* lval_qexpr(void) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_QEXPR;
    v->cell_count = 0;
    v->cell = NULL;
    return v;
}

void lval_del(lval* v) {
    switch (v->type) {
        case LVAL_NUM: break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            for (int i = 0; i < v->cell_count; ++i) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
    }
    free(v);
}

lval *lval_add(lval* v, lval *x) {
    v->cell = realloc(v->cell, sizeof(lval*) * (v->cell_count + 1));
    v->cell[v->cell_count++] = x;
    return v;
}

lval* lval_read_num(const mpc_ast_t* t) {
    // number
    errno = 0;
    long lhs = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(lhs) : lval_err("invalid number");
}

lval* lval_read(const mpc_ast_t* t) {
    // <number>
    if(strstr(t->tag, "number") != NULL) { return lval_read_num(t); }
    if(strstr(t->tag, "symbol") != NULL) { return lval_sym(t->contents); }

    lval *x = NULL;
    // root or sexpr to create S-Expr
    if(strcmp(t->tag, ">") == 0 || strstr(t->tag, "sexpr") != NULL) { x = lval_sexpr(); }
    if(strstr(t->tag, "qexpr") != NULL) { x = lval_qexpr(); }

    for(int i = 0; i < t->children_num; ++i) {
        if ( (strlen(t->children[i]->contents) == 1 && strstr("(){}", t->children[i]->contents) != NULL) ||
                strcmp(t->children[i]->tag, "regex") == 0) {
            continue;
        }
        x = lval_add(x, lval_read(t->children[i]));
    }
    return x;
}

char* lval_sprint(const lval* val, char *buffer);
char* lval_expr_sprint(const lval* v, char begin, char end, char *buffer) {
    *buffer++ = begin;
    for(int i = 0; i < v->cell_count; ++i) {
        buffer = lval_sprint(v->cell[i], buffer);
        if(i != v->cell_count - 1) *buffer++ = ' ';
    }
    *buffer++ = end;
    return buffer;
}

char* lval_sprint(const lval* val, char *buffer) {
    switch (val->type) {
        case LVAL_NUM: buffer += sprintf(buffer, "%ld", val->num); break;
        case LVAL_ERR: buffer += sprintf(buffer, "Errors: %s", val->err); break;
        case LVAL_SYM: buffer += sprintf(buffer, "%s", val->sym); break;
        case LVAL_SEXPR: buffer = lval_expr_sprint(val, '(', ')', buffer); break;
        case LVAL_QEXPR: buffer = lval_expr_sprint(val, '{', '}', buffer); break;
    }
    *buffer = '\0';
    return buffer;
}

void lval_println(const lval* val) {
    char lval_str[1024];
    lval_sprint(val, lval_str);
    puts(lval_str);
}

lval* lval_pop(lval* v, int i) {
    lval* x = v->cell[i];
    memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->cell_count - 1 - i));
    v->cell = realloc(v->cell, (--v->cell_count) * sizeof(lval*));
    return x;
}

lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

lval* lval_join(lval* lhs, lval* rhs) {
    while (rhs->cell_count > 0) {
        lhs = lval_add(lhs, lval_pop(rhs, 0));
    }

    lval_del(rhs);
    return lhs;
}

#define LASSERT(arg, cond, err) do {                      \
    if( !(cond)) { lval_del(arg); return lval_err(err); } \
} while(0)

lval* lval_eval_sexpr(lval* v);

// v->cell_count > 0
lval* builtin_car(lval* v) {
    LASSERT(v, v->cell_count == 1,             "Function 'car' passed too many arguments!");
    LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'car' passed incorrect types!");
    LASSERT(v, v->cell[0]->cell_count > 0,     "Function 'car' passed {}!");

    lval* val = lval_take(v, 0);
    while (val->cell_count > 1) {
        lval_del(lval_pop(val, 1));
    }
    return val;
}

lval* builtin_cdr(lval* v) {
    LASSERT(v, v->cell_count == 1,             "Function 'cdr' passed too many arguments!");
    LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'cdr' passed incorrect types!");
    LASSERT(v, v->cell[0]->cell_count > 0,     "Function 'cdr' passed {}!");

    lval *val = lval_take(v, 0);
    lval_del(lval_pop(val, 0));

    return val;
}

lval* builtin_list(lval *v) {
    v->type = LVAL_QEXPR;
    return v;
}

lval* builtin_eval(lval *v) {
    LASSERT(v, v->cell_count == 1,             "Function 'eval' passed too many arguments!");
    LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed incorrect type!");

    /* lval_println(v); */
    lval *val = lval_take(v, 0);
    val->type = LVAL_SEXPR;
    /* lval_println(val); */
    return lval_eval_sexpr(val);
}

lval* builtin_join(lval *v) {
    for (int i = 0; i < v->cell_count; ++i) {
        LASSERT(v->cell[i], v->cell[i]->type == LVAL_QEXPR, "Function 'join' passed incorrect type.");
    }
    lval* lhs = lval_pop(v, 0);
    while (v->cell_count > 0)
        lhs = lval_join(lhs, lval_pop(v, 0));

    lval_del(v);
    return lhs;
}


lval* builtin_op(lval* v, char* sym) {
    // v is s-expr
    for(int i = 0; i < v->cell_count; ++i) {
        if (v->cell[i]->type != LVAL_NUM) {
            lval_del(v);
            return lval_err("Cannot operate on non-number!");
        }
    }
    lval *lhs = lval_pop(v, 0);

    if (strcmp(sym, "-") == 0 && v->cell_count == 0) {
        lhs->num = -lhs->num;
    }

    while(v->cell_count > 0) {
        lval *rhs = lval_pop(v, 0);
        if (strcmp(sym, "+")   == 0) { lhs->num = lhs->num + rhs->num; }
        if (strcmp(sym, "-")   == 0) { lhs->num = lhs->num - rhs->num; }
        if (strcmp(sym, "*")   == 0) { lhs->num = lhs->num * rhs->num; }
        if (strcmp(sym, "^")   == 0) { lhs->num = pow(lhs->num, rhs->num); }
        if (strcmp(sym, "min") == 0) { lhs->num = lhs->num > rhs->num ? rhs->num : lhs->num; }
        if (strcmp(sym, "max") == 0) { lhs->num = lhs->num > rhs->num ? lhs->num : rhs->num; }
        if (strcmp(sym, "/")   == 0 || strcmp(sym, "%") == 0) {
            if(rhs->num == 0) {
                lval_del(lhs); lval_del(rhs);
                lhs = lval_err("Division By Zero!");
                break;
            }
            lhs->num = strcmp(sym, "/") == 0 ?
                lhs->num / rhs->num : lhs->num % rhs->num;
        }
        lval_del(rhs);
    }

    lval_del(v);
    return lhs;
}

#define REG_FUNC(v, func, alias, target) do {                              \
    if (strcmp(func, #alias) == 0) { return builtin_##target(v); } \
} while(0)

lval* builtin(lval* v, char* func) {
    REG_FUNC(v, func, list, list);
    REG_FUNC(v, func, car,  car);
    REG_FUNC(v, func, head,  car);
    REG_FUNC(v, func, cdr,  cdr);
    REG_FUNC(v, func, tail,  cdr);
    REG_FUNC(v, func, eval, eval);
    REG_FUNC(v, func, join, join);
    if (strstr("+-*/%^minmax", func) != NULL) { return builtin_op(v, func); }

    lval_del(v);
    return lval_err("Unknown Function!");
}

lval* lval_eval(lval* v) {
    if (v->type == LVAL_SEXPR) return lval_eval_sexpr(v);
    return v;
}

lval* lval_eval_sexpr(lval* v) {
    for(int i = 0; i < v->cell_count; ++i)
        v->cell[i] = lval_eval(v->cell[i]);

    for(int i = 0; i < v->cell_count; ++i)
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }

    if(v->cell_count == 0) return v;
    if(v->cell_count == 1) { return lval_take(v, 0); }

    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_SYM) {
        lval_del(f); lval_del(v);
        return lval_err("S-expression Does not start with symbol!");
    }

    lval* result = builtin(v, f->sym);
    lval_del(f);
    return result;
}

static mpc_parser_t *Number = NULL;
static mpc_parser_t *Symbol = NULL;
static mpc_parser_t *Sexpr  = NULL;
static mpc_parser_t *Qexpr  = NULL;
static mpc_parser_t *Expr   = NULL;
static mpc_parser_t *Lispc  = NULL;

__attribute__ ((constructor))
static void lispc_init() {
    Number = mpc_new("number");
    Symbol = mpc_new("symbol");
    Sexpr  = mpc_new("sexpr");
    Qexpr  = mpc_new("qexpr");
    Expr   = mpc_new("expr");
    Lispc  = mpc_new("lispc");

    mpca_lang(MPCA_LANG_DEFAULT, "                         \
            number: /-?[0-9]+/;                            \
            symbol: '+' | '-' | '*' | '/' | '^' | '%'      \
                    | /[a-zA-Z]+[a-zA-Z0-9]*/ ;            \
            sexpr: '(' <expr>* ')' ;                       \
            qexpr: '{' <expr>* '}' ;                       \
            expr: <number> | <symbol> | <sexpr> | <qexpr>; \
            lispc: /^/ <expr>* /$/;                        \
            ", Number, Symbol, Sexpr, Qexpr, Expr, Lispc);
}

__attribute__ ((destructor))
static void lispc_dtor() {
    mpc_cleanup(5, Number, Symbol, Sexpr, Qexpr, Expr, Lispc);
}

int lispc(const char *input, char* result_str, int verbose) {
    mpc_result_t r;
    if (mpc_parse("<lispc>", input, Lispc, &r)) {
        if(verbose) mpc_ast_print(r.output);

        lval* x = lval_read(r.output);
        if(verbose) lval_println(x);

        lval* result = lval_eval(x);
        lval_sprint(result, result_str);

        lval_del(result);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
        return 0;
    }
    return 1;
}
