# Copyright (c) 2017-2022, Patrick Pelissier
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# + Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# + Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

CC=cc -std=c11
CXX=c++ -std=c++11
CFLAGS=-O2 -march=native -Wall
XCFLAGS=
CPPFLAGS=-I..
N=4
BENCH_DEF=
RM=rm -rf

.PHONY: all pgo container queue mempool string plain bench-mlib bench-mlib-mempool bench-stl bench-qt bench-glib bench-klib bench-libdynamic bench-sparsepp bench-collectionc bench-tommyds bench-flathashmap bench-emilib bench-hopscotchmap bench-mlib-thread bench-liblfds bench-concurrentqueue bench-boost bench-mempool bench-string bench-plain bench-rigtorp-mpmc-queue bench-cmc

all: container queue mempool string plain

pgo:
	$(MAKE) all XCFLAGS="-fprofile-generate=pgo"
	$(MAKE) all XCFLAGS="-fprofile-use=pgo"

#########################################################################

container: bench-mlib bench-mlib-mempool bench-stl bench-qt bench-glib bench-klib bench-libdynamic bench-cmc bench-sparsepp bench-densehashmap bench-collectionc bench-tommyds bench-flathashmap bench-emilib bench-hopscotchmap bench-uthash bench-qlibc bench-libsrt bench-nedtries bench-rigtorp-hashmap bench-ctl bench-stc bench-pottery
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

queue: bench-mlib-thread bench-liblfds bench-concurrentqueue bench-boost bench-rigtorp-mpmc-queue
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

mempool: bench-mempool
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

string: bench-string
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

plain: bench-plain
	@echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"



#########################################################################


bench-mlib:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -pthread bench-mlib.c common.c -o bench-mlib.exe
	@./bench-mlib.exe 10
	@./bench-mlib.exe 11
	@./bench-mlib.exe 20
	@./bench-mlib.exe 30
	@./bench-mlib.exe 31
	@./bench-mlib.exe 40
	@./bench-mlib.exe 41
	@./bench-mlib.exe 42
	@./bench-mlib.exe 46
	@./bench-mlib.exe 47
	@./bench-mlib.exe 43
	@./bench-mlib.exe 50
	@./bench-mlib.exe 51

bench-mlib-mempool:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c common.c -DUSE_MEMPOOL -pthread -o bench-mlib-mempool.exe
	@./bench-mlib-mempool.exe 10
	@./bench-mlib-mempool.exe 11
	@./bench-mlib-mempool.exe 20
	@./bench-mlib-mempool.exe 30
	@./bench-mlib-mempool.exe 31
	@./bench-mlib-mempool.exe 40
	@./bench-mlib-mempool.exe 41
	@./bench-mlib-mempool.exe 42
	@./bench-mlib-mempool.exe 46
	@./bench-mlib-mempool.exe 47
	@./bench-mlib-mempool.exe 43
	@./bench-mlib-mempool.exe 50
	@./bench-mlib-mempool.exe 51

bench-mlib-thread:
	$(CC) $(CFLAGS) $(CPPFLAGS) bench-mlib.c common.c -DMULTI_THREAD_MEASURE -pthread -o bench-mlib-thread.exe
	@./bench-mlib-thread.exe 60
	@./bench-mlib-thread.exe 61
	@./bench-mlib-thread.exe 62
	@./bench-mlib-thread.exe 63
	@./bench-mlib-thread.exe 64
	@./bench-mlib-thread.exe 65
	@./bench-mlib-thread.exe 66

bench-stl:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) bench-stl.cpp common.c -o bench-stl.exe
	@./bench-stl.exe 10
	@./bench-stl.exe 20
	@./bench-stl.exe 30
	@./bench-stl.exe 40
	@./bench-stl.exe 41
	@./bench-stl.exe 42
	@./bench-stl.exe 46
	@./bench-stl.exe 43
	@./bench-stl.exe 50
	@./bench-stl.exe 51

# QT shall point to the the directory containing the file .pc are stored
bench-qt:
	@if test -n "$${QT}" ; then $(MAKE) bench-qt0 ; else echo "Nothing to be done for QT." ; fi
bench-qt0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -fPIC bench-qt.cpp common.c -o bench-qt.exe `PKG_CONFIG_PATH="$${QT}" pkg-config --libs --cflags Qt5Core`
	@./bench-qt.exe 10
	@./bench-qt.exe 20
	@./bench-qt.exe 30
	@./bench-qt.exe 40
	@./bench-qt.exe 41
	@./bench-qt.exe 42
	@./bench-qt.exe 50

# GLIB shall point to the the directory containing the file .pc are stored
bench-glib:
	@if test -n "$${GLIB}" ; then $(MAKE) bench-glib0 ; else echo "Nothing to be done for GLIB." ; fi
bench-glib0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) bench-glib.c common.c -o bench-glib.exe `PKG_CONFIG_PATH="$${GLIB}" pkg-config --libs --cflags glib-2.0`
	@./bench-glib.exe 10
	@./bench-glib.exe 20
	@./bench-glib.exe 30
	@./bench-glib.exe 40
	@./bench-glib.exe 41
	@./bench-glib.exe 50

# KLIB shall point to the directory where the headers are
bench-klib:
	@if test -n "$${KLIB}" ; then $(MAKE) bench-klib0 ; else echo "Nothing to be done for KLIB." ; fi

bench-klib0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I $${KLIB} bench-klib.c common.c -o bench-klib.exe
	@./bench-klib.exe 10
	@./bench-klib.exe 20
	@./bench-klib.exe 30
	@./bench-klib.exe 40
	@./bench-klib.exe 41
	@./bench-klib.exe 50

# CMC shall point to the directory where the CMC main header is
bench-cmc:
	@if test -n "$${CMC}" ; then $(MAKE) bench-cmc0 ; else echo "Nothing to be done for CMC." ; fi

bench-cmc0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I $${CMC} bench-cmc.c common.c -o bench-cmc.exe
	@./bench-cmc.exe 20
	@./bench-cmc.exe 30
	@./bench-cmc.exe 40

# LIBDYNAMIC shall point to the the source directory where the source is built
bench-libdynamic:
	@if test -n "$${LIBDYNAMIC}" ; then $(MAKE) bench-libdynamic0 ; else echo "Nothing to be done for LIBDYNAMIC." ; fi

bench-libdynamic0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) bench-libdynamic.c common.c -o bench-libdynamic.exe -I$${LIBDYNAMIC}/src $${LIBDYNAMIC}/.libs/libdynamic.a
	@./bench-libdynamic.exe 20
	@./bench-libdynamic.exe 40
	@./bench-libdynamic.exe 41

# SPARSEPP shall point to the directory where the headers are
bench-sparsepp:
	@if test -n "$${SPARSEPP}" ; then $(MAKE) bench-sparsepp0 ; else echo "Nothing to be done for SPARSEPP." ; fi

bench-sparsepp0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${SPARSEPP}"  -I "$${SPARSEPP}/.." bench-sparsepp.cpp common.c -o bench-sparsepp.exe
	@./bench-sparsepp.exe 41
	@./bench-sparsepp.exe 42
	@./bench-sparsepp.exe 43

# DENSEHASHMAP shall point to the directory where the src are
bench-densehashmap:
	@if test -n "$${DENSEHASHMAP}" ; then $(MAKE) bench-densehashmap0 ; else echo "Nothing to be done Google DENSEHASHMAP." ; fi

bench-densehashmap0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${DENSEHASHMAP}"  -I "$${DENSEHASHMAP}/.." bench-densehashmap.cpp common.c -o bench-densehashmap.exe
	@./bench-densehashmap.exe 41
	@./bench-densehashmap.exe 42
	@./bench-densehashmap.exe 43

# COLLECTIONC shall point to the directory where it is installed
bench-collectionc:
	@if test -n "$${COLLECTIONC}" ; then $(MAKE) bench-collectionc0 ; else echo "Nothing to be done for COLLECTIONC." ; fi

bench-collectionc0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${COLLECTIONC}/include/collectc"  bench-collectionc.c common.c "$${COLLECTIONC}/lib/libcollectc.a" -o bench-collectionc.exe
	@./bench-collectionc.exe 10
	@./bench-collectionc.exe 20
	@./bench-collectionc.exe 30
	@./bench-collectionc.exe 40
	@./bench-collectionc.exe 41
	@./bench-collectionc.exe 50

# TOMMYDS shall point to the source directory.
bench-tommyds:
	@if test -n "$${TOMMYDS}" ; then $(MAKE) bench-tommyds0 ; else echo "Nothing to be done for TOMMYDS." ; fi

bench-tommyds0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${TOMMYDS}" bench-tommyds.c $${TOMMYDS}/tommyds/tommyarray.c $${TOMMYDS}/tommyds/tommylist.c $${TOMMYDS}/tommyds/tommytree.c $${TOMMYDS}/tommyds/tommyhashlin.c common.c -o bench-tommyds.exe
	@./bench-tommyds.exe 10
	@./bench-tommyds.exe 20
	@./bench-tommyds.exe 30
	@./bench-tommyds.exe 40
	@./bench-tommyds.exe 41

# FLATHASHMAP shall point to the directory where the headers are
bench-flathashmap:
	@if test -n "$${FLATHASHMAP}" ; then $(MAKE) bench-flathashmap0 ; else echo "Nothing to be done for FLATHASHMAP." ; fi

bench-flathashmap0:
	$(CXX) -std=c++14 $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${FLATHASHMAP}" bench-flathashmap.cpp common.c -o bench-flathashmap.exe
	@./bench-flathashmap.exe 41
	@./bench-flathashmap.exe 42
	@./bench-flathashmap.exe 46
	@./bench-flathashmap.exe 43

# EMILIB shall point to the directory where the headers are
bench-emilib:
	@if test -n "$${EMILIB}" ; then $(MAKE) bench-emilib0 ; else echo "Nothing to be done for EMILIB." ; fi

bench-emilib0:
	$(CXX) -std=c++14 $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${EMILIB}" bench-emilib.cpp common.c -o bench-emilib.exe
	@./bench-emilib.exe 41
	@./bench-emilib.exe 42
	@./bench-emilib.exe 46
	@./bench-emilib.exe 43

# HOPSCOTCHMAP shall point to the directory where the source are
bench-hopscotchmap:
	@if test -n "$${HOPSCOTCHMAP}" ; then $(MAKE) bench-hopscotchmap0 ; else echo "Nothing to be done for HOPSCOTCHMAP." ; fi

bench-hopscotchmap0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${HOPSCOTCHMAP}/include/tsl" bench-hopscotchmap.cpp common.c -o bench-hopscotchmap.exe
	@./bench-hopscotchmap.exe 41
	@./bench-hopscotchmap.exe 42
	@./bench-hopscotchmap.exe 43

# UTHASH shall point to the directory where the source are
bench-uthash:
	@if test -n "$${UTHASH}" ; then $(MAKE) bench-uthash0 ; else echo "Nothing to be done for UTHASH." ; fi

bench-uthash0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${UTHASH}" bench-uthash.c common.c -o bench-uthash.exe
	@./bench-uthash.exe 10
	@./bench-uthash.exe 20
	@./bench-uthash.exe 40
	@./bench-uthash.exe 41
	@./bench-uthash.exe 50

# QLIBC shall point to the directory where the library is installed.
bench-qlibc:
	@if test -n "$${QLIBC}" ; then $(MAKE) bench-qlibc0 ; else echo "Nothing to be done for QLIBC." ; fi

bench-qlibc0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${QLIBC}/include" -L "$${QLIBC}/lib" -lqlibc -pthread bench-qlibc.c common.c -o bench-qlibc.exe
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 10
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 20
	@LD_LIBRARY_PATH="$${QLIBC}/lib:$${LD_LIBRARY_PATH}" ./bench-qlibc.exe 30

# LIBSRT shall point to the directory where it is installed
bench-libsrt:
	@if test -n "$${LIBSRT}" ; then $(MAKE) bench-libsrt0 ; else echo "Nothing to be done for LIBSRT." ; fi

bench-libsrt0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${LIBSRT}/src" bench-libsrt.c common.c -o bench-libsrt.exe $${LIBSRT}/src/.libs/libsrt.a 
	@./bench-libsrt.exe 20
	@./bench-libsrt.exe 50


# LIBLFDS shall point to the directory where the bin/build/inc/obj/src directories are
# The library shall be built.
bench-liblfds:
	@if test -n "$${LIBLFDS}" ; then $(MAKE) bench-liblfds0 ; else echo "Nothing to be done for LIBLFDS." ; fi
bench-liblfds0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${LIBLFDS}/inc" -Wno-unknown-pragmas bench-liblfds.c common.c -DMULTI_THREAD_MEASURE -o bench-liblfds.exe $${LIBLFDS}/bin/liblfds711.a -lpthread
	@LD_LIBRARY_PATH="$${LIBLFDS}/bin:$${LD_LIBRARY_PATH}" ./bench-liblfds.exe 60


# CONCURRENTQUEUE shall point to the directory where the bin/build/inc/obj/src directories are
# The library shall be built.
bench-concurrentqueue:
	@if test -n "$${CONCURRENTQUEUE}" ; then $(MAKE) bench-concurrentqueue0 ; else echo "Nothing to be done for CONCURRENTQUEUE." ; fi
bench-concurrentqueue0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${CONCURRENTQUEUE}/" bench-concurrentqueue.cpp common.c -DMULTI_THREAD_MEASURE  -o bench-concurrentqueue.exe -lpthread
	@./bench-concurrentqueue.exe 60


# XXHASH shall point to the source directory of xxhash
# The library shall be built.
bench-xxhash:
	@if test -n "$${XXHASH}" ; then $(MAKE) bench-xxhash0 ; else echo "Nothing to be done for XXHASH." ; fi
bench-xxhash0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${XXHASH}/" bench-xxhash.c common.c "$${XXHASH}/libxxhash.a" -o bench-xxhash.exe
	@./bench-xxhash.exe 70

# BOOST shall point to the header location.
bench-boost:
	@if test -n "$${BOOST}" ; then $(MAKE) bench-boost0 ; else echo "Nothing to be done for BOOST." ; fi
bench-boost0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${BOOST}" bench-boost.cpp common.c -DMULTI_THREAD_MEASURE -o bench-boost.exe -lpthread
	@./bench-boost.exe 60

# NEDTRIES shall point to the header location.
bench-nedtries:
	@if test -n "$${NEDTRIES}" ; then $(MAKE) bench-nedtries0 ; else echo "Nothing to be done for NEDTRIES." ; fi
bench-nedtries0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${NEDTRIES}" bench-nedtries.c common.c -o bench-nedtries.exe
	@./bench-nedtries.exe 40
	@./bench-nedtries.exe 45

# RIGTORP_HASHMAP shall point to the directory where the headers are
bench-rigtorp-hashmap:
	@if test -n "$${RIGTORP_HASHMAP}" ; then $(MAKE) bench-rigtorp-hashmap0 ; else echo "Nothing to be done for RIGTORP_HASHMAP." ; fi

bench-rigtorp-hashmap0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${RIGTORP_HASHMAP}/include" bench-rigtorp-hashmap.cpp common.c -o bench-rigtorp-hashmap.exe
	@./bench-rigtorp-hashmap.exe 42
	@./bench-rigtorp-hashmap.exe 41
	@./bench-rigtorp-hashmap.exe 43

# RIGTORP_MPMC_QUEUE shall point to the directory where the headers are
bench-rigtorp-mpmc-queue:
	@if test -n "$${RIGTORP_MPMC_QUEUE}" ; then $(MAKE) bench-rigtorp-mpmc-queue0 ; else echo "Nothing to be done for RIGTORP_MPMC_QUEUE." ; fi
bench-rigtorp-mpmc-queue0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${RIGTORP_MPMC_QUEUE}" bench-rigtorp-mpmcqueue.cpp common.c -DMULTI_THREAD_MEASURE -o bench-mpmcqueue.exe -lpthread
	@./bench-mpmcqueue.exe 60

# CTL shall point to the source directory (not where the headers are)
bench-ctl:
	@if test -n "$${CTL}" ; then $(MAKE) bench-ctl0 ; else echo "Nothing to be done for CTL." ; fi

bench-ctl0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I $${CTL} bench-ctl.c common.c -o bench-ctl.exe
	@./bench-ctl.exe 10
	@./bench-ctl.exe 20
	@./bench-ctl.exe 30
	@./bench-ctl.exe 40
	@./bench-ctl.exe 50

# STC shall point to the source directory (not where the headers are)
bench-stc:
	@if test -n "$${STC}" ; then $(MAKE) bench-stc0 ; else echo "Nothing to be done for STC." ; fi

bench-stc0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I $${STC}/include bench-stc.c common.c -o bench-stc.exe
	@./bench-stc.exe 10
	@./bench-stc.exe 20
	@./bench-stc.exe 30
	@./bench-stc.exe 40
	@./bench-stc.exe 41
	@./bench-stc.exe 50

# POTTERY shall point to the source directory (not where the headers are)
bench-pottery:
	@if test -n "$${POTTERY}" ; then $(MAKE) bench-pottery0 ; else echo "Nothing to be done for POTTERY." ; fi

bench-pottery0:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -DPOTTERY_AVAILABLE=1 -I $${POTTERY}/include bench-pottery.c common.c -o bench-pottery.exe
	@./bench-pottery.exe 10
	@./bench-pottery.exe 20
	@./bench-pottery.exe 30
	@./bench-pottery.exe 40
	@./bench-pottery.exe 41
	@./bench-pottery.exe 50

############################################################################

bench-mlib-json: bench-mlib
	@./bench-mlib.exe 110
	@./bench-mlib.exe 111
	@./bench-mlib.exe 112

bench-rapidjson:
	@if test -n "$${RAPIDJSON}" ; then $(MAKE) bench-rapidjson0 ; else echo "Nothing to be done for RAPIDJSON." ; fi
bench-rapidjson0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -I "$${RAPIDJSON}/" bench-rapidjson.cpp common.c -o bench-rapidjson.exe
	@./bench-rapidjson.exe 110
	@./bench-rapidjson.exe 111
	@./bench-rapidjson.exe 112

############################################################################

bench-plain:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) bench-plain-c.c -o bench-plain-c.exe
	@./bench-plain-c.exe

############################################################################

bench-mempool:
	@if test -n "$${BOOST}" ; then if test -n "$${GLIB}" ; then make bench-mempool0 BENCH_DEF="-DUSE_BOOST -DUSE_GLIB `pkg-config --libs --cflags glib-2.0` -lboost_system"  ; else make bench-mempool0 BENCH_DEF="-DUSE_BOOST -lboost_system" ; fi ; else if test -n "$${GLIB}" ; then make bench-mempool0 BENCH_DEF="-DUSE_GLIB `pkg-config --libs --cflags glib-2.0`"  ; else make bench-mempool0 ; fi ; fi
bench-mempool0:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) pool.cc  $(BENCH_DEF) -o bench-mempool.exe
	@./bench-mempool.exe

############################################################################

bench-maxdict:
	$(CC) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) max-dict.c -o bench-max-dict.exe
	@./bench-max-dict.exe

############################################################################

# BSTRLIB shall point to the BSTRLIB sources
# LIBSRT shall point to the LIBSRT sources
bench-string:
	@if test -n "$${BSTRLIB}" ; then $(MAKE) bench-string-step1 BENCH_DEF="-DBENCH_CAN_USE_BSTRLIB -I ${BSTRLIB} $${BSTRLIB}/obj/bstrlib.o $${BSTRLIB}/obj/bstraux.o $${BSTRLIB}/obj/bstrwrap.o" ; else $(MAKE) bench-string-step1 ; fi

bench-string-step1:
	@if test -n "$${LIBSRT}" ; then $(MAKE) bench-string-step2 BENCH_DEF="$(BENCH_DEF) -DBENCH_CAN_USE_LIBSRT -I$${LIBSRT}/src $${LIBSRT}/src/.libs/libsrt.a" ; else $(MAKE) bench-string-step2 BENCH_DEF="$(BENCH_DEF)"; fi

#NOTE: Need to build by hand sds.o since it is not build by default and sds.c is not compatible with C++
bench-string-step2:
	@if test -n "$${SDS}" ; then $(MAKE) bench-string-step4 BENCH_DEF="$(BENCH_DEF) -fpermissive -DBENCH_CAN_USE_SDS -I$${SDS}/ $${SDS}/sds.o" ; else $(MAKE) bench-string-step4 BENCH_DEF="$(BENCH_DEF)"; fi

bench-string-step4:
	@if test -n "$${GLIB}" ; then $(MAKE) bench-string-step5 BENCH_DEF="$(BENCH_DEF) -DBENCH_CAN_USE_GLIB $$(pkg-config --cflags --libs glib-2.0)" ; else $(MAKE) bench-string-step5 BENCH_DEF="$(BENCH_DEF)"; fi

bench-string-step5:
	@if test -n "$${POTTERY}" ; then $(MAKE) bench-string-step6 BENCH_DEF="$(BENCH_DEF) -DBENCH_CAN_USE_POTTERY -I$${POTTERY}/include -I$${POTTERY}/util/ $${POTTERY}/util/pottery/string/string.c" ; else $(MAKE) bench-string-step6 BENCH_DEF="$(BENCH_DEF)"; fi

bench-string-step6:
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -DBENCH_CAN_USE_STL -DBENCH_CAN_USE_MSTARLIB bench-string.cpp common.c $(BENCH_DEF) -o bench-string.exe 
	$(CXX) $(CFLAGS) $(XCFLAGS) $(CPPFLAGS) -DBENCH_CAN_USE_STL -DBENCH_CAN_USE_MSTARLIB bench-string-2.cpp common.c $(BENCH_DEF) -o bench-string-2.exe 
	@./bench-string.exe
	@./bench-string-2.exe  2000000 0
	@./bench-string-2.exe  2000000 1
	@./bench-string-2.exe  2000000 2
	@./bench-string-2.exe  2000000 4
	@./bench-string-2.exe  2000000 5

bench-string-pgo:
	$(MAKE) bench-string XCFLAGS="-fprofile-generate=pgo"
	$(MAKE) bench-string XCFLAGS="-fprofile-use=pgo"

############################################################################

bench-find-sort:
	g++ bench-find-sort.cpp -O2 -march=native -std=c++11 -I.. -g common.c -o bench-find-sort.exe
	./bench-find-sort.exe 10000000

############################################################################

clean:
	$(RM) *.o bench-*.exe
	$(RM) bench-*.s *~ massif.out.*
	$(RM) plot-*.dat
	$(RM) pgo
	$(RM) tmp-serial.*
	cd nucleotide && $(MAKE) clean

############################################################################

massif-do: all
	@valgrind --tool=massif ./bench-mlib.exe $(N)
	@valgrind --tool=massif ./bench-mlib-mempool.exe $(N)
	@valgrind --tool=massif ./bench-stl.exe $(N)
	@valgrind --tool=massif ./bench-qt.exe $(N)
	@valgrind --tool=massif ./bench-glib.exe $(N)
	@test -f ./bench-klib.exe && valgrind --tool=massif ./bench-klib.exe $(N)
	@test -f ./bench-libdynamic.exe && valgrind --tool=massif ./bench-libdynamic.exe $(N)

massif-visu:
	test -f massif.out.* || $(MAKE) massif-do
	for i in massif.out.* ; do ms_print $$i|head -30 ; done
