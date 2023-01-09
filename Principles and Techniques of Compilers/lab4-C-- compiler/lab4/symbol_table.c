#include "symbol_table.h"
const int hash_num = 10007;

int hash_func(char *c, int hash_n){
    unsigned int val = 0;
    for(; *c; ++c){
        val = ((val << 7) + *c) % hash_n;
    }
    return val;
}

void clear_hash(hash_map *h){
    for(int i = 0; i < 10007; i++){
        h->hash_list[i] = nullptr;
    }
}

void insert_hash(hash_map *h, hash_node *hn){
    int index = hash_func(hn->key, hash_num);
    if(h->hash_list[index] != nullptr){
        hn->next = h->hash_list[index];
        h->hash_list[index] = hn;
    }
    else{
        h->hash_list[index] = hn;
        hn->next = nullptr;
    }
}

hash_node* search_hash(hash_map *h, char key[32]){
    int index = hash_func(key, hash_num);
    if(h->hash_list[index] != nullptr){
        hash_node *cur = h->hash_list[index];
        while(cur != nullptr){
            if(strcmp(key, cur->key) == 0){
                return cur;
            }
            else{
                cur = cur->next;
            }
        }
    }
    return nullptr;
}

void delete_hash(hash_map *h, char key[32]){
    int index = hash_func(key, hash_num);
    if(h->hash_list[index] != nullptr){
        hash_node *cur = h->hash_list[index];
        hash_node *last = nullptr;
        while(cur != nullptr){
            if(cur->key && key && strcmp(key, cur->key) == 0){
                if(last){
                    last->next = cur->next;
                }
                else{
                    h->hash_list[index] = cur->next;
                }
                break;
            }
            else{
                last = cur;
                cur = cur->next;
            }
        }
    }
}

void clear_stack(stack *s){
    for(int i = 0; i < 1000; i++){
        s->stack_list[i] = nullptr;
    }
    s->top = 0;
}

void insert_stack(stack *s, hash_node *hn){
    if(s->stack_list[s->top] != nullptr){
        if(search_cur_stack(s, hn->key)){
            //printf("wrong in insert_stack: already decleared.");
        }
        else{
            hn->stack_next = s->stack_list[s->top];
            s->stack_list[s->top] = hn;
        }
    }
    else{
        s->stack_list[s->top] = hn;
        hn->stack_next = nullptr;
    }
}

void new_stack(stack *s){
    if(s->top < 999){
        s->top = s->top + 1;
    }
    else{
        printf("max_stack!!!");
    }
}

hash_node* search_cur_stack(stack *s, char key[32]){
    if(s->stack_list[s->top] != nullptr){
        hash_node *cur = s->stack_list[s->top];
        while(cur != nullptr){
            if(strcmp(cur->key, key) == 0){
                return cur;
            }
            else{
                cur = cur->stack_next;
            }
        }
    }
    return nullptr;
}

hash_node* search_all_stack(stack *s, char key[32]){
    int top = s->top;
    while(top >= 0){
        if(s->stack_list[top] != nullptr){
            hash_node *cur = s->stack_list[top];
            while(cur != nullptr){
                if(strcmp(cur->key, key) == 0){
                    return cur;
                }
                else{
                    cur = cur->stack_next;
                }
            }
        }
        --top;
    }
    return nullptr;
}

void delete_cur_stack(stack *s, hash_map *h){
    if(s->top > 0){
        //if(s->stack_list[s->top] != nullptr){
            //hash_node *cur = s->stack_list[s->top];
            //while(cur != nullptr){
            //    delete_hash(h, cur->key);
            //    hash_node *tmp = cur;
            //    cur = cur->stack_next;
                //free(tmp);
            //}
        //}
        s->stack_list[s->top] = nullptr;
        s->top = s->top - 1;
    }
    else{
        printf("only 1 level of stack left!!!\n");
    }
}

hash_node* get_cur_func(stack *s){
    int t = s->top;
    while(t >= 0){
        hash_node* cur = s->stack_list[t];
        while(cur != nullptr && cur->type->kind != FUNC){
            cur = cur->stack_next;
        }
        if(cur){
            return cur;
        }
        --t;
    }
    
}

void add_func_arg_node(type *ft, args_node *a){
    args_node *cur = ft->u.func->args_node;
    if(cur == nullptr){
        ft->u.func->args_node = a;
    }
    else{
        a->next_arg = cur;
        ft->u.func->args_node = a;
    }
}

void clear_fdm(func_decl_map *fdm){
    fdm->num = 0;
    for(int i = 0; i < 1000; i++){
        fdm->decl_map[i] = nullptr;
    }
}

void add_declearation(func_decl_map *fdm, char *name, func_desc *func_desc, int column){
    for(int i = 0; i < fdm->num; i++){
        if(strcmp(fdm->decl_map[i]->name, name) == 0){
            return;
        }
    }
    func_decl *fd = (func_decl *)malloc(sizeof(func_decl));
    strcpy(fd->name, name);
    fd->defined = 0;
    fd->func_desc = func_desc;
    fd->column = column;
    fdm->decl_map[fdm->num] = fd;
    fdm->num++;
}

void add_definition(func_decl_map *fdm, char *name, func_desc *func_desc){
    func_decl *fd = (func_decl *)malloc(sizeof(func_decl));
    strcpy(fd->name, name);
    fd->defined = 1;
    fd->func_desc = func_desc;
    fdm->decl_map[fdm->num] = fd;
    fdm->num++;
}

type* find_decl(func_decl_map *fdm, char *name){
    for(int i = 0; i < fdm->num; i++){
        if(strcmp(fdm->decl_map[i]->name, name) == 0 && fdm->decl_map[i]->defined == 0){
            type *t = (type *)malloc(sizeof(type));
            t->kind = FUNC;
            t->u.func = fdm->decl_map[i]->func_desc;
            return t;
        }
    }
    return nullptr;
}

void func_defined(func_decl_map *fdm, char *name){
    for(int i = 0; i < fdm->num; i++){
        if(strcmp(fdm->decl_map[i]->name, name) == 0){
            fdm->decl_map[i]->defined = 1;
        }
    }
}

func_decl* check_declearation(func_decl_map *fdm, char *name){
    for(int i = 0; i < fdm->num; i++){
        if(strcmp(fdm->decl_map[i]->name, name) == 0){
            return fdm->decl_map[i];
        }
    }
    return nullptr;
}

void check_definition(func_decl_map *fdm){
    for(int i = 0; i < fdm->num; i++){
        if(fdm->decl_map[i]->defined == 0){
            put_error_18(fdm->decl_map[i]->column, fdm->decl_map[i]->name);
        }
    }
}

void put_error_18(int column, char *id){
    printf("Error type 18 at Line %d: Undefined function \"%s\".\n", column, id);
}