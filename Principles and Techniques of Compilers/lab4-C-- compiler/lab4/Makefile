
parser: lexical.l main.c syntax.y sdt.c sdt.h symbol_table.c symbol_table.h intr_code_gene.c intr_code_gene.h code_gene.c code_gene.h
	flex lexical.l
	bison -d syntax.y
	gcc main.c sdt.c symbol_table.c intr_code_gene.c syntax.tab.c code_gene.c -lfl -ly -lm -o parser

clean:
	-rm parser lex.yy.c syntax.tab.c syntax.tab.h
