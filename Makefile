all:
	bison -d syntax.y
	flex lexical2.l
	gcc syntax.tab.c lex.yy.c tmain.c table.c ir.c -lfl -ly -o parser
	./parser test.cmm out.ir
