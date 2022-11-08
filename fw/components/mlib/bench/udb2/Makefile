CC=		gcc
CXX=		g++
CFLAGS=		-g -Wall -O2 -fomit-frame-pointer -march=native -lm -include stddef.h
CXXFLAGS=	$(CFLAGS)
LDFLAGS=	
OBJS=		
LIBS=		

all: run

udb2:
	git clone https://github.com/attractivechaos/udb2.git

build: udb2
	mkdir -p udb2/mlib
	cp test-mlib.c udb2/mlib/test.c
	cp Makefile-mlib udb2/mlib/Makefile
	touch build

all-recur: build
	cd udb2/stb && make CC="gcc -lm"
	cd udb2 && for dir in `ls | egrep -v '^(Makefile|common.c|_)'`; do cd $$dir && make && cd -; done

run: all-recur
	@cd udb2;						\
	echo ;	echo RUNNING BENCH ; echo ;			\
	for i in `find . -name 'run-test'` ; do			\
	    echo ; echo `dirname $$i|sed -e 's+./++g'`;		\
	    $$i ;						\
	done

clean:
	rm -rf udb2 build *~
