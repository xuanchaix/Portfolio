// hash_map
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define nullptr 0
typedef struct symbol type;
typedef struct field_list field_list;
typedef struct hash_node hash_node;
typedef struct hash_map hash_map;
typedef struct stack stack;
typedef struct func_desc func_desc;
typedef struct args_node args_node;
typedef struct func_decl_map func_decl_map;
typedef struct func_decl func_decl;

struct hash_map{
    hash_node* hash_list[10007];
};

struct stack{
    int top;
    hash_node* stack_list[1000];
};

//node
struct hash_node{
    hash_node *next;
    hash_node *stack_next;
    hash_node *build_next;
    char key[32];
    type *type;
};

//type
struct symbol{
    enum {BASIC, ARRAY, STRUCTURE, FUNC} kind;
    union{
        int basic;
        struct{
            type *elem;
            int size;
        }array;
        field_list *structure;
        func_desc *func;
    }u;
    int is_rval;
};

//struct description
struct field_list{
    char name[32];
    type *type;
    field_list *tail;
};

//function description
struct func_desc{
    type *return_type;
    args_node *args_node;
};

struct args_node{
    type *arg_type;
    char name[32];
    args_node *next_arg;
};


struct func_decl_map{
    int num;
    func_decl* decl_map[1000];
};

struct func_decl{
    char name[32];
    func_desc *func_desc;
    int defined;
    int column;
};


int hash_func(char *c, int hash_n);
void clear_hash(hash_map *h);
void insert_hash(hash_map *h, hash_node *hn);
hash_node* search_hash(hash_map *h, char *key);
void delete_hash(hash_map *h, char *key);

void clear_stack(stack *s);
void insert_stack(stack *s, hash_node *hn);
void new_stack(stack *s);
hash_node* search_cur_stack(stack *s, char *key);
hash_node* search_all_stack(stack *s, char *key);
void delete_cur_stack(stack *s, hash_map *h);
hash_node* get_cur_func(stack *s);

void add_func_arg_node(type *ft, args_node *a);

void clear_fdm(func_decl_map *fdm);
void add_declearation(func_decl_map *fdm, char *name, func_desc *func_desc, int column);
void add_definition(func_decl_map *fdm, char *name, func_desc *func_desc);
type* find_decl(func_decl_map *fdm, char *name);
void func_defined(func_decl_map *fdm, char *name);
func_decl* check_declearation(func_decl_map *fdm, char *name);
void check_definition(func_decl_map *fdm);

void put_error_18(int column, char *id);

#endif