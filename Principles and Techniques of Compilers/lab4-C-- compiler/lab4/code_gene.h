#include "intr_code_gene.h"

typedef struct reg reg;
typedef struct value value;

enum {$zero, $at, $v0, $v1, $a0, $a1, $a2, $a3, $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7,
      $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, $t8, $t9, $k0, $k1, $gp, $sp, $fp, $ra} reg_no;

struct reg{
    int save_value_index;
    int active;
};

struct value{
    char name[32];
    int temp;
    int save_reg_index;
    int active;
    int mem_offset;
};

void code_generate(intr_code_link *head);

int find_reg(char *name, FILE *f);

int reallocate_reg(char *name, int index, FILE *f);

int find_value(char *name);

void end_of_basic_block(FILE *f);

void set_active();