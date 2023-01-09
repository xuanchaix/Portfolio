#ifndef SDT_H
#define SDT_H
#include "code_gene.h"

hash_map *h;
hash_map *intr_hm;
stack *s;
func_decl_map *fdm;

//same type?
int typecmp(type *t1, type *t2);

//tree

void sdt_main(Node* root);
void sdt_Program(Node* root);
void sdt_ExtDefList(Node* this_root);
void sdt_ExtDef(Node* this_root);
hash_node* sdt_ExtDecList(Node* this_root);
type* sdt_Specifier(Node* this_root);
type* sdt_StructSpecifier(Node* this_root);
char* sdt_OptTag(Node* this_root);
char* sdt_Tag(Node* this_root);
hash_node* sdt_VarDec(Node* this_root);
hash_node* sdt_FunDec(Node* this_root, int is_def);
type* sdt_VarList(Node* this_root);
args_node* sdt_ParamDec(Node* this_root);
void sdt_Compst(Node* this_root);
void sdt_StmtList(Node* this_root);
void sdt_Stmt(Node* this_root);
field_list* sdt_DefList(Node* this_root, int is_struct);
void sdt_StmtList(Node* this_root);
field_list* sdt_Def(Node* this_root, int is_struct);
hash_node* sdt_DecList(Node* this_root, int is_struct);
hash_node* sdt_Dec(Node* this_root, int is_struct);
type* sdt_Exp(Node* this_root);
args_node* sdt_Args(Node* this_root);


//error output

//valuable not decleared
void put_error_1(int column, char* id);
//function not decleared
void put_error_2(int column, char* id);
//valuable redecleared
void put_error_3(int column, char* id);
//function redecleared
void put_error_4(int column, char* id);
//mismatching type operation
void put_error_5(int column);
//right value in the left of '='
void put_error_6(int column);
//mismatched operand
void put_error_7(int column);
//wrong return type
void put_error_8(int column);
//unmatched parameters
void put_error_9(int column);
//using [] to unarray value
void put_error_10(int column);
//using [] to no-function value
void put_error_11(int column, char* id);
//unexpected number in []
void put_error_12(int column);
//using . to no_struct value
void put_error_13(int column);
//call undefined value in struct
void put_error_14(int column, char* id);
//redefined same struct in struct or initialize when defining a struct
void put_error_15(int column);
//struct redecleared
void put_error_16(int column, char* id);
//struct not decleared
void put_error_17(int column, char* id);
//Inconsistent declaration of function
void put_error_19(int column, char* id);

#endif