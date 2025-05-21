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

todo: clean example2 executa 

example2: exam2.c exam2.o main.o
	$(CC) -o example2 *.o  $(LBFLAGS)  -l:libbdt.a

executa:
	@echo " Running example2 "
	./example2
	@echo " Press enter to see data base with BDTlis "
	@read i
	../../install/SBDTlis EXAM2.ibd
clean:	
	rm -f *.o
	rm -f *.a
	rm -f exam2.c
	rm -f exam2.h
	rm -f *.ibd
	rm -f example2
