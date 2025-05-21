CFLAGS = -c -g -I./hdrs -I../hdrs -D_LEER_
LFLAGS = -g -I.
CC = gcc -ansi  -Wall -D__USE_GNU -D_GNU_SOURCE
#cc = gcc 
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
ODIR=obj

OBJETOS_LEX=sca.c
OBJETOS_YACC=bdt.c
_OBJETOS=bdt.o sca.o lib.o  main.o 
OBJETOS = $(patsubst %,$(ODIR)/%,$(_OBJETOS))
LBFLAGS=-L../install


$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

.SUFFIXES : .y .c
.y.c:
	$(YACC) $(YFLAGS) $*.c $*.y 
	mv $*.h hdrs

.SUFFIXES : .l .c
.l.c:
	$(LEX) $(LFLAGS) $*.c $*.l

todo: compila install

compila: $(OBJETOS_YACC) $(OBJETOS_LEX) $(OBJETOS)
	$(CC) -o $(ODIR)/BDTgen $(ODIR)/*.o  $(LBFLAGS)  -lbdt -ltoolbdt
	$(CC) -o $(ODIR)/SBDTgen $(ODIR)/*.o  $(LBFLAGS)  -l:libbdt.a -l:libtoolbdt.a

install:
	cp $(ODIR)/BDTgen ../install
	cp $(ODIR)/SBDTgen ../install
	cp *.str ../install

clean:	
	touch *.y
	touch *.l
	rm -f $(ODIR)/*.o
	rm -f $(ODIR)/*.a
