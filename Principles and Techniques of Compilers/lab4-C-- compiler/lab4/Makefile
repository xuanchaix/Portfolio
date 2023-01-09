
parser: lexical.l main.c syntax.y sdt.c sdt.h symbol_table.c symbol_table.h intr_code_gene.c intr_code_gene.h code_gene.c code_gene.h
	flex lexical.l
	bison -d syntax.y
	gcc main.c sdt.c symbol_table.c intr_code_gene.c syntax.tab.c code_gene.c -lfl -ly -lm -o parser

run: 
	./parser ./test/1.cmm
	./parser ./test/2.cmm
	./parser ./test/3.cmm
	./parser ./test/4.cmm
	./parser ./test/5.cmm
	./parser ./test/6.cmm
	./parser ./test/7.cmm
	./parser ./test/8.cmm
	./parser ./test/9.cmm
	./parser ./test/10.cmm
	./parser ./test/11.cmm
	./parser ./test/12.cmm
	./parser ./test/13.cmm
	./parser ./test/14.cmm
	./parser ./test/15.cmm
	./parser ./test/16.cmm
	./parser ./test/17.cmm
	./parser ./test/s1.cmm
	./parser ./test/s2.cmm
	./parser ./test/s3.cmm
	./parser ./test/s4.cmm
	./parser ./test/s5.cmm
	./parser ./test/s6.cmm

clean:
	-rm parser lex.yy.c syntax.tab.c syntax.tab.h
