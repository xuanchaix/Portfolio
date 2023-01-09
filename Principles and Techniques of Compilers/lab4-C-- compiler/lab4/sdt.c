#include "sdt.h"
/*
Program: ExtDefList

ExtDefList: ExtDef ExtDefList 
    |{$$ = NULL;}

ExtDef: Specifier ExtDecList SEMI
    |Specifier SEMI
    |Specifier FunDec Compst
    |error SEMI

ExtDecList: VarDec
    |VarDec COMMA ExtDecList

Specifier: TYPE
    |StructSpecifier

StructSpecifier: STRUCT OptTag LC DefList RC
    |STRUCT Tag
    |error RC{syntax_error = 1;}

OptTag: ID
    |{$$ = NULL;}

Tag: ID

VarDec: ID
    |VarDec LB INT RB
    |error RB{syntax_error = 1;}

FunDec: ID LP VarList RP
    |ID LP RP
    |error RP{syntax_error = 1;}

VarList: ParamDec COMMA VarList
    |ParamDec

ParamDec: Specifier VarDec

Compst: LC DefList StmtList RC
    |error RC{syntax_error = 1;}

StmtList: Stmt StmtList
    |{$$ = NULL;}

Stmt: Exp SEMI
    |Compst
    |RETURN Exp SEMI
    |IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
    |IF LP Exp RP Stmt ELSE Stmt
    |WHILE LP Exp RP Stmt
    |error SEMI{syntax_error = 1;}

DefList: Def DefList
    |{$$ = NULL;}

Def: Specifier DecList SEMI
    |error SEMI{syntax_error = 1;}

DecList: Dec
    |Dec COMMA DecList

Dec: VarDec
    |VarDec ASSIGNOP Exp

Exp: Exp AND Exp
    |Exp OR Exp
    |Exp RELOP Exp
    |Exp ADD Exp
    |Exp SUB Exp
    |Exp MUL Exp
    |Exp DIV Exp
    |LP Exp RP
    |SUB Exp %prec NEG
    |NOT Exp
    |ID LP Args RP
    |ID LP RP
    |Exp LB Exp RB
    |Exp DOT ID
    |Exp ASSIGNOP Exp
    |ID
    |INT
    |FLOAT

Args: Exp COMMA Args
    |Exp
*/

void sdt_main(Node* root){
    //main entry
    s = malloc(sizeof(stack));
    h = malloc(sizeof(hash_map));
    intr_hm = malloc(sizeof(hash_map));
    fdm = malloc(sizeof(func_decl_map));
    head_intr_code_link = malloc(sizeof(intr_code_link));
    tail_intr_code_link = head_intr_code_link;

    //init
    clear_hash(h);
    clear_stack(s);
    clear_fdm(fdm);

    //do parser
    init_intr_code_gene(head_intr_code_link, h, s, fdm);
    sdt_Program(root);
    check_definition(fdm);

    //print and generate
    //if too many mistakes, comment the optimization below
    optimize_intr_code(head_intr_code_link);
    optimize_intr_code(head_intr_code_link);
    optimize_intr_code(head_intr_code_link);
    print_intr_code(head_intr_code_link);

    //code generation
    code_generate(head_intr_code_link);
}

void sdt_Program(Node* root){
    //Program: ExtDefList
    sdt_ExtDefList(get_n_child(root, 0));
}

void sdt_ExtDefList(Node* this_root){
    //ExtDefList: ExtDef ExtDefList 
    //|
    if(this_root){
        sdt_ExtDef(get_n_child(this_root, 0));
        sdt_ExtDefList(get_n_child(this_root, 1));
    }
}

void sdt_ExtDef(Node* this_root){
    //ExtDef: Specifier ExtDecList SEMI
    //|Specifier SEMI
    //|Specifier FunDec Compst
    //|Specifier FunDec SEMI
    Node *n0 = get_n_child(this_root, 0);
    Node *n1 = get_n_child(this_root, 1);
    Node *n2 = get_n_child(this_root, 2);
    if(n0 && strcmp(n0->name, "Specifier") == 0){
        type *t = sdt_Specifier(n0);
        //global value definition
        if(strcmp(n1->name, "ExtDecList") == 0){
            hash_node *hn = sdt_ExtDecList(n1);
            hash_node *cur = hn;
            while(cur != nullptr){
                //finish the type
                if(cur->type->kind == BASIC){
                    cur->type->u.basic = t->u.basic;
                    //free(t);
                }
                else if(cur->type->kind == ARRAY){
                    cur->type->u.array.elem = t;
                }
                else{
                    cur->type->u.structure = t->u.structure;
                }
                //add to stack and hash
                if(search_cur_stack(s, cur->key)){
                    if(cur->type->kind == STRUCTURE){
                        put_error_16(n1->column, cur->key);
                    }
                    else{
                        put_error_3(n1->column, cur->key);
                    }
                }
                else{
                    insert_stack(s, cur);
                    insert_hash(h, cur);
                }
                cur = cur->build_next;
            }
        }
        else if(strcmp(n1->name, "SEMI") == 0){
            //nop
        }
        else if(strcmp(n1->name, "FunDec") == 0){
            //function definition
            if(strcmp(n2->name, "Compst") == 0){
                new_stack(s);
                hash_node *hn = sdt_FunDec(n1, 1);
                if(hn){
                    hn->type->u.func->return_type = t;
                    type *d_ft = find_decl(fdm, hn->key);
                    if(d_ft && typecmp(hn->type, d_ft)){
                        func_defined(fdm, hn->key);
                    }
                    else if(!d_ft){
                        add_definition(fdm, hn->key, hn->type->u.func);
                    }
                    else{
                        put_error_19(n0->column, hn->key);
                    }
                }
                insert_stack(s, hn);
                insert_hash(h, hn);
                sdt_Compst(n2);
                intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
                icl2->next = nullptr;
                icl2->code = ic1;
                ic1->kind = FUNCTION_i;
                ic1->u.label.label = hn->key;
                args_node *cur = hn->type->u.func->args_node;
                while(cur != nullptr){
                    intr_code_link *iclt = (intr_code_link *)malloc(sizeof(intr_code_link));
                    intr_code *ict = (intr_code *)malloc(sizeof(intr_code));
                    iclt->next = nullptr;
                    iclt->code = ict;
                    ict->kind = PARAM_i;
                    operand opt;
                    strcpy(opt.name, cur->name);
                    opt.kind = VARIABLE;
                    ict->u.one.op = opt;
                    link_icl(icl2, iclt);
                    cur = cur->next_arg;
                }

                intr_code_link *icl = translate_compst(n2, h);
                link_icl(icl2, icl);
                tail_intr_code_link = link_icl(tail_intr_code_link, icl2);
            
                delete_cur_stack(s, h);
                if(!search_cur_stack(s, hn->key)){
                    insert_stack(s, hn);
                    insert_hash(h, hn);
                }
                else{
                    put_error_4(n1->column, hn->key);
                }
            }
            //function declearation
            else if(strcmp(n2->name, "SEMI") == 0){
                hash_node *hn = sdt_FunDec(n1, 0);
                if(hn){
                    hn->type->u.func->return_type = t;
                    add_declearation(fdm, hn->key, hn->type->u.func, n0->column);
                }
            }
        }
        else{
            printf("wrong expression in sdt_ExtDef");
        }
    }
    else{
        printf("wrong expression in sdt_ExtDef");
    }
    
}

hash_node* sdt_ExtDecList(Node* this_root){
    //ExtDecList: VarDec
    //|VarDec COMMA ExtDecList
    Node *n0 = get_n_child(this_root, 0);
    Node *n2 = get_n_child(this_root, 2);
    hash_node *hn = sdt_VarDec(n0);
    if(n2){
        if(strcmp(n2->name, "ExtDecList") == 0){
            hash_node *new_hn = sdt_ExtDecList(n2);
            if(new_hn){
                hn->build_next = new_hn;
            }
        }
        else{
            printf("wrong expression in sdt_ExtDecList");
        }
    }
    return hn;
}

type* sdt_Specifier(Node* this_root){
    //Specifier: TYPE
    //|StructSpecifier
    Node *n0 = get_n_child(this_root, 0);
    if(strcmp(n0->name, "TYPE") == 0){
        type *t = (type *)malloc(sizeof(type));
        if(strcmp(n0->val.char_val, "int") == 0){
            t->kind = BASIC;
            t->u.basic = 1;
        }
        else if(strcmp(n0->val.char_val, "float") == 0){
            t->kind = BASIC;
            t->u.basic = 2;
        }
        else{
            printf("wrong type %s in line %d", n0->val.char_val, n0->column);
        }
        return t;
    }
    else if(strcmp(n0->name, "StructSpecifier") == 0){
        return sdt_StructSpecifier(n0);
    }
    else{
        printf("wrong expression in sdt_Specifier");
    }
}

type* sdt_StructSpecifier(Node* this_root){
    //StructSpecifier: STRUCT OptTag LC DefList RC
    //|STRUCT Tag
    Node *n0 = get_n_child(this_root, 0);
    Node *n1 = get_n_child(this_root, 1);
    if(strcmp(n0->name, "STRUCT") == 0){
        if(strcmp(n1->name, "OptTag") == 0){
            Node *n3 = get_n_child(this_root, 3);
            char *name = sdt_OptTag(n1);
            hash_node *hn = (hash_node *)malloc(sizeof(hash_node));
            type *t = (type *)malloc(sizeof(type));
            strcpy(hn->key, name);
            hn->next = nullptr;
            hn->stack_next = nullptr;
            t->kind = STRUCTURE;
            hn->type = t;
            field_list *fl = sdt_DefList(n3, 1);
            field_list *check_fl = fl;
            while(check_fl != nullptr){
                if(strcmp(check_fl->name, name) == 0){
                    put_error_15(n0->column);
                }
                check_fl = check_fl->tail;
            }
            t->u.structure = fl;
            if(search_cur_stack(s, hn->key)){
                put_error_16(n0->column, hn->key);
            }
            else{
                insert_hash(h, hn);
                insert_stack(s, hn);
            }
            return nullptr;
        }
        else if(strcmp(n1->name, "Tag") == 0){
            char *name = sdt_Tag(n1);
            hash_node *hn = search_hash(h, name);
            if(hn){
                return hn->type;
            }
            else{
                put_error_17(n0->column, name);
                type *t = (type *)malloc(sizeof(type));
                t->kind = STRUCTURE;
                t->u.structure = (field_list *)malloc(sizeof(field_list));
                t->u.structure = nullptr;
                return t;
            }
        }
        else{
            printf("wrong expression in sdt_StructSpecifier");
            return nullptr;
        }
    }
    else{
        printf("wrong expression in sdt_StructSpecifier");
        return nullptr;
    }
    
}

char* sdt_OptTag(Node* this_root){
    //OptTag: ID
    //|
    Node *n0 = get_n_child(this_root, 0);
    if(n0){
        return n0->val.char_val;
    }
}

char* sdt_Tag(Node* this_root){
    //Tag: ID
    Node *n0 = get_n_child(this_root, 0);
    if(n0){
        return n0->val.char_val;
    }
}

hash_node* sdt_VarDec(Node* this_root){
    //VarDec: ID
    //|VarDec LB INT RB
    Node *n0 = get_n_child(this_root, 0);
    if(strcmp(n0->name, "ID") == 0){
        hash_node *hn = (hash_node *)malloc(sizeof(hash_node));
        type *t = (type *)malloc(sizeof(type));
        hn->next = nullptr;
        hn->stack_next = nullptr;
        hn->build_next = nullptr;
        strcpy(hn->key, n0->val.char_val);
        t->kind = BASIC;
        hn->type = t;
        return hn;
    }
    else if(strcmp(n0->name, "VarDec") == 0){
        Node *n2 = get_n_child(this_root, 2);
        hash_node *hn = sdt_VarDec(n0);
        type *t = hn->type;
        if(t->kind == BASIC || t->kind == STRUCTURE){
            type *t1 = (type *)malloc(sizeof(type));
            t1->kind = BASIC;
            type *t2 = (type *)malloc(sizeof(type));
            t2->kind = ARRAY;
            t2->u.array.elem = t1;
            hn->type = t2;
            if(n2->leaf == 1){
                if(n2->val.int_val >= 1){
                    t->u.array.size = n2->val.int_val;
                }
                else{
                    put_error_12(n0->column);
                }
            }
            else{
                put_error_12(n0->column);
            }
            
        }
        else if(t->kind == ARRAY){
            type *tt = t;
            while(tt->kind == ARRAY){
                tt->u.array.size = tt->u.array.size * n2->val.int_val;
                tt = tt->u.array.elem;
            }
            tt->kind = ARRAY;
            type *t1 = (type *)malloc(sizeof(type));
            t1->kind = BASIC;
            tt->u.array.elem = t1;
            tt->u.array.size = n2->val.int_val;
        }
        else{
            //error
        }
        return hn;
    }
    else{
        printf("wrong expression in sdt_VarDec");
        return nullptr;
    }
}

hash_node* sdt_FunDec(Node* this_root, int is_def){
    //FunDec: ID LP VarList RP
    //|ID LP RP
    Node *n0 = get_n_child(this_root, 0);
    Node *n2 = get_n_child(this_root, 2);
    hash_node *hn = (hash_node *)malloc(sizeof(hash_node));
    if(is_def){//function definition
        if(strcmp(n0->name, "ID") == 0){
            hn->build_next = nullptr;
            hn->next = nullptr;
            hn->stack_next = nullptr;
            strcpy(hn->key, n0->val.char_val);
            if(strcmp(n2->name, "VarList") == 0){
                type *ft = sdt_VarList(n2);
                hn->type = ft;
                return hn;
            }
            else if(strcmp(n2->name, "RP") == 0){
                type *ft = (type *)malloc(sizeof(type));
                func_desc *fd = (func_desc *)malloc(sizeof(func_desc));
                fd->args_node = nullptr;
                fd->return_type = nullptr;
                ft->kind = FUNC;
                ft->u.func = fd;
                hn->type = ft;
                return hn;
            }
            else{
                printf("wrong expression in sdt_FunDec");
            }
        }
        else{
            printf("wrong expression in sdt_FunDec");
        }
    }
    else{//function declearation
        func_desc *fd;
        if(strcmp(n2->name, "VarList") == 0){
            type *ft = sdt_VarList(n2);
            fd = ft->u.func;
            type *d_ft = find_decl(fdm, n0->val.char_val);
            if(d_ft && typecmp(ft, d_ft)){
            }
            else if(!d_ft){
                strcpy(hn->key, n0->val.char_val);
                hn->type = ft;
                return hn;
            }
            else{
                put_error_19(n0->column, n0->val.char_val);
            }
        }
        else if(strcmp(n2->name, "RP") == 0){
            type *ft = (type *)malloc(sizeof(type));
            fd = (func_desc *)malloc(sizeof(func_desc));
            fd->args_node = nullptr;
            fd->return_type = nullptr;
            ft->kind = FUNC;
            ft->u.func = fd;
            type *d_ft = find_decl(fdm, n0->val.char_val);
            if(d_ft && typecmp(ft, d_ft)){
            }
            else if(!d_ft){
                strcpy(hn->key, n0->val.char_val);
                hn->type = ft;
                return hn;
            }
            else{
                put_error_19(n0->column, n0->val.char_val);
            }
        }
    }
    return nullptr;
}

type* sdt_VarList(Node* this_root){
    //VarList: ParamDec COMMA VarList
    //|ParamDec
    Node *n0 = get_n_child(this_root, 0);
    Node *n2 = get_n_child(this_root, 2);
    args_node *a0 = sdt_ParamDec(n0);
    type *ft;
    if(n2 && strcmp(n2->name, "VarList") == 0){
        ft = sdt_VarList(n2);
        add_func_arg_node(ft, a0);
    }
    else{
        ft = (type *)malloc(sizeof(type));
        ft->kind = FUNC;
        func_desc *fd = (func_desc *)malloc(sizeof(func_desc));
        fd->return_type = nullptr;
        ft->u.func = fd;
        fd->args_node = a0;
    }
    return ft;
}

args_node* sdt_ParamDec(Node* this_root){ // parameter of the function(in the function scope)
    //ParamDec: Specifier VarDec
    Node *n0 = get_n_child(this_root, 0);
    Node *n1 = get_n_child(this_root, 1);
    type *t = sdt_Specifier(n0);
    hash_node *hn = sdt_VarDec(n1);
    args_node *a0 = (args_node *)malloc(sizeof(args_node));
    a0->arg_type = t;
    a0->next_arg = nullptr;
    strcpy(a0->name, hn->key);

    //finish the type
    if(t->kind == STRUCTURE){
        hn->type = t;
    }
    else if(hn->type->kind == BASIC){
        hn->type->u.basic = t->u.basic;
        //free(t);
    }
    else if(hn->type->kind == ARRAY){
        hn->type->u.array.elem = t;
    }
    //add to stack and hash
    if(search_cur_stack(s, hn->key)){
        put_error_3(n1->column, hn->key);
    }
    else{
        insert_stack(s, hn);
        insert_hash(h, hn);
    }
    return a0;
}

void sdt_Compst(Node* this_root){
    //Compst: LC DefList StmtList RC
    Node *n1 = get_n_child(this_root, 1);
    Node *n2 = get_n_child(this_root, 2);
    if(strcmp(n1->name, "DefList") == 0){
        sdt_DefList(n1, 0);
        if(strcmp(n2->name, "StmtList") == 0){
            sdt_StmtList(n2);
        }
    }
    else if(strcmp(n1->name, "StmtList") == 0){
        sdt_StmtList(n1);
    }
}

void sdt_StmtList(Node* this_root){
    //StmtList: Stmt StmtList
    //|
    Node *n0 = get_n_child(this_root, 0);
    if(n0){
        Node *n1 = get_n_child(this_root, 1);
        sdt_Stmt(n0);
        sdt_StmtList(n1);
    }
}

void sdt_Stmt(Node* this_root){
    //Stmt: Exp SEMI
    //|Compst
    //|RETURN Exp SEMI
    //|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
    //|IF LP Exp RP Stmt ELSE Stmt
    //|WHILE LP Exp RP Stmt
    Node *n0 = get_n_child(this_root, 0);
    if(strcmp(n0->name, "Exp") == 0){
        sdt_Exp(n0);
    }
    else if(strcmp(n0->name, "Compst") == 0){
        sdt_Compst(n0);
    }
    else if(strcmp(n0->name, "RETURN") == 0){
        Node *n1 = get_n_child(this_root, 1);
        type *t1 = sdt_Exp(n1);
        type *t0 = get_cur_func(s)->type->u.func->return_type;
        if(!typecmp(t0, t1)){
            put_error_8(n1->column);
        }
    }
    else if(strcmp(n0->name, "IF") == 0){
        Node *n2 = get_n_child(this_root, 2);
        Node *n4 = get_n_child(this_root, 4);
        sdt_Exp(n2);
        new_stack(s);
        sdt_Stmt(n4);
        delete_cur_stack(s, h);
        Node *n6 = get_n_child(this_root, 6);
        if(n6){
            new_stack(s);
            sdt_Stmt(n6);
            delete_cur_stack(s, h);
        }
    }
    else if(strcmp(n0->name, "WHILE") == 0){
        Node *n2 = get_n_child(this_root, 2);
        Node *n4 = get_n_child(this_root, 4);
        sdt_Exp(n2);
        new_stack(s);
        sdt_Stmt(n4);
        delete_cur_stack(s, h);
    }
    else{
        printf("wrong expression in sdt_Stmt");
    }
}

field_list* sdt_DefList(Node* this_root, int is_struct){
    //DefList: Def DefList
    //|
    Node *n0 = get_n_child(this_root, 0);
    if(n0){
        Node *n1 = get_n_child(this_root, 1);
        if(!is_struct){
            sdt_Def(n0, is_struct);
            sdt_DefList(n1, is_struct);
        }
        else{//struct
            field_list *fl = sdt_Def(n0, is_struct);
            field_list *new_fl = sdt_DefList(n1, is_struct);
            field_list *check_fl1 = fl;
            field_list *check_fl2 = new_fl;

            while(check_fl1 != nullptr){
                while(check_fl2 != nullptr){
                    if(strcmp(check_fl1->name, check_fl2->name) == 0){
                        put_error_15(n1->column);
                    }
                    check_fl2 = check_fl2->tail;
                }
                check_fl1 = check_fl1->tail;
            }
            if(new_fl){
                field_list *cur = fl;
                while(cur != nullptr && cur->tail != nullptr){
                    cur = cur->tail;
                }
                if(cur == nullptr){
                    fl = new_fl;
                }
                else{
                    cur->tail = new_fl;
                }
                return fl;
            }
            else{
                return fl;
            }
        }
    }
    else{
        return nullptr;
    }
}

field_list* sdt_Def(Node* this_root, int is_struct){
    //Def: Specifier DecList SEMI
    Node *n0 = get_n_child(this_root, 0);
    Node *n1 = get_n_child(this_root, 1);
    type *t = sdt_Specifier(n0);
    hash_node *hn = sdt_DecList(n1, is_struct);
    hash_node *cur = hn;
    if(!is_struct){
        while(cur != nullptr){
            //finish the type
            if(t->kind == STRUCTURE){
                if(!typecmp(cur->type, t) && cur->type->is_rval){
                    put_error_5(n0->column);
                }
                cur->type = t;
            }
            else if(cur->type->kind == BASIC){
                if(!typecmp(cur->type, t) && cur->type->is_rval){
                    put_error_5(n0->column);
                }
                else{
                    cur->type->u.basic = t->u.basic;
                }
                //free(t);
            }
            else if(cur->type->kind == ARRAY){
                if(!typecmp(cur->type, t) && cur->type->is_rval){
                    put_error_5(n0->column);
                }
                type *tt = cur->type->u.array.elem;
                while(tt->kind == ARRAY){
                    tt = tt->u.array.elem;
                }
                tt->u.basic = t->u.basic;
            }
            //add to stack and hash
            if(search_cur_stack(s, cur->key)){
                put_error_3(n1->column, cur->key);
            }
            else{
                insert_stack(s, cur);
                insert_hash(h, cur);
            }
            cur = cur->build_next;
        }
        return nullptr;
    }
    else{//struct
        field_list *cur_fl;
        field_list *head_fl = (field_list *)malloc(sizeof(field_list));
        strcpy(head_fl->name, cur->key);
        head_fl->tail = nullptr;
        head_fl->type = cur->type;
        cur_fl = head_fl;
        cur = cur->build_next;
        while(cur != nullptr){
            field_list *fl = (field_list *)malloc(sizeof(field_list));
            strcpy(fl->name, cur->key);
            fl->tail = nullptr;
            fl->type = cur->type;
            cur_fl->tail = fl;
            cur_fl = fl;
            cur = cur->build_next;
        }
        return head_fl;
    }
}

hash_node* sdt_DecList(Node* this_root, int is_struct){
    //DecList: Dec
    //|Dec COMMA DecList
    Node *n0 = get_n_child(this_root, 0);
    Node *n2 = get_n_child(this_root, 2);
    hash_node *hn = sdt_Dec(n0, is_struct);
    if(n2){
        hash_node *new_hn = sdt_DecList(n2, is_struct);
        if(new_hn){
            hn->build_next = new_hn; //use the struct of linknode to link the list
        }
    }
    return hn;
}

hash_node* sdt_Dec(Node* this_root, int is_struct){
    //Dec: VarDec
    //|VarDec ASSIGNOP Exp
    Node *n0 = get_n_child(this_root, 0);
    Node *n2 = get_n_child(this_root, 2);
    hash_node *hn = sdt_VarDec(n0);
    if(n2){
        type *t2 = sdt_Exp(n2);
        if(t2){
            t2->is_rval = 0;
            hn->type = t2;
            char *ch = temp_char_array(temp_count);
            //intr_code_link *icl = translate_exp(n2, h, temp_char_array(temp_count++));
            //tail_intr_code_link =  link_icl(tail_intr_code_link, icl);
        }
    }
    return hn;
}

type* sdt_Exp(Node* this_root){
    //Exp: 1Exp AND Exp
    //|2Exp OR Exp
    //|3Exp RELOP Exp
    //|4Exp ADD Exp
    //|5Exp SUB Exp
    //|6Exp MUL Exp
    //|7Exp DIV Exp
    //|8LP Exp RP
    //|9SUB Exp %prec NEG
    //|10NOT Exp
    //|11ID LP Args RP
    //|12ID LP RP
    //|13Exp LB Exp RB
    //|14Exp DOT ID
    //|15Exp ASSIGNOP Exp
    //|16ID
    //|17INT
    //|18FLOAT
    Node *n0 = get_n_child(this_root, 0);
    if(strcmp(n0->name, "Exp") == 0){
        type *t0 = sdt_Exp(n0);
        Node *n1 = get_n_child(this_root, 1);
        //...
        Node *n2 = get_n_child(this_root, 2);
        if(strcmp(n2->name, "Exp") == 0){
            type *t2 = sdt_Exp(n2);
            if(strcmp(n1->name, "LB") == 0){//13
                if(t2->kind != BASIC || t2->u.basic != 1){
                    put_error_12(n2->column);
                }
                if(t0->kind != ARRAY){
                    put_error_10(n0->column);
                    return t0;
                }
                else{
                    return t0->u.array.elem;
                }
            }
            else if(strcmp(n1->name, "ASSIGNOP") == 0){//15
                if(!typecmp(t0, t2) && t0 != nullptr){
                    put_error_5(n1->column);
                }
                else if(t0 != nullptr && t0->is_rval){
                    put_error_6(n0->column);
                }
                else{
                    return t2;
                }
            }
            else{//1 2 3 4 5 6 7
                if(!typecmp(t0, t2) && t0 != nullptr && t2 != nullptr){
                    put_error_7(n2->column);
                }
                else{
                    return t2;
                }
            }
        }
        else if(strcmp(n2->name, "ID") == 0){//14
            //undo
            if(t0->kind == STRUCTURE){
                field_list *cur_fl = t0->u.structure;
                while(cur_fl != nullptr){
                    if(strcmp(cur_fl->name, n2->val.char_val) == 0){
                        return cur_fl->type;
                    }
                    cur_fl = cur_fl->tail;
                }
                put_error_14(n0->column, n2->val.char_val);
            }
            else{
                put_error_13(n0->column);
            }
        }
        else{
            printf("wrong expression in sdt_Exp");
        }
    }
    else if(strcmp(n0->name, "LP") == 0){//8
        Node *n1 = get_n_child(this_root, 1);
        return sdt_Exp(n1);
    }
    else if(strcmp(n0->name, "SUB") == 0){//9
        Node *n1 = get_n_child(this_root, 1);
        return sdt_Exp(n1);
    }
    else if(strcmp(n0->name, "NOT") == 0){//10
        Node *n1 = get_n_child(this_root, 1);
        return sdt_Exp(n1);
    }
    else if(strcmp(n0->name, "ID") == 0){
        Node *n1 = get_n_child(this_root, 1);
        /*if(n1){//11 12 function // before 2.1
        
            hash_node *hn = search_hash(h, n0->val.char_val);
            if(hn){
                if(hn->type->kind == FUNC){
                    Node *n2 = get_n_child(this_root, 2);
                    if(strcmp(n2->name, "Args") == 0){//11
                        args_node *cur2 = sdt_Args(n2);
                        args_node *cur1 = hn->type->u.func->args_node;
                        while(cur1 != nullptr && cur2 != nullptr){
                            if(typecmp(cur1->arg_type, cur2->arg_type)){
                                cur1 = cur1->next_arg;
                                cur2 = cur2->next_arg;
                            }
                            else{
                                put_error_9(n0->column);
                                break;
                            }
                        }
                        if(cur1 != nullptr || cur2 != nullptr){
                            put_error_9(n0->column);
                        }
                    }
                    else{//12
                        if(hn->type->u.func->args_node == nullptr){
                            return hn->type->u.func->return_type;
                        }
                        else{
                            put_error_9(n0->column);
                        }
                    }
                return hn->type->u.func->return_type;
                }
                else{
                    put_error_11(n0->column, hn->key);
                }
            }
            else{
                put_error_2(n0->column, n0->val.char_val);
            }
        }*/
        if(n1){
            func_decl *fdec = check_declearation(fdm, n0->val.char_val);
            if(fdec){
                Node *n2 = get_n_child(this_root, 2);
                if(strcmp(n2->name, "Args") == 0){//11
                    args_node *cur2 = sdt_Args(n2);
                    args_node *cur1 = fdec->func_desc->args_node;
                    while(cur1 != nullptr && cur2 != nullptr){
                        if(typecmp(cur1->arg_type, cur2->arg_type)){
                            cur1 = cur1->next_arg;
                            cur2 = cur2->next_arg;
                        }
                        else{
                            put_error_9(n0->column);
                            break;
                        }
                    }                        
                    if(cur1 != nullptr || cur2 != nullptr){
                        put_error_9(n0->column);
                    }
                }
                else{//12
                    if(fdec->func_desc->args_node == nullptr){
                        return fdec->func_desc->return_type;
                    }
                    else{
                        put_error_9(n0->column);
                    }
                }
                return fdec->func_desc->return_type;
            }
            else{
                hash_node *hn = search_hash(h, n0->val.char_val);
                if(hn && hn->type->kind != FUNC){
                    put_error_11(n0->column, hn->key);
                }
                else{
                    put_error_2(n0->column, n0->val.char_val);
                }
            }
        }
        else{//16 valuable
            hash_node *hn = search_hash(h, n0->val.char_val);
            if(hn){
                return hn->type;
            }
            else{
                put_error_1(n0->column, n0->val.char_val);
            }
        }
    }
    else if(strcmp(n0->name, "INT") == 0){//17
        type *t = (type *)malloc(sizeof(type));
        t->kind = BASIC;
        t->u.basic = 1;
        t->is_rval = 1;
        return t;
    }
    else if(strcmp(n0->name, "FLOAT") == 0){//18
        type *t = (type *)malloc(sizeof(type));
        t->kind = BASIC;
        t->u.basic = 2;
        t->is_rval = 1;
        return t;
    }
    else{
        printf("wrong expression in sdt_Exp");
    }
    return nullptr;
}

args_node* sdt_Args(Node* this_root){
    //Args: Exp COMMA Args
    //|Exp
    args_node *a0;
    Node *n0 = get_n_child(this_root, 0);
    Node *n2 = get_n_child(this_root, 2);
    type *t = sdt_Exp(n0);
    if(n2){
        a0 = sdt_Args(n2);
        args_node *a1 = (args_node *)malloc(sizeof(args_node));
        a1->arg_type = t;
        a1->next_arg = a0;
        return a1;
    }
    else{
        a0 = (args_node *)malloc(sizeof(args_node));
        a0->next_arg = nullptr;
        a0->arg_type = t;
        return a0;
    }
}



//valuable not decleared
void put_error_1(int column, char* id){
    printf("Error type 1 at Line %d: valuable \"%s\" hasn't been decleared in this scope.\n", column, id);
}

//function not decleared
void put_error_2(int column, char* id){
    printf("Error type 2 at Line %d: function \"%s\" hasn't been decleared when calling.\n", column, id);
}

//valuable redecleared
void put_error_3(int column, char* id){
    printf("Error type 3 at Line %d: \"%s\" has already been decleared.\n", column, id);
}

//function redecleared
void put_error_4(int column, char* id){
    printf("Error type 4 at Line %d: \"%s\" has already been decleared.\n", column, id);
}

//mismatching type operation
void put_error_5(int column){
    printf("Error type 5 at Line %d: mismatched type on two sides of '='.\n", column);
}

//right value in the left of '='
void put_error_6(int column){
    printf("Error type 6 at Line %d: right value in the left of '='.\n", column);
}

//mismatched operand
void put_error_7(int column){
    printf("Error type 7 at Line %d: mismatched operand type.\n", column);
}

//wrong return type
void put_error_8(int column){
    printf("Error type 8 at Line %d: wrong return type.\n", column);
}

//unmatched parameters
void put_error_9(int column){
    printf("Error type 9 at Line %d: unmatched parameters.\n", column);
}

//using [] to unarray value
void put_error_10(int column){
    printf("Error type 10 at Line %d: using [] to unarray value.\n", column);
}

//using [] to no-function value
void put_error_11(int column, char* id){
    printf("Error type 11 at Line %d: using () to no-function value \"%s\".\n", column, id);
}

//unexpected number in []
void put_error_12(int column){
    printf("Error type 12 at Line %d: unexpected float number in [].\n", column);
}

//using . to no_struct value
void put_error_13(int column){
    printf("Error type 13 at Line %d: using . to no_struct value.\n", column);
}

//call undefined value in struct
void put_error_14(int column, char* id){
    printf("Error type 14 at Line %d: call undefined value in struct \"%s\".\n", column, id);
}

//redefined same struct in struct or initialize when defining a struct
void put_error_15(int column){
    printf("Error type 15 at Line %d: redefined same struct in struct or initialize when defining struct.\n", column);
}

//struct redecleared
void put_error_16(int column, char* id){
    printf("Error type 16 at Line %d: \"%s\" has already been decleared.\n", column, id);
}


//struct not decleared
void put_error_17(int column, char* id){
    printf("Error type 17 at Line %d: struct \"%s\" hasn't been decleared when using.\n", column, id);
}

//Inconsistent declaration of function
void put_error_19(int column, char* id){
    printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", column, id);
}

//type compare
int typecmp(type *t1, type *t2){
    if(t1 == nullptr || t2 == nullptr){
        return 0;
    }
    else if(t1->kind == BASIC && t2->kind == BASIC){
        if(t1->u.basic == t2->u.basic){
            return 1;
        }
        else{
            return 0;
        }
    }
    else if(t1->kind == ARRAY && t2->kind == ARRAY){
        if(t1->u.array.elem == t2->u.array.elem && t1->u.array.size == t2->u.array.size){
            return 1;
        }
        else{
            return 0;
        }
    }
    else if(t1->kind == STRUCTURE && t2->kind == STRUCTURE){
        char name_list[100][32];
        int num_list = 0;
        int cont = 1;
        int find = 0;
        field_list *cur1 = t1->u.structure;
        while(cur1 != nullptr){
            field_list *cur2 = t2->u.structure;
            find = 0;
            while(cur2 != nullptr){
                if(cur1->type->kind != ARRAY && cur2->type->kind != ARRAY){
                    if(typecmp(cur1->type, cur2->type)){
                        cont = 1;
                        for(int i = 0; i < num_list; i++){
                            if(strcmp(name_list[i], cur2->name) == 0){
                                cont = 0;
                                break;
                            }
                        }
                        if(cont){
                            find = 1;
                            strcpy(name_list[num_list], cur2->name);
                            num_list++;
                            break;
                        }
                    }
                }
                else{
                    return 0;
                }
                cur2 = cur2->tail;
            }
            if(!find){
                return 0;
            }
            cur1 = cur1->tail;
        }
        return 1;
    }
    else if(t1->kind == FUNC && t2->kind == FUNC){
        if(typecmp(t1->u.func->return_type, t2->u.func->return_type)){
            args_node *cur1 = t1->u.func->args_node;
            args_node *cur2 = t2->u.func->args_node;
            while(cur1 != nullptr && cur2 != nullptr){
                if(typecmp(cur1->arg_type, cur2->arg_type)){
                    cur1 = cur1->next_arg;
                    cur2 = cur2->next_arg;
                }
                else{
                    return 0;
                }
            }
            if(cur1 != nullptr || cur2 != nullptr){
                return 0;
            }
        }
        else{
            return 0;
        }
        return 1;
    }
    return 0;
}