#include "lisp.h"
#include <editline/readline.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    puts("Lisp interpreter written in c by netcan");
    puts("Press Ctrl-C to exit\n");
    char result[10240];
    while(lispc_runing) {
        char* input = readline("lispc> ");
        add_history(input);
        if(input == NULL) {
            puts("bye!");
            break;
        }
        if(lispc(input, result, 1) != LISPC_PARSE_ERROR) {
            puts(result);
        }

        free(input);
    }

    return 0;
}

