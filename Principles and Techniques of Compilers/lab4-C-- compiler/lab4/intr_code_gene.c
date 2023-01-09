#include "intr_code_gene.h"

char* temp_char_array(int number){
    int i;
    int n = (int)log10((double)number) + 3;
    char *temp_char = (char *)malloc(n * sizeof(char));
    temp_char[n - 1] = '\0';
    temp_char[0] = 't';
    for(i = n - 2; i > 0 && number > 0; --i, number /= 10){
        temp_char[i] = (number % 10) + '0';
    }
    return temp_char;
}

char* label_char_array(int number){
    int i;
    int n = (int)log10((double)number) + 3;
    char *label_char = (char *)malloc(n * sizeof(char));
    label_char[n - 1] = '\0';
    label_char[0] = 'l';
    for(i = n - 2; i > 0 && number > 0; --i, number /= 10){
        label_char[i] = (number % 10) + '0';
    }
    return label_char;
}

char* variable_char_array(int number){
    int i;
    int n = (int)log10((double)number) + 3;
    char *v_char = (char *)malloc(n * sizeof(char));
    v_char[n - 1] = '\0';
    v_char[0] = 'v';
    for(i = n - 2; i > 0 && number > 0; --i, number /= 10){
        v_char[i] = (number % 10) + '0';
    }
    return v_char;
}

intr_code_link* link_icl(intr_code_link *icl1, intr_code_link *icl2){
    if(icl1 && icl2){
        intr_code_link *cur = icl1;
        while(cur->next != nullptr){
            cur = cur->next;
        }
        cur->next = icl2;
        icl2->prev = cur;
    }
    else{
        printf("error nullptr icl1 %ld or icl2 %ld in link_icl\n", (size_t)icl1, (size_t)icl2);
    }
    intr_code_link *cur = icl2;
    while(cur->next != nullptr){
        cur = cur->next;
    }
    return cur;
}

void init_intr_code_gene(intr_code_link *head, hash_map *hm, stack *s, func_decl_map *fdm){
    temp_count = 1;
    label_count = 1;
    variable_count = 1;
    head->prev = nullptr;
    head->next = nullptr;
    head->code = nullptr;
    hash_node *read_func = (hash_node *)malloc(sizeof(hash_node));
    strcpy(read_func->key, "read");
    type *t = (type *)malloc(sizeof(type));
    t->kind = FUNC;
    type *t0 = (type *)malloc(sizeof(type));
    t0->kind = BASIC;
    t0->u.basic = 1;
    func_desc *fd = (func_desc *)malloc(sizeof(func_desc *));
    fd->return_type = t0;
    fd->args_node = nullptr;
    t->u.func = fd;
    read_func->type = t;
    insert_hash(hm, read_func);
    insert_stack(s, read_func);
    //add_declearation(fdm, "read", fd, 0);
    add_definition(fdm, "read", fd);

    hash_node *write_func = (hash_node *)malloc(sizeof(hash_node));
    strcpy(write_func->key, "write");
    type *t1 = (type *)malloc(sizeof(type));
    t1->kind = FUNC;
    type *t2 = (type *)malloc(sizeof(type));
    t2->kind = BASIC;
    t2->u.basic = 1;
    type *t3 = (type *)malloc(sizeof(type));
    t3->kind = BASIC;
    t3->u.basic = 1;
    func_desc *fd1 = (func_desc *)malloc(sizeof(func_desc *));
    fd1->return_type = t2;
    args_node *an = (args_node *)malloc(sizeof(args_node));
    an->arg_type = t3;
    an->next_arg = nullptr;
    fd1->args_node = an;
    t->u.func = fd1;
    write_func->type = t1;
    insert_hash(hm, write_func);
    insert_stack(s, write_func);
    //add_declearation(fdm, "write", fd1, 0);
    add_definition(fdm, "write", fd1);
}

void optimize_intr_code(intr_code_link *head){

    //delete public subject expression
    intr_code_link *cur = head->next;
    public_sub_expr *public_sub_expr_head = (public_sub_expr *)malloc(sizeof(public_sub_expr));
    public_sub_expr_head->next = nullptr;
    while(cur != nullptr){
        operand v;
        if(cur->code->kind == ASSIGN_i){
            v = cur->code->u.assign.left;
        }
        else if(cur->code->kind == ADD_i || cur->code->kind == SUB_i || cur->code->kind == MUL_i || cur->code->kind == DIV_i){
            v = cur->code->u.three.result;
        }
        else if(cur->code->kind == READ_i){
            v = cur->code->u.one.op;
        }
        else if(cur->code->kind == LABEL_i || cur->code->kind == FUNCTION_i){
            public_sub_expr *public_sub_expr_head = (public_sub_expr *)malloc(sizeof(public_sub_expr));
            public_sub_expr_head->next = nullptr;
            cur = cur->next;
            continue;
        }
        /*
        else if(cur->code->kind == WRITE_i || cur->code->kind == ARG_i || cur->code->kind == RETURN_i || cur->code->kind == RELOP_i){

        }
        else{
            cur = cur->next;
            continue;
        }*/
        if(public_sub_expr_head->next == nullptr){
            public_sub_expr *temp = (public_sub_expr *)malloc(sizeof(public_sub_expr));
            temp->next = nullptr;
            temp->icl = cur;
            public_sub_expr_head->next = temp;
        }
        else{
            public_sub_expr *pcur = public_sub_expr_head->next;
            public_sub_expr *plast = public_sub_expr_head;
            while(pcur != nullptr){
                opcmp(cur->code, pcur->icl->code);
                operand v1, v2, v3;
                if(pcur->icl->code->kind == ASSIGN_i){
                    v1 = pcur->icl->code->u.assign.right;
                    v2 = pcur->icl->code->u.assign.left;
                    if(strcmp(v1.name, v.name) == 0){
                        plast->next = pcur->next;
                        pcur = pcur->next;
                        continue;
                    }
                    if(strcmp(v2.name, v.name) == 0){
                        plast->next = pcur->next;
                        pcur = pcur->next;
                        continue;
                    }
                }
                else if(pcur->icl->code->kind == ADD_i || pcur->icl->code->kind == SUB_i || pcur->icl->code->kind == MUL_i || pcur->icl->code->kind == DIV_i){
                    v1 = pcur->icl->code->u.three.result;
                    v2 = pcur->icl->code->u.three.op1;
                    v3 = pcur->icl->code->u.three.op2;
                    if(strcmp(v1.name, v.name) == 0){
                        plast->next = pcur->next;
                        pcur = pcur->next;
                        continue;
                    }
                    if(strcmp(v2.name, v.name) == 0){
                        plast->next = pcur->next;
                        pcur = pcur->next;
                        continue;
                    }
                    if(strcmp(v3.name, v.name) == 0){
                        plast->next = pcur->next;
                        pcur = pcur->next;
                        continue;
                    }
                }
                else{
                    plast = pcur;
                    pcur = pcur->next;
                    continue;
                }
                plast = pcur;
                pcur = pcur->next;
            }
            if(cur->code->kind == ASSIGN_i || cur->code->kind == ADD_i || cur->code->kind == SUB_i || cur->code->kind == MUL_i || cur->code->kind == DIV_i){
                public_sub_expr *temp = (public_sub_expr *)malloc(sizeof(public_sub_expr));
                temp->next = nullptr;
                temp->icl = cur;
                plast->next = temp;
            }
        }
        cur = cur->next;
    }

    //delete dead code
    cur = head->next;
    intr_code_link *last = head;

    while(cur != nullptr){
        operand v;
        if(cur->code->kind == ASSIGN_i){
            v = cur->code->u.assign.left;
        }
        else if(cur->code->kind == ADD_i || cur->code->kind == SUB_i || cur->code->kind == MUL_i || cur->code->kind == DIV_i){
            v = cur->code->u.three.result;
        }
        //else{
        //    cur = cur->next;
        //    continue;
        //}
        intr_code_link *ncur = cur->next;
        int del = 1;
        if(cur->code->kind == FUNCTION_i || cur->code->kind == LABEL_i || cur->code->kind == GOTO_i || cur->code->kind == RELOP_i || cur->code->kind == RETURN_i || cur->code->kind == ARG_i || cur->code->kind == CALL_i || cur->code->kind == PARAM_i || cur->code->kind == WRITE_i || cur->code->kind == READ_i){
            del = 0;
        }
        if(v.kind == VARIABLE){
            del = 0;
        }
        while(ncur != nullptr && del){
            if(ncur->code->kind == ASSIGN_i){
                if(strcmp(ncur->code->u.assign.right.name, v.name) == 0 && ncur->code->u.assign.right.kind != CONSTANT){
                    del = 0;
                    break;
                }
                if(strcmp(ncur->code->u.assign.left.name, v.name) == 0){
                    break;
                }
            }
            else if(ncur->code->kind == ADD_i || ncur->code->kind == SUB_i || ncur->code->kind == MUL_i || ncur->code->kind == DIV_i){
                if(strcmp(ncur->code->u.three.op1.name, v.name) == 0 && ncur->code->u.three.op1.kind != CONSTANT){
                    del = 0;
                    break;
                }
                if(strcmp(ncur->code->u.three.op2.name, v.name) == 0 && ncur->code->u.three.op2.kind != CONSTANT){
                    del = 0;
                    break;
                }
                if(strcmp(ncur->code->u.three.result.name, v.name) == 0){
                    break;
                }
            }
            else if(ncur->code->kind == WRITE_i || ncur->code->kind == ARG_i || ncur->code->kind == RETURN_i){
                if(strcmp(ncur->code->u.one.op.name, v.name) == 0 && ncur->code->u.one.op.kind != CONSTANT){
                    del = 0;
                    break;
                }
            }
            else if(ncur->code->kind == RELOP_i){
                if(strcmp(ncur->code->u.relop.op1.name, v.name) == 0 && ncur->code->u.relop.op1.kind != CONSTANT){
                    del = 0;
                    break;
                }
                if(strcmp(ncur->code->u.relop.op2.name, v.name) == 0 && ncur->code->u.relop.op2.kind != CONSTANT){
                    del = 0;
                    break;
                }
            }
            ncur = ncur->next;
        }
        //if(cur->code->kind == LABEL_i && cur->next && cur->next->code->kind == LABEL_i){
        //    del = 1;
        //}
        if(del){
            last->next = cur->next;
            cur = cur->next;
        }
        else{
            last = cur;
            cur = cur->next;
        }
    }
}

int opcmp(intr_code *ic1, intr_code *ic2){
    if(ic1->kind == ic2->kind && ic2->kind == ASSIGN_i){
        if(ic1->u.assign.right.kind == ic2->u.assign.left.kind && ic2->u.assign.left.kind == TEMP){
            //printf("%s %s\n", ic1->u.assign.right.name, ic2->u.assign.left.name);
            if(strcmp(ic1->u.assign.right.name, ic2->u.assign.left.name) == 0){
                if(ic2->u.assign.right.kind == CONSTANT){
                    operand o1 = ic2->u.assign.right;
                    strcpy(o1.name, ic1->u.assign.right.name);
                    ic1->u.assign.right = o1;
                }
                else{
                    ic1->u.assign.right = ic2->u.assign.right;
                }
            }
        }
    }
    else if((ic1->kind == ic2->kind && ic2->kind == ADD_i) || (ic1->kind == ic2->kind && ic2->kind == MUL_i) || (ic1->kind == ic2->kind && ic2->kind == SUB_i) || (ic1->kind == ic2->kind && ic2->kind == DIV_i)){
        int retval = 1;
        if((ic1->u.three.op1.kind == ic2->u.three.op1.kind && ic2->u.three.op1.kind == VARIABLE) || (ic1->u.three.op1.kind == ic2->u.three.op1.kind && ic2->u.three.op1.kind == TEMP)){
            if(strcmp(ic1->u.three.op1.name, ic2->u.three.op1.name) != 0){
                retval = 0;
            }
            else if((ic1->u.three.op2.kind == ic2->u.three.op2.kind && ic2->u.three.op2.kind == VARIABLE) || (ic1->u.three.op2.kind == ic2->u.three.op2.kind && ic2->u.three.op2.kind == TEMP)){
                if(strcmp(ic1->u.three.op2.name, ic2->u.three.op2.name) != 0){
                    retval = 0;
                }
            }
            else if(ic1->u.three.op2.kind == ic2->u.three.op2.kind && ic2->u.three.op2.kind == CONSTANT){
                if(ic1->u.three.op2.u.value != ic2->u.three.op2.u.value){
                    retval = 0;
                }
            }
        }
        else if(ic1->u.three.op1.kind == ic2->u.three.op1.kind && ic2->u.three.op1.kind == CONSTANT){
            if(ic1->u.three.op1.u.value != ic2->u.three.op1.u.value){
                retval = 0;
            }
            else if((ic1->u.three.op2.kind == ic2->u.three.op2.kind && ic2->u.three.op2.kind == VARIABLE) || (ic1->u.three.op2.kind == ic2->u.three.op2.kind && ic2->u.three.op2.kind == TEMP)){
                if(strcmp(ic1->u.three.op2.name, ic2->u.three.op2.name) != 0){
                    retval = 0;
                }
            }
            else if(ic1->u.three.op2.kind == ic2->u.three.op2.kind && ic2->u.three.op2.kind == CONSTANT){
                if(ic1->u.three.op2.u.value != ic2->u.three.op2.u.value){
                    retval = 0;
                }
            }
        }
        if(retval){
            operand v = ic1->u.three.result;
            ic1->kind = ASSIGN_i;
            ic1->u.assign.left = v;
            ic1->u.assign.right = ic2->u.three.result;
        }
        return retval;
    }
    
    else if((ic1->kind == ADD_i || ic1->kind == MUL_i || ic1->kind == SUB_i || ic1->kind == DIV_i) && ic2->kind == ASSIGN_i){
        if(ic1->u.three.op1.kind == ic2->u.assign.left.kind && ic2->u.assign.left.kind == TEMP){
            if(strcmp(ic1->u.three.op1.name, ic2->u.assign.left.name) == 0){
                if(ic2->u.assign.right.kind == CONSTANT){
                    operand o1 = ic2->u.assign.right;
                    strcpy(o1.name, ic1->u.three.op1.name);
                    ic1->u.three.op1 = o1;
                }
                else{
                    ic1->u.three.op1 = ic2->u.assign.right;
                }
            }
        }
        if(ic1->u.three.op2.kind == ic2->u.assign.left.kind && ic2->u.assign.left.kind == TEMP){
            if(strcmp(ic1->u.three.op2.name, ic2->u.assign.left.name) == 0){
                if(ic2->u.assign.right.kind == CONSTANT){
                    operand o1 = ic2->u.assign.right;
                    strcpy(o1.name, ic1->u.three.op2.name);
                    ic1->u.three.op2 = o1;
                }
                else{
                    ic1->u.three.op2 = ic2->u.assign.right;
                }
            }
        }
    }
    else if((ic1->kind == RETURN_i || ic1->kind == WRITE_i || ic1->kind == ARG_i) && ic2->kind == ASSIGN_i){
        operand v2 = ic2->u.assign.left;
        //printf("%s %s\n", ic1->u.one.op.name, v2.name);
        if(ic1->u.one.op.kind == v2.kind && v2.kind == TEMP){
            if(strcmp(ic1->u.one.op.name, v2.name) == 0){
                if(ic2->u.assign.right.kind == CONSTANT){
                    operand o1 = ic2->u.assign.right;
                    strcpy(o1.name, ic1->u.one.op.name);
                    ic1->u.one.op = o1;
                }
                else{
                    ic1->u.one.op = ic2->u.assign.right;
                }
            }
        }
    }
    else if(ic1->kind == RELOP_i && ic2->kind == ASSIGN_i){
        if(ic1->u.relop.op1.kind == ic2->u.assign.left.kind && ic2->u.assign.left.kind == TEMP){
            if(strcmp(ic1->u.relop.op1.name, ic2->u.assign.left.name) == 0){
                if(ic2->u.assign.right.kind == CONSTANT){
                    operand o1 = ic2->u.assign.right;
                    strcpy(o1.name, ic1->u.relop.op1.name);
                    ic1->u.relop.op1 = o1;
                }
                else{
                    ic1->u.relop.op1 = ic2->u.assign.right;
                }
            }
        }
        if(ic1->u.relop.op2.kind == ic2->u.assign.left.kind && ic2->u.assign.left.kind == TEMP){
            if(strcmp(ic1->u.relop.op2.name, ic2->u.assign.left.name) == 0){
                if(ic2->u.assign.right.kind == CONSTANT){
                    operand o1 = ic2->u.assign.right;
                    strcpy(o1.name, ic1->u.relop.op2.name);
                    ic1->u.relop.op2 = o1;
                }
                else{
                    ic1->u.relop.op2 = ic2->u.assign.right;
                }
            }
        }

    }
    else if(ic1->kind == ic2->kind == GOTO_i){
    }
    else if(ic1->kind == ic2->kind == LABEL_i){
    }
    else if(ic1->kind == ic2->kind == CALL_i){
    }
    else if(ic1->kind == ic2->kind == READ_i){
        //
    }
    else if(ic1->kind == ic2->kind == FUNCTION_i){
    }
    else if(ic1->kind == ic2->kind == PARAM_i){
    }
}


intr_code_link* translate_exp(Node *exp, hash_map *hm, char* place){
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
    Node *n0 = get_n_child(exp, 0);
    if(strcmp(n0->name, "Exp") == 0){
        Node *n1 = get_n_child(exp, 1);
        Node *n2 = get_n_child(exp, 2);
        if(strcmp(n2->name, "Exp") == 0){
            if(strcmp(n1->name, "LB") == 0){//13 array
                //t1 = exp2
                //t2 = t1 * sizeof(type)
                //t3 = &exp1
                //t4 = t3 + t2
                //place = *t4
                char *t1 = temp_char_array(temp_count++);
                char *t2 = temp_char_array(temp_count++);
                char *t3 = temp_char_array(temp_count++);
                Node *cur = n0->first_child;
                int count = 0;
                while(strcmp(cur->name, "ID") != 0){
                    ++count;
                    cur = cur->first_child;
                }
                hash_node *hn = search_hash(hm, cur->val.char_val);
                intr_code_link *icl1 = translate_exp(n2, hm, t1);
                intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code_link *icl4 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code_link *icl5 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
                icl2->next = nullptr;
                icl2->code = ic1;
                ic1->kind = MUL_i;
                operand o1;
                strcpy(o1.name, t1);
                o1.kind = TEMP;
                operand o2;
                o2.kind = CONSTANT;
                type *tpe = hn->type;
                for(int i = 0; i < count; i++){
                    tpe = tpe->u.array.elem;
                }
                if(tpe->u.array.elem->kind == BASIC){
                    o2.u.value = 4;
                }
                else if(tpe->u.array.elem->kind == ARRAY){
                    o2.u.value = 4 * tpe->u.array.elem->u.array.size;
                }
                else{
                    o2.u.value = 4;
                    printf("undo: struct array!\n");
                }
                operand o3;
                o3.kind = TEMP;
                strcpy(o3.name, t2);
                ic1->u.three.result = o3;
                ic1->u.three.op1 = o1;
                ic1->u.three.op2 = o2;

                if(strcmp(n0->first_child->name, "ID") == 0){
                    intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
                    icl3->next = nullptr;
                    icl3->code = ic2;
                    ic2->kind = ADDR_i;
                    operand o4;
                    operand o5;
                    o4.kind = TEMP;
                    strcpy(o4.name, t3);
                    o5.kind = VARIABLE;
                    strcpy(o5.name, n0->first_child->val.char_val);
                    ic2->u.two.op1 = o4;
                    ic2->u.two.op2 = o5;
                }
                else{
                    icl3 = translate_exp(n0, hm, t3);
                }
                
                intr_code *ic3 = (intr_code *)malloc(sizeof(intr_code));
                icl4->next = nullptr;
                icl4->code = ic3;
                ic3->kind = ADD_i;
                operand o6;
                strcpy(o6.name, t2);
                o6.kind = TEMP;
                operand o7;
                o7.kind = TEMP;
                strcpy(o7.name, t3);
                operand o8;
                o8.kind = TEMP;
                if(tpe->u.array.elem->kind == BASIC){
                    char *t4 = temp_char_array(temp_count++);
                    strcpy(o8.name, t4);
                    intr_code *ic4 = (intr_code *)malloc(sizeof(intr_code));
                    icl5->next = nullptr;
                    icl5->code = ic4;
                    ic4->kind = F_STAR_i;
                    operand o9;
                    operand o10;
                    o9.kind = TEMP;
                    strcpy(o9.name, t4);
                    o10.kind = TEMP;
                    strcpy(o10.name, place);
                    ic4->u.two.op1 = o10;
                    ic4->u.two.op2 = o9;
                    link_icl(icl4, icl5);
                }
                else{
                    strcpy(o8.name, place);
                }
                ic3->u.three.result = o8;
                ic3->u.three.op1 = o6;
                ic3->u.three.op2 = o7;
                link_icl(icl1, icl2);
                link_icl(icl2, icl3);
                link_icl(icl3, icl4);

                return icl1;
            }
            else if(n1 && (strcmp(n1->name, "ASSIGNOP") == 0)){//15
                char *t1 = temp_char_array(temp_count++);
                intr_code_link *icl1 = translate_exp(n2, hm, t1);
                intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
                link_icl(icl1, icl2);
                intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
                operand o1;
                o1.kind = VARIABLE;
                strcpy(o1.name, n0->first_child->val.char_val);
                operand o2;
                o2.kind = TEMP;
                strcpy(o2.name, t1);
                ic1->kind = ASSIGN_i;
                ic1->u.assign.left = o1;
                ic1->u.assign.right = o2;
                icl2->code = ic1;
                intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
                link_icl(icl2, icl3);
                icl3->next = nullptr;
                intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
                operand o3;
                o3.kind = TEMP;
                strcpy(o3.name, place);
                operand o4;
                o4.kind = VARIABLE;
                strcpy(o4.name, n0->first_child->val.char_val);
                ic2->kind = ASSIGN_i;
                ic2->u.assign.left = o3;
                ic2->u.assign.right = o4;
                icl3->code = ic2;
                return icl1;
            }
            else if(n1 && (strcmp(n1->name, "AND") == 0 || strcmp(n1->name, "OR") == 0 || strcmp(n1->name, "RELOP") == 0)){//1 2 3
                char *label1 = label_char_array(label_count++);
                char *label2 = label_char_array(label_count++);
                intr_code_link *icl1 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
                icl1->code = ic1;
                icl1->next = nullptr;
                ic1->kind = ASSIGN_i;
                operand o1;
                o1.kind = TEMP;
                strcpy(o1.name, place);
                operand o2;
                o2.kind = CONSTANT;
                o2.u.value = 0;
                ic1->u.assign.left = o1;
                ic1->u.assign.right = o2;
                intr_code_link *icl2 = translate_cond(exp, label1, label2, hm);
                intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code_link *icl4 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code_link *icl5 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
                intr_code *ic3 = (intr_code *)malloc(sizeof(intr_code));
                icl3->code = ic2;
                icl3->next = nullptr;
                ic2->kind = LABEL_i;
                ic2->u.label.label = label1;
                icl5->code = ic3;
                icl5->next = nullptr;
                ic3->kind = LABEL_i;
                ic3->u.label.label = label2;
                intr_code *ic4 = (intr_code *)malloc(sizeof(intr_code));
                icl4->code = ic4;
                icl4->next = nullptr;
                ic4->kind = ASSIGN_i;
                operand o3;
                o3.kind = TEMP;
                strcpy(o3.name, place);
                operand o4;
                o4.kind = CONSTANT;
                o4.u.value = 1;
                ic4->u.assign.left = o3;
                ic4->u.assign.right = o4;
                link_icl(icl1, icl2);
                link_icl(icl2, icl3);
                link_icl(icl3, icl4);
                link_icl(icl4, icl5);
                return icl1;
            }
            else{//4 5 6 7
                char *t1 = temp_char_array(temp_count++);
                char *t2 = temp_char_array(temp_count++);
                intr_code_link *icl1 = translate_exp(n0, hm, t1);
                intr_code_link *icl2 = translate_exp(n2, hm, t2);
                link_icl(icl1, icl2);
                intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
                link_icl(icl2, icl3);
                icl3->next = nullptr;
                intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
                operand o1;
                o1.kind = TEMP;
                strcpy(o1.name, t1);
                operand o2;
                o2.kind = TEMP;
                strcpy(o2.name, t2);
                operand o3;
                o3.kind = TEMP;
                strcpy(o3.name, place);
                if(strcmp(n1->name, "ADD") == 0){
                    ic->kind = ADD_i;
                }
                else if(strcmp(n1->name, "SUB") == 0){
                    ic->kind = SUB_i;
                }
                else if(strcmp(n1->name, "MUL") == 0){
                    ic->kind = MUL_i;
                }
                else{
                    ic->kind = DIV_i;
                }
                ic->u.three.op1 = o1;
                ic->u.three.op2 = o2;
                ic->u.three.result = o3;
                icl3->code = ic;
                return icl1;
            }
        }
        else if(strcmp(n2->name, "ID") == 0){//14

        }
    }
    else if(strcmp(n0->name, "LP") == 0){//8
        Node *n1 = get_n_child(exp, 1);
        return translate_exp(n1, hm, place);
    }
    else if(n0 && (strcmp(n0->name, "SUB") == 0)){//9
        Node *n1 = get_n_child(exp, 1);
        char *t1 = temp_char_array(temp_count++);
        intr_code_link *icl1 = translate_exp(n1, hm, t1);
        intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
        icl2->next = nullptr;
        icl2->code = ic;
        operand o1;
        o1.kind = CONSTANT;
        o1.u.value = 0;
        operand o2;
        o2.kind = TEMP;
        strcpy(o2.name, t1);
        operand o3;
        o3.kind = TEMP;
        strcpy(o3.name, place);
        ic->kind = SUB_i;
        ic->u.three.op1 = o1;
        ic->u.three.op2 = o2;
        ic->u.three.result = o3;
        link_icl(icl1, icl2);
        return icl1;
    }
    else if(n0 && (strcmp(n0->name, "NOT") == 0)){//10
        char *label1 = label_char_array(label_count++);
                char *label2 = label_char_array(label_count++);
                intr_code_link *icl1 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
                icl1->code = ic1;
                icl1->next = nullptr;
                ic1->kind = ASSIGN_i;
                operand o1;
                o1.kind = TEMP;
                strcpy(o1.name, place);
                operand o2;
                o2.kind = CONSTANT;
                o2.u.value = 0;
                ic1->u.assign.left = o1;
                ic1->u.assign.right = o2;
                intr_code_link *icl2 = translate_cond(exp, label1, label2, hm);
                intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code_link *icl4 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code_link *icl5 = (intr_code_link *)malloc(sizeof(intr_code_link));
                intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
                intr_code *ic3 = (intr_code *)malloc(sizeof(intr_code));
                icl3->code = ic2;
                icl3->next = nullptr;
                ic2->kind = LABEL_i;
                ic2->u.label.label = label1;
                icl5->code = ic3;
                icl5->next = nullptr;
                ic3->kind = LABEL_i;
                ic3->u.label.label = label2;
                intr_code *ic4 = (intr_code *)malloc(sizeof(intr_code));
                icl4->code = ic4;
                icl4->next = nullptr;
                ic4->kind = ASSIGN_i;
                operand o3;
                o3.kind = TEMP;
                strcpy(o3.name, place);
                operand o4;
                o4.kind = CONSTANT;
                o4.u.value = 1;
                ic4->u.assign.left = o3;
                ic4->u.assign.right = o4;
                link_icl(icl1, icl2);
                link_icl(icl2, icl3);
                link_icl(icl3, icl4);
                link_icl(icl4, icl5);
                return icl1;
    }
    else if(n0 && (strcmp(n0->name, "ID") == 0)){
        Node *n1 = get_n_child(exp, 1);
        if(n1){//function 11 12
            Node *n2 = get_n_child(exp, 2);
            if(strcmp(n2->name, "Args") == 0){//12
                hash_node *hn = search_hash(hm, n0->val.char_val);
                arg_list *head = (arg_list *)malloc(sizeof(arg_list));
                intr_code_link *icl1 = translate_args(n2, hm, nullptr, head);
                if(strcmp(hn->key, "write") == 0){
                    intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
                    intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
                    icl2->next = nullptr;
                    icl2->code = ic1;
                    ic1->kind = WRITE_i;
                    operand op;
                    op.kind = VARIABLE;
                    strcpy(op.name, head->name);
                    ic1->u.one.op = op;
                    intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
                    intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
                    icl3->next = nullptr;
                    icl3->code = ic2;
                    ic2->kind = ASSIGN_i;
                    operand o3;
                    o3.kind = TEMP;
                    strcpy(o3.name, place);
                    operand o4;
                    o4.kind = CONSTANT;
                    o4.u.value = 0;
                    ic2->u.assign.left = o3;
                    ic2->u.assign.right = o4;
                    link_icl(icl1, icl2);
                    link_icl(icl2, icl3);
                    return icl1;
                }
                else{
                    intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
                    intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
                    icl2->next = nullptr;
                    icl2->code = ic1;
                    ic1->kind = ARG_i;
                    operand op;
                    op.kind = VARIABLE;
                    strcpy(op.name, head->name);
                    ic1->u.one.op = op;
                    arg_list *cur = head->next;
                    while(cur != nullptr){
                        intr_code_link *iclt = (intr_code_link *)malloc(sizeof(intr_code_link));
                        intr_code *ict = (intr_code *)malloc(sizeof(intr_code));
                        iclt->next = nullptr;
                        iclt->code = ict;
                        ict->kind = ARG_i;
                        operand opt;
                        opt.kind = VARIABLE;
                        strcpy(opt.name, cur->name);
                        ict->u.one.op = opt;
                        link_icl(icl2, iclt);
                        cur = cur->next;
                    }
                    intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
                    intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
                    icl3->next = nullptr;
                    icl3->code = ic2;
                    ic2->kind = CALL_i;
                    operand op1;
                    op1.kind = TEMP;
                    strcpy(op1.name, place);
                    ic2->u.two.op1 = op1;
                    operand op2;
                    op2.kind = VARIABLE;
                    strcpy(op2.name, hn->key);
                    ic2->u.two.op2 = op2;
                    link_icl(icl1, icl2);
                    link_icl(icl2, icl3);
                    return icl1;
                }
            }
            else{//11
                hash_node *hn = search_hash(hm, n0->val.char_val);
                if(strcmp(hn->key, "read") == 0){
                    intr_code_link *icl = (intr_code_link *)malloc(sizeof(intr_code_link));
                    intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
                    icl->next = nullptr;
                    icl->code = ic;
                    ic->kind = READ_i;
                    operand op;
                    op.kind = TEMP;
                    strcpy(op.name, place);
                    ic->u.one.op = op;
                    return icl;
                }
                else{
                    intr_code_link *icl = (intr_code_link *)malloc(sizeof(intr_code_link));
                    intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
                    icl->next = nullptr;
                    icl->code = ic;
                    ic->kind = CALL_i;
                    operand op1;
                    op1.kind = TEMP;
                    strcpy(op1.name, place);
                    ic->u.two.op1 = op1;
                    operand op2;
                    op2.kind = VARIABLE;
                    strcpy(op2.name, hn->key);
                    ic->u.two.op2 = op2;
                    return icl;
                }
            }
        }
        else{//16 value
            hash_node *hn = search_hash(hm, n0->val.char_val);
            if(hn){
                intr_code_link *icl = (intr_code_link *)malloc(sizeof(intr_code_link));
                icl->prev = nullptr;
                icl->next = nullptr;
                intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
                icl->code = ic;
                ic->kind = ASSIGN_i;
                operand o1;
                o1.kind = TEMP;
                strcpy(o1.name, place);
                ic->u.assign.left = o1;
                operand o2;
                o2.kind = VARIABLE;
                strcpy(o2.name, n0->val.char_val);
                ic->u.assign.right = o2;
                return icl;
            }
        }
    }
    else if(n0 && (strcmp(n0->name, "INT") == 0)){//17
        intr_code_link *icl = (intr_code_link *)malloc(sizeof(intr_code_link));
        icl->prev = nullptr;
        icl->next = nullptr;
        intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
        icl->code = ic;
        ic->kind = ASSIGN_i;
        operand o1;
        o1.kind = TEMP;
        strcpy(o1.name, place);
        ic->u.assign.left = o1;
        operand o2;
        o2.kind = CONSTANT;
        o2.u.value = n0->val.int_val;
        ic->u.assign.right = o2;
        return icl;
    }
    else if(strcmp(n0->name, "FLOAT") == 0){//18
        intr_code_link *icl = (intr_code_link *)malloc(sizeof(intr_code_link));
        icl->prev = nullptr;
        icl->next = nullptr;
        intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
        icl->code = ic;
        ic->kind = ASSIGN_i;
        operand o1;
        o1.kind = TEMP;
        strcpy(o1.name, place);
        ic->u.assign.left = o1;
        operand o2;
        o2.kind = CONSTANT;
        o2.u.value = n0->val.float_val;
        ic->u.assign.right = o2;
        return icl;
    }
    else{
        printf("wrong expression in sdt_Exp");
    }
    return nullptr;
}

intr_code_link* translate_args(Node *args, hash_map *hm, arg_list *arglist, arg_list *head){
    //Args: Exp COMMA Args
    //|Exp
    Node *n0 = get_n_child(args, 0);
    Node *n2 = get_n_child(args, 2);
    if(n2){
        char *t1 = temp_char_array(temp_count++);
        intr_code_link *icl1 = translate_exp(n0, hm, t1);
        arg_list *new_al = (arg_list *)malloc(sizeof(arg_list));
        strcpy(new_al->name, t1);
        new_al->next = arglist;
        new_al->kind = TEMP;
        intr_code_link *icl2 = translate_args(n2, hm, new_al, head);
        link_icl(icl1, icl2);
        return icl1;
    }
    else{
        char *t1 = temp_char_array(temp_count++);
        intr_code_link *icl = translate_exp(n0, hm, t1);
        strcpy(head->name, t1);
        head->next = arglist;
        head->kind = TEMP;
        return icl;
    }
}

intr_code_link* translate_stmt(Node *stmt, hash_map *hm){
    //Stmt: Exp SEMI
    //|Compst
    //|RETURN Exp SEMI
    //|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
    //|IF LP Exp RP Stmt ELSE Stmt
    //|WHILE LP Exp RP Stmt
    Node *n0 = get_n_child(stmt, 0);
    if(strcmp(n0->name, "Exp") == 0){
        return translate_exp(n0, hm, temp_char_array(temp_count++));
    }
    else if(strcmp(n0->name, "Compst") == 0){
        return translate_compst(n0, hm);
    }
    else if(strcmp(n0->name, "RETURN") == 0){
        Node *n1 = get_n_child(stmt, 1);
        char *t = temp_char_array(temp_count++);
        intr_code_link *icl1 = translate_exp(n1, hm, t);
        intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
        icl2->next = nullptr;
        intr_code *ic = (intr_code *)malloc(sizeof(intr_code));
        icl2->code = ic;
        ic->kind = RETURN_i;
        operand o1;
        o1.kind = TEMP;
        strcpy(o1.name, t);
        ic->u.one.op = o1;
        link_icl(icl1, icl2);
        return icl1;
    }
    else if(strcmp(n0->name, "IF") == 0){
        Node *n2 = get_n_child(stmt, 2);
        Node *n4 = get_n_child(stmt, 4);
        char* label1 = label_char_array(label_count++);
        char* label2 = label_char_array(label_count++);
        intr_code_link *icl1 = translate_cond(n2, label1, label2, hm);
        icl1->next->next->code->u.relop.is_if = 1;
        intr_code_link *icl2 = translate_stmt(n4, hm);
        intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code_link *icl4 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
        intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
        icl3->code = ic1;
        icl3->next = nullptr;
        ic1->kind = LABEL_i;
        ic1->u.label.label = label1;
        icl4->code = ic2;
        icl4->next = nullptr;
        ic2->kind = LABEL_i;
        ic2->u.label.label = label2;
        Node *n6 = get_n_child(stmt, 6);
        if(n6){
            char* label3 = label_char_array(label_count++);
            intr_code_link *icl5 = translate_stmt(n6, hm);
            intr_code_link *icl6 = (intr_code_link *)malloc(sizeof(intr_code_link));
            intr_code_link *icl7 = (intr_code_link *)malloc(sizeof(intr_code_link));
            icl6->next = nullptr;
            intr_code *ic3 = (intr_code *)malloc(sizeof(intr_code));
            icl6->code = ic3;
            ic3->kind = GOTO_i;
            ic3->u.goto_.label_false = label3;
            intr_code *ic4 = (intr_code *)malloc(sizeof(intr_code));
            icl7->code = ic4;
            icl7->next = nullptr;
            ic4->kind = LABEL_i;
            ic4->u.label.label = label3;
            link_icl(icl1, icl3);
            link_icl(icl3, icl2);
            link_icl(icl2, icl6);
            link_icl(icl6, icl4);
            link_icl(icl4, icl5);
            link_icl(icl5, icl7);
        }
        else{
            link_icl(icl1, icl3);
            link_icl(icl3, icl2);
            link_icl(icl2, icl4);
        }
        return icl1;
    }
    else if(strcmp(n0->name, "WHILE") == 0){
        Node *n2 = get_n_child(stmt, 2);
        Node *n4 = get_n_child(stmt, 4);
        char* label1 = label_char_array(label_count++);
        char* label2 = label_char_array(label_count++);
        char* label3 = label_char_array(label_count++);
        intr_code_link *icl1 = translate_cond(n2, label2, label3, hm);
        intr_code_link *icl2 = translate_stmt(n4, hm);
        intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
        icl3->code = ic1;
        icl3->next = nullptr;
        ic1->kind = LABEL_i;
        ic1->u.label.label = label1;
        ic1->u.label.is_while = 1;
        intr_code_link *icl4 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
        icl4->code = ic2;
        icl4->next = nullptr;
        ic2->kind = LABEL_i;
        ic2->u.label.label = label2;
        intr_code_link *icl5 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic3 = (intr_code *)malloc(sizeof(intr_code));
        icl5->next = nullptr;
        icl5->code = ic3;
        ic3->kind = GOTO_i;
        ic3->u.goto_.label_false = label1;
        intr_code_link *icl6 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic4 = (intr_code *)malloc(sizeof(intr_code));
        icl6->code = ic4;
        icl6->next = nullptr;
        ic4->kind = LABEL_i;
        ic4->u.label.label = label3;
        link_icl(icl3, icl1);
        link_icl(icl1, icl4);
        link_icl(icl4, icl2);
        link_icl(icl2, icl5);
        link_icl(icl5, icl6);
        return icl3;
    }
    else{
        printf("wrong expression in translate_Stmt\n");
    }
}

intr_code_link* translate_compst(Node *compst, hash_map *hm){
    //Compst: LC DefList StmtList RC
    Node *n1 = get_n_child(compst, 1);
    Node *n2 = get_n_child(compst, 2);
    if(strcmp(n1->name, "DefList") == 0){
        intr_code_link *icl1 = translate_deflist(n1, hm);
        if(strcmp(n2->name, "StmtList") == 0){
            intr_code_link *icl2 = translate_stmtlist(n2, hm);
            if(icl1){
                link_icl(icl1, icl2);
            }
            else{
                icl1 = icl2;
            }
        }
        return icl1;
    }
    if(strcmp(n1->name, "StmtList") == 0){
        return translate_stmtlist(n1, hm);
    }
    return nullptr;
}

intr_code_link* translate_stmtlist(Node* stmtlist, hash_map *hm){
    Node *n0 = get_n_child(stmtlist, 0);
    if(n0){
        Node *n1 = get_n_child(stmtlist, 1);
        intr_code_link *icl1 = translate_stmt(n0, hm);
        intr_code_link *icl2 = translate_stmtlist(n1, hm);
        if(icl2){
            if(icl1){
                link_icl(icl1, icl2);
            }
            else{
                icl1 = icl2;
            }
        }
        return icl1;
    }
    return nullptr;
}

intr_code_link* translate_deflist(Node *deflist, hash_map *hm){
    Node *n0 = get_n_child(deflist, 0);
    if(n0){
        Node *n1 = get_n_child(deflist, 1);
        intr_code_link* icl2 = translate_deflist(n1, hm);
        intr_code_link* icl1 = translate_def(n0, hm);
        if(icl2 && icl1){
            link_icl(icl1, icl2);
        }
        else if(icl2){
            icl1 = icl2;
        }
        return icl1;
    }
    else{
        return nullptr;
    }
}

intr_code_link* translate_def(Node *def, hash_map *hm){
    Node *n1 = get_n_child(def, 1);
    return translate_declist(n1, hm);
}

intr_code_link* translate_declist(Node *declist, hash_map *hm){
    Node *n0 = get_n_child(declist, 0);
    Node *n2 = get_n_child(declist, 2);
    intr_code_link *icl1 = translate_dec(n0, hm);
    if(n2){
        intr_code_link *icl2 = translate_declist(n2, hm);
        if(icl1 && icl2){
            link_icl(icl1, icl2);
        }
        else if(icl2){
            icl1 = icl2;
        }
    }
    return icl1;
}

intr_code_link* translate_dec(Node *dec, hash_map *hm){
    //Dec: VarDec
    //|VarDec ASSIGNOP Exp
    Node *n0 = get_n_child(dec, 0);
    Node *n2 = get_n_child(dec, 2);
    if(n2){
        char *t1 = temp_char_array(temp_count++);
        intr_code_link *icl1 = translate_exp(n2, hm, t1);
        intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
        link_icl(icl1, icl2);
        intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
        operand o1;
        o1.kind = VARIABLE;
        strcpy(o1.name, n0->first_child->val.char_val);
        operand o2;
        o2.kind = TEMP;
        strcpy(o2.name, t1);
        ic1->kind = ASSIGN_i;
        ic1->u.assign.left = o1;
        ic1->u.assign.right = o2;
        icl2->code = ic1;
        return icl1;
    }
    else{
        return nullptr;
    }
}

intr_code_link* translate_cond(Node *exp, char* label1, char* label2, hash_map *hm){
    Node *n0 = get_n_child(exp, 0);
    Node *n1 = get_n_child(exp, 1);
    Node *n2 = get_n_child(exp, 2);
    if(n1 && strcmp(n1->name, "RELOP") == 0){
        char *t1 = temp_char_array(temp_count++);
        char *t2 = temp_char_array(temp_count++);
        intr_code_link *icl1 = translate_exp(n0, hm, t1);
        intr_code_link *icl2 = translate_exp(n2, hm, t2);
        char *operation = n1->val.char_val;
        intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
        icl3->next = nullptr;
        intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
        icl3->code = ic1;
        ic1->kind = RELOP_i;
        ic1->u.relop.relop = operation;
        ic1->u.relop.is_if = 0;
        operand op1;
        op1.kind = TEMP;
        strcpy(op1.name, t1);
        operand op2;
        op2.kind = TEMP;
        strcpy(op2.name, t2);
        ic1->u.relop.op1 = op1;
        ic1->u.relop.op2 = op2;
        ic1->u.relop.label_true = label1;
        intr_code_link *icl4 = (intr_code_link *)malloc(sizeof(intr_code_link));
        icl4->next = nullptr;
        intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
        icl4->code = ic2;
        ic2->kind = GOTO_i;
        ic2->u.goto_.label_false = label2;
        link_icl(icl1, icl2);
        link_icl(icl2, icl3);
        link_icl(icl3, icl4);
        return icl1;
    }
    else if(n0 && strcmp(n0->name, "NOT") == 0){
        return translate_cond(n1, label2, label1, hm);
    }
    else if(n1 && strcmp(n1->name, "AND") == 0){
        char *label3 = label_char_array(label_count++);
        intr_code_link *icl1 = translate_cond(n0, label3, label2, hm);
        intr_code_link *icl2 = translate_cond(n2, label1, label2, hm);
        intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
        icl3->code = ic1;
        icl3->next = nullptr;
        ic1->kind = LABEL_i;
        ic1->u.label.label = label3;
        link_icl(icl1, icl3);
        link_icl(icl3, icl2);
        return icl1;
    }
    else if(n1 && strcmp(n1->name, "OR") == 0){
        char *label3 = label_char_array(label_count++);
        intr_code_link *icl1 = translate_cond(n0, label1, label3, hm);
        intr_code_link *icl2 = translate_cond(n2, label1, label2, hm);
        intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
        icl3->code = ic1;
        icl3->next = nullptr;
        ic1->kind = LABEL_i;
        ic1->u.label.label = label3;
        link_icl(icl1, icl3);
        link_icl(icl3, icl2);
        return icl1;
    }
    else{
        char *t1 = temp_char_array(temp_count++);
        intr_code_link *icl1 = translate_exp(exp, hm, t1);
        intr_code_link *icl2 = (intr_code_link *)malloc(sizeof(intr_code_link));
        intr_code *ic1 = (intr_code *)malloc(sizeof(intr_code));
        icl2->next = nullptr;
        icl2->code = ic1;
        ic1->kind = RELOP_i;
        operand op1;
        op1.kind = TEMP;
        strcpy(op1.name, t1);
        operand op2;
        op2.kind = CONSTANT;
        op2.u.value = 0;
        ic1->u.relop.label_true = label1;
        ic1->u.relop.op1 = op1;
        ic1->u.relop.op2 = op2;
        ic1->u.relop.relop = "!=";
        intr_code_link *icl3 = (intr_code_link *)malloc(sizeof(intr_code_link));
        icl3->next = nullptr;
        intr_code *ic2 = (intr_code *)malloc(sizeof(intr_code));
        icl3->code = ic2;
        ic2->kind = GOTO_i;
        ic2->u.goto_.label_false = label2;
        link_icl(icl1, icl2);
        link_icl(icl2, icl3);
        return icl1;
    }
}

void print_intr_code(intr_code_link *head){
    intr_code_link *cur = head->next;
    FILE *f = fopen("out.ir", "w");
    while(cur != nullptr){
        //printf("%d\n", cur->code->kind);
        if(cur->code->kind == ASSIGN_i){
            if(cur->code->u.assign.right.kind == CONSTANT){
                //printf("%s := #%d\n", cur->code->u.assign.left.name, cur->code->u.assign.right.u.value);
                fprintf(f, "%s := #%d\n", cur->code->u.assign.left.name, cur->code->u.assign.right.u.value);
            }
            else{
                //printf("%s := %s\n", cur->code->u.assign.left.name, cur->code->u.assign.right.name);
                fprintf(f, "%s := %s\n", cur->code->u.assign.left.name, cur->code->u.assign.right.name);
            }
        }
        else if(cur->code->kind == ADD_i){
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := #%d + #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := #%d + #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                //printf("%s := #%d + %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
                fprintf(f, "%s := #%d + %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := %s + #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := %s + #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
            }
            else{
                //printf("%s := %s + %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
                fprintf(f, "%s := %s + %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
            }
        }
        else if(cur->code->kind == MUL_i){
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := #%d * #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := #%d * #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                //printf("%s := #%d * %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
                fprintf(f, "%s := #%d * %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := %s * #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := %s * #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
            }
            else{
                //printf("%s := %s * %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
                fprintf(f, "%s := %s * %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
            }
        }
        else if(cur->code->kind == SUB_i){
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := #%d - #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := #%d - #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                //printf("%s := #%d - %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
                fprintf(f, "%s := #%d - %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := %s - #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := %s - #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
            }
            else{
                //printf("%s := %s - %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
                fprintf(f, "%s := %s - %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
            }

        }
        else if(cur->code->kind == DIV_i){
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := #%d / #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := #%d / #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.u.value);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                //printf("%s := #%d / %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
                fprintf(f, "%s := #%d / %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.u.value, cur->code->u.three.op2.name);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                //printf("%s := %s / #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
                fprintf(f, "%s := %s / #%d\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.u.value);
            }
            else{
                //printf("%s := %s / %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
                fprintf(f, "%s := %s / %s\n", cur->code->u.three.result.name ,cur->code->u.three.op1.name, cur->code->u.three.op2.name);
            }
        }
        else if(cur->code->kind == RETURN_i){
            if(cur->code->u.one.op.kind == CONSTANT){
                //printf("RETURN #%d\n", cur->code->u.one.op.u.value);
                fprintf(f, "RETURN #%d\n", cur->code->u.one.op.u.value);
            }
            else{
                //printf("RETURN %s\n", cur->code->u.one.op.name);
                fprintf(f, "RETURN %s\n", cur->code->u.one.op.name);
            }
        }
        else if(cur->code->kind == RELOP_i){
            if(cur->code->u.relop.op1.kind == CONSTANT && cur->code->u.relop.op2.kind  == CONSTANT){
                //printf("IF #%d %s #%d GOTO %s\n", cur->code->u.relop.op1.u.value, cur->code->u.relop.relop, cur->code->u.relop.op2.u.value, cur->code->u.relop.label_true);
                fprintf(f, "IF #%d %s #%d GOTO %s\n", cur->code->u.relop.op1.u.value, cur->code->u.relop.relop, cur->code->u.relop.op2.u.value, cur->code->u.relop.label_true);
            }
            else if(cur->code->u.relop.op1.kind == CONSTANT && cur->code->u.relop.op2.kind  != CONSTANT){
                //printf("IF #%d %s %s GOTO %s\n", cur->code->u.relop.op1.u.value, cur->code->u.relop.relop, cur->code->u.relop.op2.name, cur->code->u.relop.label_true);
                fprintf(f, "IF #%d %s %s GOTO %s\n", cur->code->u.relop.op1.u.value, cur->code->u.relop.relop, cur->code->u.relop.op2.name, cur->code->u.relop.label_true);
            }
            else if(cur->code->u.relop.op1.kind != CONSTANT && cur->code->u.relop.op2.kind  == CONSTANT){
                //printf("IF %s %s #%d GOTO %s\n", cur->code->u.relop.op1.name, cur->code->u.relop.relop, cur->code->u.relop.op2.u.value, cur->code->u.relop.label_true);
                fprintf(f, "IF %s %s #%d GOTO %s\n", cur->code->u.relop.op1.name, cur->code->u.relop.relop, cur->code->u.relop.op2.u.value, cur->code->u.relop.label_true);
            }
            else{
                //printf("IF %s %s %s GOTO %s\n", cur->code->u.relop.op1.name, cur->code->u.relop.relop, cur->code->u.relop.op2.name, cur->code->u.relop.label_true);
                fprintf(f, "IF %s %s %s GOTO %s\n", cur->code->u.relop.op1.name, cur->code->u.relop.relop, cur->code->u.relop.op2.name, cur->code->u.relop.label_true);
            }
        }
        else if(cur->code->kind == GOTO_i){
            //printf("GOTO %s\n", cur->code->u.goto_.label_false);
            fprintf(f, "GOTO %s\n", cur->code->u.goto_.label_false);
        }
        else if(cur->code->kind == LABEL_i){
            //printf("LABEL %s :\n", cur->code->u.label.label);
            fprintf(f, "LABEL %s :\n", cur->code->u.label.label);
        }
        else if(cur->code->kind == CALL_i){
            //printf("%s := CALL %s\n", cur->code->u.two.op1.name, cur->code->u.two.op2.name);
            fprintf(f, "%s := CALL %s\n", cur->code->u.two.op1.name, cur->code->u.two.op2.name);
        }
        else if(cur->code->kind == READ_i){
            //printf("READ %s\n", cur->code->u.one.op.name);
            fprintf(f, "READ %s\n", cur->code->u.one.op.name);
        }
        else if(cur->code->kind == WRITE_i){
            //printf("WRITE %s\n", cur->code->u.one.op.name);
            fprintf(f, "WRITE %s\n", cur->code->u.one.op.name);
        }
        else if(cur->code->kind == ARG_i){
            //printf("ARG %s\n", cur->code->u.one.op.name);
            fprintf(f, "ARG %s\n", cur->code->u.one.op.name);
        }
        else if(cur->code->kind == FUNCTION_i){
            //printf("\nFUNCTION %s :\n", cur->code->u.label.label);
            fprintf(f, "\nFUNCTION %s :\n", cur->code->u.label.label);
        }
        else if(cur->code->kind == PARAM_i){
            //printf("PARAM %s\n", cur->code->u.one.op.name);
            fprintf(f, "PARAM %s\n", cur->code->u.one.op.name);
        }
        else if(cur->code->kind == ADDR_i){
            //printf("%s := &%s\n", cur->code->u.two.op1.name, cur->code->u.two.op2.name);
            fprintf(f, "%s := &%s\n", cur->code->u.two.op1.name, cur->code->u.two.op2.name);
        }
        else if(cur->code->kind == F_STAR_i){
            //printf("%s := *%s\n", cur->code->u.two.op1.name, cur->code->u.two.op2.name);
            fprintf(f, "%s := *%s\n", cur->code->u.two.op1.name, cur->code->u.two.op2.name);
        }
        cur = cur->next;
    }
}