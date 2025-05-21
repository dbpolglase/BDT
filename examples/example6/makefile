CFLAGS = -c -g -I. -I./../../hdrs -D_LEER_
BDTFLAGS = -I. -I./hdrs -I../hdrs 
LFLAGS = -g -I.
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

todo: clean example6 executa 

example6: exam6.c 

executa:
	@echo " EXAMPLE6 "
	@echo " LOAD RECORDS with BDTini "
	../../install/SBDTini -ffich_to_load *.ibd
	@echo " PRES ANY KEY to SHOW RESOLT with BDTlis "
	read i
	../../install/SBDTlis *.ibd
clean:	
	rm -f *.c
	rm -f *.h
	rm -f *.ibd
