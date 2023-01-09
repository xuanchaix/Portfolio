#include "syntax.tab.h"
//#include "sdt.h"
#include <stdio.h>

void yyrestart();
void print_tree();
void sdt_main();
int yydebug;
int lexical_error;
int syntax_error;
struct Node *root;

int main(int argc, char** argv){
    if(argc <= 1){
        return 1;
    }
    FILE* f = fopen(argv[1], "r");
    if(!f){
        perror(argv[1]);
        return 1;
    }
    yydebug = 1;
    yyrestart(f);
    yyparse();
    if(!lexical_error && !syntax_error){
        //print_tree(root, 0);
        sdt_main(root);
    }
    return 0;
}
