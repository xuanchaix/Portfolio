#include "code_gene.h"

reg reg_array[32];
value value_array[100000];
char* reg_name[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
      "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
static int now_offset[1000];
int top_now_offset = 0;
int n_value_array = 0;
int arg_num = 0;

int find_value(char *name){
    for(int i = 0; i < n_value_array; i++){
        if(strcmp(name, value_array[i].name) == 0 && value_array[i].active){
            return i;
        }
    }
    if(n_value_array < 100000){
        value_array[n_value_array].active = 1;
        if(name[0] == 't'){
            value_array[n_value_array].temp = 1;
        }
        else{
            value_array[n_value_array].temp = 0;
        }
        strcpy(value_array[n_value_array].name, name);
        n_value_array++;
        return n_value_array - 1;
    }
    else{
        for(int i = 0; i < n_value_array; i++){
            if(value_array[i].active == 0){
                value_array[n_value_array].active = 1;
                if(name[0] == 't'){
                    value_array[n_value_array].temp = 1;
                }
                else{
                    value_array[n_value_array].temp = 0;
                }
                strcpy(value_array[n_value_array].name, name);
                return i;
            }
        }
    }
    return -1;
}

int find_reg(char *name, FILE *f){
    int index = find_value(name);
    for(int i = $a0; i <= $s7; i++){
        //printf("%s %d %d %d\n", name, index, reg_array[i].save_value_index, reg_array[i].active);
        if(index == reg_array[i].save_value_index && reg_array[i].active){
            return i;
        }
    }
    for(int i = $t0; i<= $s7; i++){
        if(reg_array[i].active == 0){
            reg_array[i].active = 1;
            reg_array[i].save_value_index = index;
            value_array[index].save_reg_index = i;
            value_array[index].active = 1;
            if(value_array[index].mem_offset){
                //read
                fprintf(f, "lw %s, %d(%s)\n", reg_name[i], value_array[index].mem_offset, reg_name[$fp]);
            }
            return i;
        }
    }
    return reallocate_reg(name, index, f);
}

int reallocate_reg(char *name, int index, FILE *f){
    int r;
    srand((unsigned)time(NULL));
    r = rand() % 16 + 8;
    //save
    if(value_array[reg_array[r].save_value_index].mem_offset){
        fprintf(f, "sw %s, %d(%s)\n", reg_name[r], value_array[reg_array[r].save_value_index].mem_offset, reg_name[$fp]);
    }
    else{
        fprintf(f, "addi $sp, $sp, -4\n");
        now_offset[top_now_offset] -=4;
        fprintf(f, "sw %s, %d($fp)\n", reg_name[r], now_offset[top_now_offset]);
        value_array[reg_array[r].save_value_index].mem_offset = now_offset[top_now_offset];
    }

    reg_array[r].save_value_index = index;
    value_array[index].save_reg_index = r;
    value_array[index].active = 1;
    if(value_array[index].mem_offset){
        //read
        fprintf(f, "lw %s, %d(%s)\n", reg_name[r], value_array[index].mem_offset, reg_name[$fp]);
    }
    return r;
}

void end_of_basic_block(FILE *f){
    for(int i = $a0; i <= $s7; i++){
        if(reg_array[i].active == 1){
            reg_array[i].active = 0;
            if(value_array[reg_array[i].save_value_index].mem_offset && value_array[reg_array[i].save_value_index].temp != 1){
                fprintf(f, "sw %s, %d(%s)\n", reg_name[i], value_array[reg_array[i].save_value_index].mem_offset, reg_name[$fp]);
            }
            else if(value_array[reg_array[i].save_value_index].temp != 1){
                fprintf(f, "addi $sp, $sp, -4\n");
                now_offset[top_now_offset] -=4;
                fprintf(f, "sw %s, %d($fp)\n", reg_name[i], now_offset[top_now_offset]);
                value_array[reg_array[i].save_value_index].mem_offset = now_offset[top_now_offset];
            }
        }
    }
}

void set_active(){
    for(int i = $a0; i <= $s7; i++){
        if(reg_array[i].active == 1 && value_array[reg_array[i].save_value_index].temp == 1){
            value_array[reg_array[i].save_value_index].active = 0;
        }
        if(reg_array[i].active == 1){
            reg_array[i].active = 0;
        }
    }
}

void code_generate(intr_code_link *head){
    intr_code_link *cur = head->next;
    FILE *f = fopen("out.s", "w");
    fprintf(f, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n\n.text\nread:\nli $v0, 4\nla $a0, _prompt");
    fprintf(f, "\nsyscall\nli $v0, 5\nsyscall\njr $ra\n\nwrite:\nli $v0, 1\nsyscall\nli $v0, 4\nla $a0, _ret\nsyscall\nmove $v0, $0\njr $ra\n\n");
    for(int i = 0; i < 32; i++){
        reg_array[i].active = 0;
    }
    for(int i = 0; i < 100000; i++){
        value_array[i].active = 0;
        value_array[i].mem_offset = 0;
    }
    int index;
    while(cur != nullptr){
        if(cur->code->kind == ASSIGN_i){
            int r1, r2;
            if(cur->code->u.assign.right.kind == CONSTANT){
                r1 = find_reg(cur->code->u.assign.left.name, f);
                fprintf(f, "li %s, %d\n", reg_name[r1], cur->code->u.assign.right.u.value);
            }
            else{
                r1 = find_reg(cur->code->u.assign.left.name, f);
                r2 = find_reg(cur->code->u.assign.right.name, f);
                fprintf(f, "move %s, %s\n", reg_name[r1], reg_name[r2]);
            }
        }
        else if(cur->code->kind == ADD_i){
            int r1, r2, r3;
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = $t8;
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.three.op1.u.value);
                fprintf(f, "addi %s, %s, %d\n", reg_name[r1], reg_name[r2], cur->code->u.three.op2.u.value);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "addi %s, %s, %d\n", reg_name[r1], reg_name[r2], cur->code->u.three.op1.u.value);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                fprintf(f, "addi %s, %s, %d\n", reg_name[r1], reg_name[r2], cur->code->u.three.op2.u.value);
            }
            else{
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                r3 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "add %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
        }
        else if(cur->code->kind == MUL_i){
            int r1, r2, r3;
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = $t8;
                r3 = $t9;
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.three.op1.u.value);
                fprintf(f, "li %s, %d\n", reg_name[r3], cur->code->u.three.op2.u.value);
                fprintf(f, "mul %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = $t8;
                r3 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.three.op1.u.value);
                fprintf(f, "mul %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                r3 = $t8;
                fprintf(f, "li %s, %d\n", reg_name[r3], cur->code->u.three.op2.u.value);
                fprintf(f, "mul %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else{
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                r3 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "mul %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
        }
        else if(cur->code->kind == SUB_i){
            int r1, r2, r3;
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = $t8;
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.three.op1.u.value);
                fprintf(f, "addi %s, %s, %d\n", reg_name[r1], reg_name[r2], -cur->code->u.three.op2.u.value);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = $t8;
                r3 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.three.op1.u.value);
                fprintf(f, "sub %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                r3 = $t8;
                fprintf(f, "li %s, %d\n", reg_name[r3], cur->code->u.three.op2.u.value);
                fprintf(f, "sub %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else{
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                r3 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "sub %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }

        }
        else if(cur->code->kind == DIV_i){
            int r1, r2, r3;
            if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = $t8;
                r3 = $t9;
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.three.op1.u.value);
                fprintf(f, "li %s, %d\n", reg_name[r3], cur->code->u.three.op2.u.value);
                fprintf(f, "div %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else if(cur->code->u.three.op1.kind == CONSTANT && cur->code->u.three.op2.kind != CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = $t8;
                r3 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.three.op1.u.value);
                fprintf(f, "div %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else if(cur->code->u.three.op1.kind != CONSTANT && cur->code->u.three.op2.kind == CONSTANT){
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                r3 = $t8;
                fprintf(f, "li %s, %d\n", reg_name[r3], cur->code->u.three.op2.u.value);
                fprintf(f, "div %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
            else{
                r1 = find_reg(cur->code->u.three.result.name, f);
                r2 = find_reg(cur->code->u.three.op1.name, f);
                r3 = find_reg(cur->code->u.three.op2.name, f);
                fprintf(f, "div %s, %s, %s\n", reg_name[r1], reg_name[r2], reg_name[r3]);
            }
        }
        else if(cur->code->kind == RETURN_i){
            now_offset[top_now_offset] = 0;
            top_now_offset--;
            for(int i = $a0; i <= $a3; i++){
                reg_array[i].active = 0;
            }
            int r1;
            if(cur->code->u.one.op.kind == CONSTANT){
                fprintf(f, "li %s, %d\n", reg_name[$v0], cur->code->u.one.op.u.value);
            }
            else{
                r1 = find_reg(cur->code->u.one.op.name, f);
                fprintf(f, "move %s, %s\n", reg_name[$v0], reg_name[r1]);
            }
            fprintf(f, "move $sp, $fp\n");
            fprintf(f, "lw $fp, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, 4\n");
            fprintf(f, "jr %s\n", reg_name[$ra]);
        }
        else if(cur->code->kind == RELOP_i){
            if(cur->code->u.relop.is_if == 1){
                end_of_basic_block(f);
            }
            int r1, r2;
            if(cur->code->u.relop.op1.kind == CONSTANT && cur->code->u.relop.op2.kind  == CONSTANT){
                r1 = $t8;
                r2 = $t9;
                fprintf(f, "li %s, %d\n", reg_name[r1], cur->code->u.relop.op1.u.value);
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.relop.op2.u.value);
                if(strcmp("==", cur->code->u.relop.relop) == 0){
                    fprintf(f, "beq %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("!=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bne %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bge %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "ble %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bgt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<", cur->code->u.relop.relop) == 0){
                    fprintf(f, "blt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
            }
            else if(cur->code->u.relop.op1.kind == CONSTANT && cur->code->u.relop.op2.kind  != CONSTANT){
                r1 = $t8;
                r2 = find_reg(cur->code->u.relop.op2.name, f);
                fprintf(f, "li %s, %d\n", reg_name[r1], cur->code->u.relop.op1.u.value);
                if(strcmp("==", cur->code->u.relop.relop) == 0){
                    fprintf(f, "beq %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("!=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bne %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bge %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "ble %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bgt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<", cur->code->u.relop.relop) == 0){
                    fprintf(f, "blt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
            }
            else if(cur->code->u.relop.op1.kind != CONSTANT && cur->code->u.relop.op2.kind  == CONSTANT){
                r1 = find_reg(cur->code->u.relop.op1.name, f);
                r2 = $t8;
                fprintf(f, "li %s, %d\n", reg_name[r2], cur->code->u.relop.op2.u.value);
                if(strcmp("==", cur->code->u.relop.relop) == 0){
                    fprintf(f, "beq %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("!=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bne %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bge %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "ble %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bgt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<", cur->code->u.relop.relop) == 0){
                    fprintf(f, "blt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
            }
            else{
                r1 = find_reg(cur->code->u.relop.op1.name, f);
                r2 = find_reg(cur->code->u.relop.op2.name, f);
                if(strcmp("==", cur->code->u.relop.relop) == 0){
                    fprintf(f, "beq %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("!=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bne %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bge %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<=", cur->code->u.relop.relop) == 0){
                    fprintf(f, "ble %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp(">", cur->code->u.relop.relop) == 0){
                    fprintf(f, "bgt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
                else if(strcmp("<", cur->code->u.relop.relop) == 0){
                    fprintf(f, "blt %s, %s, %s\n", reg_name[r1], reg_name[r2], cur->code->u.relop.label_true);
                }
            }
            set_active();
        }
        else if(cur->code->kind == GOTO_i){
            end_of_basic_block(f);
            set_active();
            fprintf(f, "j %s\n", cur->code->u.goto_.label_false);
        }
        else if(cur->code->kind == LABEL_i){
            if(cur->code->u.label.is_while == 1){
                end_of_basic_block(f);
            }
            fprintf(f, "%s:\n", cur->code->u.label.label);
        }
        else if(cur->code->kind == CALL_i){
            end_of_basic_block(f);
            set_active();
            arg_num = 0;

            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw %s, 0($sp)\n", reg_name[$ra]);

            fprintf(f, "jal %s\n", cur->code->u.two.op2.name);
            
            int r1 = find_reg(cur->code->u.two.op1.name, f);
            fprintf(f, "move %s, $v0\n", reg_name[r1]);
            fprintf(f, "lw %s, 0($sp)\n", reg_name[$ra]);
            fprintf(f, "addi $sp, $sp, 4\n");
        }
        else if(cur->code->kind == READ_i){
            end_of_basic_block(f);
            set_active();
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw %s, 0($sp)\n", reg_name[$ra]);

            fprintf(f, "jal read\n");
            int r1 = find_reg(cur->code->u.one.op.name, f);
            fprintf(f, "move %s, $v0\n", reg_name[r1]);
            fprintf(f, "lw %s, 0($sp)\n", reg_name[$ra]);
            fprintf(f, "addi $sp, $sp, 4\n");
        }
        else if(cur->code->kind == WRITE_i){
            int r1 = find_reg(cur->code->u.one.op.name, f);
            fprintf(f, "move $a0, %s\n", reg_name[r1]);
            end_of_basic_block(f);
            set_active();
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw %s, 0($sp)\n", reg_name[$ra]);

            fprintf(f, "jal write\n");
            fprintf(f, "lw %s, 0($sp)\n", reg_name[$ra]);
            fprintf(f, "addi $sp, $sp, 4\n");
        }
        else if(cur->code->kind == ARG_i){
            /*
            for(int i = $a0; i <= $a3; i++){
                if(reg_array[i].active){
                    fprintf(f, "addi $sp, $sp, -4\n");
                    fprintf(f, "sw %s, 0($sp)\n", reg_name[i]);
                    reg_array[i].active = 0;
                }
            }*/
            int ld = 0;
            if(cur->code->u.one.op.kind == CONSTANT){
                fprintf(f, "li %s, %d\n", reg_name[4 + arg_num], cur->code->u.one.op.u.value);
                arg_num++;
            }
            else{
                int r1 = find_reg(cur->code->u.one.op.name, f);
                fprintf(f, "move %s, %s\n", reg_name[4 + arg_num], reg_name[r1]);
                arg_num++;
            }
            
        }
        else if(cur->code->kind == FUNCTION_i){
            top_now_offset++;
            now_offset[top_now_offset] = 0;
            fprintf(f, "\n%s:\n", cur->code->u.label.label);
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw $fp, 0($sp)\n");
            fprintf(f, "move $fp, $sp\n");
        }
        else if(cur->code->kind == PARAM_i){
            int fd = 0;
            for(int i = $a0; i <= $a3; i++){
                if(reg_array[i].active == 0){
                    fd = 1;
                    reg_array[i].active = 1;
                    reg_array[i].save_value_index = find_value(cur->code->u.one.op.name);
                    break;
                }
            }
            if(fd == 0){

            }
        }
        cur = cur->next;
    }
}