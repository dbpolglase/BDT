CFLAGS = -c -g -I. -I./../../hdrs -D_LEER_
BDTFLAGS = -I. -I./hdrs -I../hdrs 
LFLAGS = -g -I.
CC = gcc -ansi  -Wall -D__USE_GNU -D_GNU_SOURCE
AR = ar r
LINT= lint
LEX=lex
YACC=yacc
#LFLAGS=-d
LFLAGS=-o
#YFLAGS=-t -y -H
YFLAGS=-H -o 
DIR_RESULTADOS=../liberacion
DIR_CABECERAS=../includes

OBJETOS=main.o 
LBFLAGS=-L../../install


.SUFFIXES : .bdt .c
.bdt.c:
	../../install/SBDTgen -v $(BDTFLAGS) $<

.SUFFIXES : .c .o
.c.o:
	$(CC) $(CFLAGS) $*.c

todo: clean write5 delete5 executa

write5: exam5.c exam5.o write.o
	$(CC) -o write_ex5 exam5.o write.o $(LBFLAGS)  -l:libbdt.a
delete5: exam5.c exam5.o delete.o
	$(CC) -o delete_ex5 exam5.o delete.o  $(LBFLAGS)  -l:libbdt.a

executa:
	clear
	./delete_ex5 &
	./write_ex5 &
clean:	
	rm -f *.o
	rm -f *.a
	rm -f exam*.c
	rm -f exam*.h
	rm -f *.ibd
	rm -f write_ex*
	rm -f delete_ex*
	rm -f errores*
