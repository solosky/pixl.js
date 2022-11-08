How to make a release?
======================

1) Check that all test pass on all primary systems.
   Run 'make check' or 'make checkall' on all primary systems.
   No error shall be reported.
   
2) Check that no undefined behavior, nor data race, ... is reported by sanitizer:
   Run 'make sanitize'  in tests folder on a system.
   This needs at least GCC 7

3) Check that coverage is reasonable.
   Run 'make coverage' and analyze result.

4) Publish the release

In case of problem, open a problem report.


List of automatically tested systems
====================================

* gcc on linux/x86-64 (32 bits and 64 bits)
* g++ on linux/x86-64 (32 bits and 64 bits)
* clang on macos/x86-64
* clang++ on macos/x86-64
* tcc on linux/x86-64
* gcc on windows/x86-64 (mingw)
* msvc on windows/x86-64
* clang on windows/x86-64

List of manually tested systems
===============================

* clang on linux/x86-64
* clang on linux/armv7l
* gcc on linux/powerpc
* gcc on linux/armv7l

Atomic library
==============

For the targets armel, m68k, mips, mipsel, powerpc, powerpcspe or sh4,
you may need to run the test suite by adding the atomic library:

make check LDFLAGS="-pthread -latomic"

since 'atomic' library is not automatically included for 32 bits systems.


Cross compilation
=================

There is no need to cross compile the library as it is a header only library.

However if you want to run the tests with a cross-compiler, do like the following
command. It performs a cross-compilation of the test suite from linux to windows
and run the test suite with wine:

        make CC="i586-mingw32msvc-gcc -std=c99 -DWINVER=0x600 -D_WIN32_WINNT=0x600" LOG_COMPILER=wine check


Handling issues
===============

Issues are opened in the bug tracker of the project (typically https://github.com/P-p-H-d/mlib/issues )

If the issue cannot be taken into account quickly and are long term,
it should be moved into the long term issues included in the repository:
https://github.com/P-p-H-d/mlib/blob/master/doc/ISSUES.org
This is a file in ORG mode: the issues are created with the proper tags.

The original issue in the tracker can then be closed with the issuer agreement.

This will have the following gains:

- no hard dependencies on any bug tracking infrastructure (can be migrated easily). 
- a release will automatically contain all its open problem reports.

Once one issue of ISSUES.org is taken into account,
its state shall be changed to DONE (the issue shall not be removed).


Debugging
=========

Debugging the library can be quite difficult
as it performs heavy preprocessing expansion
and expands all code in one line.
Different strategy for debugging the library itself are still possible:

1) Good old printf.
Add printf of interresting values in the debugged function.


2) Preprocess the library and then compile the preprocessed file like this:

          cc -std=c99 -E test-file.c > test-file.i
          perl -pi -e 's/;/;\n/g' test-file.i
          cc -std=c99 -c -Wall test-file.i

 The resulting executable can be debugged much more easily.


3) To debug the preprocessing itself, the Boot Wave library can be used
and in particular the [wave driver](https://www.boost.org/doc/libs/1_71_0/libs/wave/doc/wave_driver.html)

 Add in the code the following pragmas around the code to debug:

     	  #pragma wave trace(enable)  
     	  ARRAY_DEF (array_charp, char *)
     	  #pragma wave trace(disable)

Run the wave driver:

    	 ./wave ex-array01.c -I .. --c99 -t test.trace

The different expansion performed by the library are described in test.trace

Coding rules
============

* Use of non standard C shall be protected under a specific compiler test,
  so that the library remains compatible with a strict C99 compiler.

* C11 features shall be optional.

* Use 'make format' to format the header files properly.

* External types shall be suffixed by _t and in lower case
  but shall be optional if the user doesn't want then.

* Internal types shall be suffixed by _ct and in lower case
  (as confidential types)

* External / Internal functions shall be in lower case.

* External / Internal macros shall be in upper case.

* Internal macros / functions shall be prefixed by m_<container>
  with container is the name of the container with one letter transformed:
      o --> 0
      e --> 3
      i --> 1
      y --> 4
  This prevents the automatic completion to work when the user starts writing its function.

* Test functions shall be predicated and be suffixed by '_p'


Misc
====

To put a breakpoint for the thread sanitizer in case of error, in gdb add:

$ set env TSAN_OPTIONS=halt_on_error=1,abort_on_error=1
