%{
    #include <stdio.h>
    #include "lex.yy.c"

    extern struct Node *root;
    void add_first_child_node(struct Node *parent_node, struct Node *child_node);
    void add_other_child_node(struct Node *older_node, struct Node *younger_node);

    void yyerror(char* error_msg);
    void yymissingerror(char* error);

    extern int syntax_error;
    int syntax_error = 0;
%}

%start Program

%union {
    struct Node *Node;
}

%token <Node>INT
%token <Node>FLOAT
%token <Node>COMMA
%token <Node>ASSIGNOP 
%token <Node>RELOP
%token <Node>ADD 
%token <Node>SUB 
%token <Node>MUL 
%token <Node>DIV
%token <Node>AND 
%token <Node>OR 
%token <Node>NOT 
%token <Node>LP 
%token <Node>RP 
%token <Node>LC 
%token <Node>RC
%token <Node>LB 
%token <Node>RB 
%token <Node>IF 
%token <Node>ELSE
%token <Node>WHILE 
%token <Node>SEMI
%token <Node>TYPE 
%token <Node>STRUCT 
%token <Node>ID
%token <Node>DOT
%token <Node>RETURN

%type <Node>Program ExtDecList ExtDef ExtDefList Specifier StructSpecifier OptTag Tag
%type <Node>VarDec FunDec VarList ParamDec Compst StmtList Stmt DefList Def DecList Dec Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left ADD SUB
%left MUL DIV
%right NOT NEG
%left DOT 
%left LB RB LC RC
%left LP RP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

Program: ExtDefList {
    root = generate_node("Program", @$.first_line, 0, 0, 0);
    $$ = root;
    add_first_child_node(root, $1);
    }

ExtDefList: ExtDef ExtDefList {
    $$ = generate_node("ExtDefList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |{$$ = NULL;}

ExtDef: Specifier ExtDecList SEMI{
    $$ = generate_node("ExtDef", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Specifier SEMI{
    $$ = generate_node("ExtDef", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |Specifier FunDec Compst{
    $$ = generate_node("ExtDef", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Specifier FunDec SEMI{
    $$ = generate_node("ExtDef", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |error SEMI{syntax_error = 1;}

ExtDecList: VarDec{
    $$ = generate_node("ExtDecList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |VarDec COMMA ExtDecList{
    $$ = generate_node("ExtDecList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }

Specifier: TYPE{
    $$ = generate_node("Specifier", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |StructSpecifier{
    $$ = generate_node("Specifier", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }

StructSpecifier: STRUCT OptTag LC DefList RC{
    $$ = generate_node("StructSpecifier", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    if($2 == NULL){
        if($4 == NULL){
            add_other_child_node($1, $3);
            add_other_child_node($3, $5);
        }
        else{
            add_other_child_node($1, $3);
            add_other_child_node($3, $4);
            add_other_child_node($4, $5);
        }
    }
    else if($4 == NULL){
        add_other_child_node($1, $2);
        add_other_child_node($2, $3);
        add_other_child_node($3, $5);
    }
    else{
        add_other_child_node($1, $2);
        add_other_child_node($2, $3);
        add_other_child_node($3, $4);
        add_other_child_node($4, $5);
    }
    }
    |STRUCT Tag{
    $$ = generate_node("StructSpecifier", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |error RC{syntax_error = 1;}

OptTag: ID{
    $$ = generate_node("OptTag", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |{$$ = NULL;}

Tag: ID{
    $$ = generate_node("Tag", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }

VarDec: ID{
    $$ = generate_node("VarDec", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |VarDec LB INT RB{
    $$ = generate_node("VarDec", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    add_other_child_node($3, $4);
    }
    |error RB{syntax_error = 1;}

FunDec: ID LP VarList RP{
    $$ = generate_node("FunDec", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    add_other_child_node($3, $4);
    }
    |ID LP RP{
    $$ = generate_node("FunDec", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |error RP{syntax_error = 1;}

VarList: ParamDec COMMA VarList{
    $$ = generate_node("VarList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |ParamDec{
    $$ = generate_node("VarList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }

ParamDec: Specifier VarDec{
    $$ = generate_node("ParamDec", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }

Compst: LC DefList StmtList RC{
    $$ = generate_node("Compst", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    if($2 == NULL){
        if($3 == NULL){
            add_other_child_node($1, $4);
        }
        else{
            add_other_child_node($1, $3);
            add_other_child_node($3, $4);
        }
    }
    else if($3 == NULL){
        add_other_child_node($1, $2);
        add_other_child_node($2, $4);
    }
    else{
        add_other_child_node($1, $2);
        add_other_child_node($2, $3);
        add_other_child_node($3, $4);
    }
    }
    |error RC{syntax_error = 1;}

StmtList: Stmt StmtList{
    $$ = generate_node("StmtList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |{$$ = NULL;}

Stmt: Exp SEMI{
    $$ = generate_node("Stmt", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |Compst{
    $$ = generate_node("Stmt", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |RETURN Exp SEMI{
    $$ = generate_node("Stmt", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{
    $$ = generate_node("Stmt", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    add_other_child_node($3, $4);
    add_other_child_node($4, $5);
    }
    |IF LP Exp RP Stmt ELSE Stmt{
    $$ = generate_node("Stmt", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    add_other_child_node($3, $4);
    add_other_child_node($4, $5);
    add_other_child_node($5, $6);
    add_other_child_node($6, $7);
    }
    |WHILE LP Exp RP Stmt{
    $$ = generate_node("Stmt", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    add_other_child_node($3, $4);
    add_other_child_node($4, $5);
    }
    |error SEMI{syntax_error = 1;}

DefList: Def DefList{
    $$ = generate_node("DefList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |{$$ = NULL;}

Def: Specifier DecList SEMI{
    $$ = generate_node("Def", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |error SEMI{syntax_error = 1;}

DecList: Dec{
    $$ = generate_node("DecList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |Dec COMMA DecList{
    $$ = generate_node("DecList", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }

Dec: VarDec{
    $$ = generate_node("Dec", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |VarDec ASSIGNOP Exp{
    $$ = generate_node("Dec", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }

Exp: Exp AND Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp OR Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp RELOP Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp ADD Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp SUB Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp MUL Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp DIV Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |LP Exp RP{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |SUB Exp %prec NEG{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |NOT Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    }
    |ID LP Args RP{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    add_other_child_node($3, $4);
    }
    |ID LP RP{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp LB Exp RB{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    add_other_child_node($3, $4);
    }
    |Exp DOT ID{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp ASSIGNOP Exp{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |ID{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |INT{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
    |FLOAT{
    $$ = generate_node("Exp", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }

Args: Exp COMMA Args{
    $$ = generate_node("Args", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    add_other_child_node($1, $2);
    add_other_child_node($2, $3);
    }
    |Exp{
    $$ = generate_node("Args", @$.first_line, 0, 0, 0);
    add_first_child_node($$, $1);
    }
%%

void add_first_child_node(struct Node *parent_node, struct Node *child_node){
    parent_node->first_child = child_node;
}

void add_other_child_node(struct Node *older_node, struct Node *younger_node){
    older_node->next_brother = younger_node;
}

void yyerror(char* error_msg){
    printf("Error type B at Line %d: %s\n", yylineno, error_msg);
}

void yymissingerror(char* error){
    printf("Error type B at Line %d: Missing \"%s\"\n", yylineno, error);
}

void print_tree(struct Node *root, int height){
    if(root){
        if(root->leaf){
            for(int i = 0; i < height; i++){
                printf(" ");
            }
            if(root->leaf == 1){
                printf("%s: %d\n", root->name, root->val.int_val);
            }
            else if(root->leaf == 2){
                printf("%s: %f\n", root->name, root->val.float_val);
            }
            else if(root->leaf == 3){
                printf("%s: %s\n", root->name, root->val.char_val);
            }
        }
        else{
            for(int i = 0; i < height; i++){
                printf(" ");
            }
            printf("%s (%d)\n", root->name, root->column);
        }
        print_tree(root->first_child, height + 1);
        print_tree(root->next_brother, height);
    }
}
