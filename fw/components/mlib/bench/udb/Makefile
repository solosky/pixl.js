CC=		gcc
CXX=		g++
CFLAGS=		-g -Wall -O2 -fomit-frame-pointer -march=native -lm -include stddef.h
CXXFLAGS=	$(CFLAGS)
LDFLAGS=	
OBJS=		
LIBS=		-lm

all: run

benchmarks:
	git clone https://github.com/attractivechaos/benchmarks.git

build: benchmarks
	mkdir -p benchmarks/udb/mlib
	cp test-mlib.c benchmarks/udb/mlib/test.c
	cp Makefile-mlib benchmarks/udb/mlib/Makefile
	mkdir -p benchmarks/udb/mlib-oa
	cp test-mlib-oa.c benchmarks/udb/mlib-oa/test.c
	cp Makefile-mlib benchmarks/udb/mlib-oa/Makefile
	touch build

all-recur: build
	cd benchmarks/udb;					\
	wdir=`pwd`; 						\
	list_c=`find . -name 'test.c'`;				\
	list_cc=`find . -name 'test.cc'`;			\
	for t in $$list_c $$list_cc ; do 			\
		subdir=`dirname $$t`;				\
		cd $$subdir; 					\
		$(MAKE) CC="$(CC)" CXX="$(CXX)" DFLAGS="$(DFLAGS)" CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" || exit 1; 		\
		cd $$wdir; 					\
	done;

run: all-recur
	@cd benchmarks/udb;					\
	echo ; echo "LIBRARY | INT TIME | STR TIME"	;	\
	for i in `find . -name 'test'` ; do			\
	    printf "%s|" `dirname $$i|sed -e 's+./++g'`;	\
	    $$i -is | awk '					\
		/\[int data\] # element/ {  nbInt= $$5 }	\
		/\[int data\] CPU time/  {timeInt= $$5 }	\
		/\[str data\] # element/ {  nbStr= $$5 }	\
		/\[str data\] CPU time/  {timeStr= $$5 }	\
		END { 						\
		if (nbInt != 624444) timeInt="BRK" ; 		\
		if (nbStr != 624444) timeStr="BRK" ;		\
		print timeInt,"|" ,timeStr			\
		}' ;						\
	done

clean:
	rm -rf benchmarks build
