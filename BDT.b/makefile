CFLAGS = -g -I../hdrs -I../../hdrs 
LFLAGS =  -g -I.
CC = gcc -ansi  -g -Wall -D__USE_GNU -D_GNU_SOURCE -fPIC -c 
#CC = /opt/SUNWspro/bin/cc
GCC = gcc
#CC = purify -chain-length=10 -batch -logfile=pure.out -first-only -inuse-at-exit -cache-dir=./tmp gcc 
#cc = gcc 
AR = ar r
LINT= lint
DIR_RESULTADOS=../liberacion
DIR_CABECERAS=../includes
ODIR=./obj

_OBJETOS_TOOL_LIB=dat.o fim.o 
_OBJETOS_LIB=tpc.o mem_din.o \
mdi.o err.o apf.o ppr.o

OBJETOS_TOOL_LIB = $(patsubst %,$(ODIR)/%,$(_OBJETOS_TOOL_LIB))

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

OBJETOS_LIB = $(patsubst %,$(ODIR)/%,$(_OBJETOS_LIB))

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

todo: libbdt libtoolbdt install

libtoolbdt: $(OBJETOS_TOOL_LIB)
	$(AR) $(ODIR)/libtoolbdt.a $(OBJETOS_TOOL_LIB);
	$(GCC) -shared -o $(ODIR)/libtoolbdt.so $(OBJETOS_TOOL_LIB);

libbdt: $(OBJETOS_LIB) 
	$(AR) $(ODIR)/libbdt.a $(OBJETOS_LIB);
	$(GCC) -shared -o $(ODIR)/libbdt.so $(OBJETOS_LIB);



install:	
	cp $(ODIR)/*.a ../install
	cp $(ODIR)/*.so ../install
clean:	
	rm -f $(ODIR)/*.o
	rm -f $(ODIR)/*.a
	rm -f $(ODIR)/*.so
