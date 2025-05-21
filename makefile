all: libreria BDTgen BDTlis BDTini

libreria:
	cd BDT.b && make clean todo
BDTgen:
	cd BDTgen.p && make clean todo
BDTlis:
	cd BDTlis.p && make clean todo
BDTini:
	cd BDTini.p && make clean todo

clean:
	cd BDT.b && make clean
	cd BDTgen.p && make clean
	cd BDTlis.p && make clean
	cd BDTini.p && make clean
	cd install && rm *
