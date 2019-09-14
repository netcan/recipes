/* author: netcan
 * date: 2019/09/13 18:04
 * @shanghai
 */
#include "lisp.h"
#include <mpc.h>
#include <stdio.h>
#include <stdlib.h>

static const char* ltype_name[] = {
    [LVAL_ERR]   = "Error",
    [LVAL_NUM]   = "Number",
    [LVAL_SYM]   = "Symbol",
    [LVAL_FUNC]  = "Function",
    [LVAL_QEXPR] = "Q-Expression",
    [LVAL_SEXPR] = "S-Expression",
};

lval* lval_num(long x) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_NUM;
    v->num = x;
    return v;
}

lval* lval_err(const char* fmt, ...) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_ERR;
    v->err = malloc(1024);

    va_list va;
    va_start(va, fmt);
    vsnprintf(v->err, 1023, fmt, va);
    va_end(va);

    v->err = realloc(v->err, strlen(v->err) + 1);
    return v;
}

lval* lval_sym(const char* s) {
    lval *v = malloc(sizeof(lval));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

lval* lval_func(lbuiltin func) {
    lval* v = malloc(sizeof(lval));
    v->type = LVAL_FUNC;
    v->func = func;
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
        case LVAL_FUNC: break;
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

lval* lval_copy(lval* v) {
    lval* x = malloc(sizeof(lval));
    x->type = v->type;
    switch (v->type) {
        case LVAL_NUM: x->num = v->num; break;
        case LVAL_FUNC: x->func = v->func; break;
        case LVAL_ERR:
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;
        case LVAL_SYM:
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;
        case LVAL_SEXPR:
        case LVAL_QEXPR:
            x->cell_count = v->cell_count;
            x->cell = malloc(sizeof(x->cell_count) * sizeof(lval*));
            for(int i = 0; i < x->cell_count; ++i) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
            break;
    }
    return x;
}

// v is LVAL_QEXPR or LVAL_SEXPR
lval *lval_add(lval* v, lval *x) {
    v->cell = realloc(v->cell, sizeof(lval*) * (v->cell_count + 1));
    v->cell[v->cell_count++] = x;
    return v;
}

lenv* lenv_new(void) {
    lenv* env = malloc(sizeof(lenv));
    env->count = 0;
    env->syms = NULL;
    env->vals = NULL;
    return env;
}

void lenv_del(lenv* env) {
    for(int i = 0; i < env->count; ++i) {
        free(env->syms[i]);
        lval_del(env->vals[i]);
    }
    free(env->syms);
    free(env->vals);
    free(env);
}

// v is LVAL_SYM
lval* lenv_get(lenv* env, lval* v) {
    for(int i = 0; i < env->count; ++i) {
        if (strcmp(env->syms[i], v->sym) == 0)
            return lval_copy(env->vals[i]);
    }
    return lval_err("unbound symbol!");
}

// k is LVAL_SYM
void lenv_put(lenv* env, lval* k, lval* v) {
    for(int i = 0; i < env->count; ++i) {
        // exist same key, replace it
        if (strcmp(env->syms[i], k->sym) == 0) {
            lval_del(env->vals[i]);
            env->vals[i] = lval_copy(v);
            return;
        }
    }
    ++env->count;
    env->syms = realloc(env->syms, env->count * sizeof(char*));
    env->vals = realloc(env->vals, env->count * sizeof(lval*));
    env->syms[env->count - 1] = malloc(strlen(k->sym) + 1);
    strcpy(env->syms[env->count - 1], k->sym);
    env->vals[env->count - 1] = lval_copy(v);
    return;
}

lval* lval_read_num(const mpc_ast_t* t) {
    // number
    errno = 0;
    long lhs = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(lhs) : lval_err("Invalid number.");
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
        case LVAL_NUM:   buffer += sprintf(buffer, "%ld", val->num)        ; break;
        case LVAL_ERR:   buffer += sprintf(buffer, "Errors: %s", val->err) ; break;
        case LVAL_SYM:   buffer += sprintf(buffer, "%s", val->sym)         ; break;
        case LVAL_FUNC:  buffer += sprintf(buffer, "<function>") ; break;
        case LVAL_SEXPR: buffer  = lval_expr_sprint(val, '(', ')', buffer) ; break;
        case LVAL_QEXPR: buffer  = lval_expr_sprint(val, '{', '}', buffer) ; break;
    }
    *buffer = '\0';
    return buffer;
}

void lval_println(const lval* val) {
    char lval_str[1024];
    lval_sprint(val, lval_str);
    puts(lval_str);
}

// v is LVAL_QEXPR or LVAL_SEXPR
lval* lval_pop(lval* v, int i) {
    lval* x = v->cell[i];
    memmove(&v->cell[i], &v->cell[i+1], sizeof(lval*) * (v->cell_count - 1 - i));
    v->cell = realloc(v->cell, (--v->cell_count) * sizeof(lval*));
    return x;
}

// v is LVAL_QEXPR or LVAL_SEXPR
lval* lval_take(lval* v, int i) {
    lval* x = lval_pop(v, i);
    lval_del(v);
    return x;
}

// lhs/rhs is LVAL_QEXPR or LVAL_SEXPR
lval* lval_join(lval* lhs, lval* rhs) {
    while (rhs->cell_count > 0) {
        lhs = lval_add(lhs, lval_pop(rhs, 0));
    }

    lval_del(rhs);
    return lhs;
}

lval* lval_eval_sexpr(lenv* env, lval* v);

// v->cell_count > 0
lval* builtin_car(lenv* env, lval* v) {
    LASSERT_ARG_NUM("car", v, 1);
    LASSERT_TYPE("car", v, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("car", v, 0);

    lval* val = lval_take(v, 0);
    while (val->cell_count > 1) {
        lval_del(lval_pop(val, 1));
    }
    return val;
}

lval* builtin_cdr(lenv* env, lval* v) {
    LASSERT_ARG_NUM("cdr", v, 1);
    LASSERT_TYPE("cdr", v, 0, LVAL_QEXPR);
    LASSERT_NOT_EMPTY("cdr", v, 0);

    lval *val = lval_take(v, 0);
    lval_del(lval_pop(val, 0));

    return val;
}

lval* builtin_list(lenv* env, lval *v) {
    v->type = LVAL_QEXPR;
    return v;
}

lval* builtin_eval(lenv* env, lval *v) {
    LASSERT_ARG_NUM("eval", v, 1);
    LASSERT_TYPE("eval", v, 0, LVAL_QEXPR);

    /* lval_println(v); */
    lval *val = lval_take(v, 0);
    val->type = LVAL_SEXPR;
    /* lval_println(val); */
    return lval_eval_sexpr(env, val);
}

lval* builtin_join(lenv* env, lval *v) {
    for (int i = 0; i < v->cell_count; ++i) {
        LASSERT_TYPE("join", v, i, LVAL_QEXPR);
    }
    lval* lhs = lval_pop(v, 0);
    while (v->cell_count > 0)
        lhs = lval_join(lhs, lval_pop(v, 0));

    lval_del(v);
    return lhs;
}

lval* builtin_cons(lenv* env, lval* v) {
    LASSERT_ARG_NUM("cons", v, 2);
    LASSERT_TYPE("cons", v, 1, LVAL_QEXPR);
    lval* val = lval_qexpr();
    lval_add(val, lval_pop(v, 0));
    lval* remain = lval_take(v, 0);
    while (remain->cell_count > 0) {
        lval_add(val, lval_pop(remain, 0));
    }
    return val;
}

lval* builtin_len(lenv* env, lval* v) {
    LASSERT_ARG_NUM("len", v, 1);
    LASSERT_TYPE("len", v, 0, LVAL_QEXPR);

    long len = v->cell[0]->cell_count;;
    lval_del(v);
    return lval_num(len);
}

lval* builtin_op(lval* v, char* sym) {
    // v is s-expr
    for(int i = 0; i < v->cell_count; ++i) {
        LASSERT_TYPE(sym, v, i, LVAL_NUM);
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
        if (strcmp(sym, "/")   == 0 ||
            strcmp(sym, "%")   == 0) {
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

lval* builtin_def(lenv* env, lval* v) {
    LASSERT_TYPE("def", v, 0, LVAL_QEXPR);
    // don't use lval_pop(v, 0), lassert will cause memory leak
    lval* syms = v->cell[0];
    LASSERT_ARG_NUM("def", v, syms->cell_count + 1);

    for(int i = 0; i < syms->cell_count; ++i) {
        LASSERT_TYPE("def", syms, i, LVAL_SYM);
    }
    for(int i = 0; i < syms->cell_count; ++i) {
        lenv_put(env, syms->cell[i], v->cell[i + 1]);
    }
    lval_del(v);
    return lval_sexpr();
}

lval* builtin_add(lenv* env, lval* v) { return builtin_op(v, "+"); }
lval* builtin_sub(lenv* env, lval* v) { return builtin_op(v, "-"); }
lval* builtin_mul(lenv* env, lval* v) { return builtin_op(v, "*"); }
lval* builtin_div(lenv* env, lval* v) { return builtin_op(v, "/"); }
lval* builtin_mod(lenv* env, lval* v) { return builtin_op(v, "%"); }
lval* builtin_pow(lenv* env, lval* v) { return builtin_op(v, "^"); }
lval* builtin_min(lenv* env, lval* v) { return builtin_op(v, "min"); }
lval* builtin_max(lenv* env, lval* v) { return builtin_op(v, "max"); }


void lenv_add_builtin(lenv* env, char* name, lbuiltin func) {
    lval* k = lval_sym(name);
    lval* v = lval_func(func);
    lenv_put(env, k, v);
    lval_del(k); lval_del(v);
    return;
}

void lenv_add_builtins(lenv* env) {
    lenv_add_builtin(env, "+",   builtin_add);
    lenv_add_builtin(env, "-",   builtin_sub);
    lenv_add_builtin(env, "*",   builtin_mul);
    lenv_add_builtin(env, "/",   builtin_div);
    lenv_add_builtin(env, "%",   builtin_mod);
    lenv_add_builtin(env, "^",   builtin_pow);
    lenv_add_builtin(env, "max", builtin_max);
    lenv_add_builtin(env, "min", builtin_min);

    lenv_add_builtin(env, "cons", builtin_cons);
    lenv_add_builtin(env, "car",  builtin_car);
    lenv_add_builtin(env, "cdr",  builtin_cdr);
    lenv_add_builtin(env, "head", builtin_car);
    lenv_add_builtin(env, "tail", builtin_cdr);
    lenv_add_builtin(env, "eval", builtin_eval);
    lenv_add_builtin(env, "list", builtin_list);
    lenv_add_builtin(env, "join", builtin_join);
    lenv_add_builtin(env, "len",  builtin_len);

    lenv_add_builtin(env, "def",  builtin_def);
    lenv_add_builtin(env, "let",  builtin_def);
}

lval* lval_eval(lenv* env, lval* v) {
    if (v->type == LVAL_SYM) {
        lval* x = lenv_get(env, v);
        lval_del(v);
        return x;
    }
    if (v->type == LVAL_SEXPR)
        return lval_eval_sexpr(env, v);
    return v;
}

lval* lval_eval_sexpr(lenv* env, lval* v) {
    for(int i = 0; i < v->cell_count; ++i)
        v->cell[i] = lval_eval(env, v->cell[i]);

    for(int i = 0; i < v->cell_count; ++i)
        if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }

    if(v->cell_count == 0) return v;
    if(v->cell_count == 1) { return lval_take(v, 0); }

    lval* f = lval_pop(v, 0);
    if (f->type != LVAL_FUNC) {
        lval_del(f); lval_del(v);
        return lval_err("S-expression Does not start with function!");
    }

    lval* result = f->func(env, v);
    lval_del(f);
    return result;
}

static mpc_parser_t *Number = NULL;
static mpc_parser_t *Symbol = NULL;
static mpc_parser_t *Sexpr  = NULL;
static mpc_parser_t *Qexpr  = NULL;
static mpc_parser_t *Expr   = NULL;
static mpc_parser_t *Lispc  = NULL;
static lenv *LispEnv        = NULL;

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
            symbol:  /[a-zA-Z0-9_+\\-*\\/\\^%\\\\=<>!&]+/; \
            sexpr: '(' <expr>* ')' ;                       \
            qexpr: '{' <expr>* '}' ;                       \
            expr: <number> | <symbol> | <sexpr> | <qexpr>; \
            lispc: /^/ <expr>* /$/;                        \
            ", Number, Symbol, Sexpr, Qexpr, Expr, Lispc);

    LispEnv = lenv_new();
    lenv_add_builtins(LispEnv);
}

__attribute__ ((destructor))
static void lispc_dtor() {
    mpc_cleanup(5, Number, Symbol, Sexpr, Qexpr, Expr, Lispc);
    lenv_del(LispEnv);
}

int lispc(const char *input, char* result_str, int verbose) {
    mpc_result_t r;
    int ret_code = LISPC_RET_OK;
    if (mpc_parse("<lispc>", input, Lispc, &r)) {
        if(verbose) mpc_ast_print(r.output);

        lval* x = lval_read(r.output);
        if(verbose) lval_println(x);

        lval* result = lval_eval(LispEnv, x);
        lval_sprint(result, result_str);

        if (result->type == LVAL_ERR)
            ret_code = LISPC_RUNTIME_ERROR;

        lval_del(result);
        mpc_ast_delete(r.output);
    } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
        ret_code = LISPC_PARSE_ERROR;
    }
    return ret_code;
}
