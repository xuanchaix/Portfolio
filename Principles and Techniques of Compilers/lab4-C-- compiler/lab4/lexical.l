
%{
    #include "syntax.tab.h"
    #include "lex.yy.h"

    extern int lexical_error;
    int lexical_error = 0;
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    
    /*typedef struct Node{
        char name[32];
        struct{
            int int_val;
            float float_val;
            char char_val[32];
        }val;

        struct Node *first_child;
        struct Node *next_brother;

        int column;
        int leaf;
    }Node;*/

    struct Node* generate_node(char *name, int col, int int_val, float float_val, char *char_val){
        struct Node *node = (struct Node *)malloc(sizeof(struct Node));
        strcpy(node->name, name);
        node->first_child = NULL;
        node->next_brother = NULL;
        node->column = col;
        node->val.int_val = 0;
        node->val.float_val = 0;
        strcpy(node->val.char_val, "");
        
        if(int_val){
            node->val.int_val = int_val;
            node->leaf = 1;
        }
        else if(float_val){
            node->val.float_val = float_val;
            node->leaf = 2;
        }
        else if(char_val){
            strcpy(node->val.char_val, char_val);
            node->leaf = 3;
        }
        return node;
    }

    Node* get_n_child(Node* root, int n){
        if(root){
            Node* cur = root->first_child;
            while(cur != NULL && n > 0){
                cur = cur->next_brother;
                --n;
            }
            return cur;
        }
        else{
            return NULL;
        }
    }
%}

digit [0-9]
letter [_a-zA-Z]

%%
0|[1-9]{digit}* {
    yylval.Node = generate_node("INT", yylineno, atoi(yytext), 0, yytext);
    return INT;}
0[xX][0-9a-fA-F]+ {
    yylval.Node = generate_node("INT", yylineno, 1, 0, yytext);
    sscanf(yytext, "%x", &yylval.Node->val.int_val);
    return INT;
}
0[xX][0-9a-zA-Z]+ {
    yylval.Node = generate_node("INT", yylineno, 0, 0, yytext);
    lexical_error = 1;
    printf("Error type A at line %d: Illegal hexadecimal number \'%s\'\n", yylineno, yytext);
    return INT;
}
0[0-7]+ {
    yylval.Node = generate_node("INT", yylineno, 1, 0, yytext);
    sscanf(yytext, "%o", &yylval.Node->val.int_val);
    return INT;
}
0[0-9]+ {
    yylval.Node = generate_node("INT", yylineno, 0, 0, yytext);
    lexical_error = 1;
    printf("Error type A at line %d: Illegal octal number \'%s\'\n", yylineno, yytext);
    return INT;
}
{digit}*"."{digit}+|{digit}+"." {
    yylval.Node = generate_node("FLOAT", yylineno, 0, atof(yytext), yytext);
    return FLOAT;}
{digit}*"."{digit}*[eE][+-]?{digit}+ {
    yylval.Node = generate_node("FLOAT", yylineno, 0, atof(yytext), yytext);
    sscanf(yytext, "%f", &yylval.Node->val.float_val);
    return FLOAT;}
{digit}*"."{digit}*[eE][+-]?{digit}* {
    yylval.Node = generate_node("FLOAT", yylineno, 0, atof(yytext), yytext);
    lexical_error = 1;
    printf("Error type A at line %d: Illegal floating point number \'%s\'\n", yylineno, yytext);
    return FLOAT;}
"//" {
    char c = input();
    while(c != '\n'){
        c = input();
    }
    yylineno++;
}
"/*" {
    char c = input();
    char prev_c = 0;
    while(c != '/' || prev_c != '*'){
        if(c == '\n'){
            yylineno++;
        }
        prev_c = c;
        c = input();
    }
}
; {
    yylval.Node = generate_node("SEMI", yylineno, 0, 0, ";");
    return SEMI;}
, {
    yylval.Node = generate_node("COMMA", yylineno, 0, 0, ",");
    return COMMA;}
= {
    yylval.Node = generate_node("ASSIGNOP", yylineno, 0, 0, "=");
    return ASSIGNOP;}
>|<|>=|<=|==|!= {
    yylval.Node = generate_node("RELOP", yylineno, 0, 0, yytext);
    return RELOP;}
"+" {
    yylval.Node = generate_node("ADD", yylineno, 0, 0, "+");
    return ADD;}
"-" {
    yylval.Node = generate_node("SUB", yylineno, 0, 0, "-");
    return SUB;}
"*" {
    yylval.Node = generate_node("MUL", yylineno, 0, 0, "*");
    return MUL;}
"/" {
    yylval.Node = generate_node("DIV", yylineno, 0, 0, "/");
    return DIV;}
"&&" {
    yylval.Node = generate_node("AND", yylineno, 0, 0, "&&");
    return AND;}
"||" {
    yylval.Node = generate_node("OR", yylineno, 0, 0, "||");
    return OR;}
"." {
    yylval.Node = generate_node("DOT", yylineno, 0, 0, ".");
    return DOT;}
"!" {
    yylval.Node = generate_node("NOT", yylineno, 0, 0, "!");
    return NOT;}
int|float {
    yylval.Node = generate_node("TYPE", yylineno, 0, 0, yytext);
    return TYPE;}
"(" {
    yylval.Node = generate_node("LP", yylineno, 0, 0, "(");
    return LP;}
")" {
    yylval.Node = generate_node("RP", yylineno, 0, 0, ")");
    return RP;}
"[" {
    yylval.Node = generate_node("LB", yylineno, 0, 0, "[");
    return LB;}
"]" {
    yylval.Node = generate_node("RB", yylineno, 0, 0, "]");
    return RB;}
"{" {
    yylval.Node = generate_node("LC", yylineno, 0, 0, "{");
    return LC;}
"}" {
    yylval.Node = generate_node("RC", yylineno, 0, 0, "}");
    return RC;}
struct {
    yylval.Node = generate_node("STRUCT", yylineno, 0, 0, "struct");
    return STRUCT;}
return {
    yylval.Node = generate_node("RETURN", yylineno, 0, 0, "return");
    return RETURN;}
if {
    yylval.Node = generate_node("IF", yylineno, 0, 0, "if");
    return IF;}
else {
    yylval.Node = generate_node("ELSE", yylineno, 0, 0, "else");
    return ELSE;}
while {
    yylval.Node = generate_node("WHILE", yylineno, 0, 0, "while");
    return WHILE;}
{letter}({letter}|{digit})* {
    yylval.Node = generate_node("ID", yylineno, 0, 0, yytext);
    return ID;}
"\n" {yylineno++; yycolumn = 1;}
[ \t\r]+ {;}
. {lexical_error = 1;
    printf("Error type A at line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}
%%
