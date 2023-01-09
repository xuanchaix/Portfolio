#include "symbol_table.h"
#include "lex.yy.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct intr_code intr_code;
typedef struct intr_code_link intr_code_link;
typedef struct operand operand;
typedef struct arg_list arg_list;
typedef struct public_sub_expr public_sub_expr;

intr_code_link *head_intr_code_link;
intr_code_link *tail_intr_code_link;
int temp_count;
int label_count;
int variable_count;

struct operand{
    enum{VARIABLE, CONSTANT, ADDRESS, TEMP} kind;
    union{
        int var_no;
        int value;
    }u;
    char name[32];
};

struct intr_code{
    enum{ASSIGN_i, ADD_i, SUB_i, MUL_i, DIV_i, LABEL_i, FUNCTION_i, ADDR_i, F_STAR_i, B_STAR_i, GOTO_i, RELOP_i, RETURN_i
    , DEC_i, ARG_i, CALL_i, PARAM_i, READ_i, WRITE_i} kind;
    union{
        struct {operand right, left;} assign;
        struct {operand result, op1, op2;} three;
        struct {operand op;} one;
        struct {operand op1, op2;} two;
        struct {operand op1, op2; char *label_true, *relop; int is_if;} relop;
        struct {char* label_false;} goto_;
        struct {char* label; int is_while;} label;
    }u;
};

struct intr_code_link{
    intr_code *code;
    intr_code_link *prev, *next;
};

struct arg_list{
    int kind;
    int val;
    char name[32];
    arg_list *next;
};

struct public_sub_expr{
    intr_code_link *icl;
    public_sub_expr *next;
};

void init_intr_code_gene(intr_code_link *head, hash_map *hm, stack *s, func_decl_map* fdm);

void print_intr_code(intr_code_link *head);

char* temp_char_array(int number);
char* label_char_array(int number);
char* variable_char_array(int number);

void optimize_intr_code(intr_code_link *head);
int opcmp(intr_code *ic1, intr_code *ic2);

intr_code_link* link_icl(intr_code_link *icl1, intr_code_link *icl2);

intr_code_link* translate_exp(Node *exp, hash_map *hm, char* place);

intr_code_link* translate_stmt(Node *stmt, hash_map *hm);

intr_code_link* translate_cond(Node *exp, char* label1, char* label2, hash_map *hm);

intr_code_link* translate_compst(Node *compst, hash_map *hm);

intr_code_link* translate_stmtlist(Node* stmtlist, hash_map *hm);

intr_code_link* translate_deflist(Node* deflist, hash_map *hm);

intr_code_link* translate_def(Node *def, hash_map *hm);

intr_code_link* translate_declist(Node *declist, hash_map *hm);

intr_code_link* translate_dec(Node *dec, hash_map *hm);

intr_code_link* translate_args(Node *args, hash_map *hm, arg_list *arglist, arg_list *head);