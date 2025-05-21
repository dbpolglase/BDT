CFLAGS = -c -g -I./hdrs -I../hdrs -D_LEER_
LFLAGS = -g -I.
CC = gcc -ansi  -Wall -D__USE_GNU -D_GNU_SOURCE 
#CC = /opt/SUNWspro/bin/cc
#CC = gcc
#CC = purify -chain-length=10 -batch -logfile=pure.out -first-only -inuse-at-exit -cache-dir=./tmp gcc 
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

_OBJETOS=lst.o  main.o
LBFLAGS=-L../install
OBJETOS = $(patsubst %,$(ODIR)/%,$(_OBJETOS))


$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

todo: compila install

compila: $(OBJETOS) 
	$(CC) -o $(ODIR)/BDTlis $(ODIR)/*.o  $(LBFLAGS)  -lbdt -ltoolbdt
	$(CC) -o $(ODIR)/SBDTlis $(ODIR)/*.o  $(LBFLAGS)  -l:libbdt.a -l:libtoolbdt.a

install:
	cp $(ODIR)/BDTlis ../install
	cp $(ODIR)/SBDTlis ../install

clean:	
	rm -f $(ODIR)/*.o
	rm -f $(ODIR)/*.a
