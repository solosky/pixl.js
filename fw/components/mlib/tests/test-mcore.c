/*
 * Copyright (c) 2017-2022, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdbool.h>
#include <assert.h>
#include "test-obj.h"

#include "m-core.h"

static void test_cat(void)
{
#define AandB true
#define AandBandC true
  assert (M_C(A,andB));
  assert (M_C3(A,and,B));
  assert (M_C4(A,and,B,andC));

  assert (M_C3_EMPTY(A,and,B));
  assert (M_C3_EMPTY(Aand,,B));
  assert (M_C3_EMPTY(AandB,,));

  assert (M_C5_EMPTY(A,a,n,d,B));
  assert (M_C5_EMPTY(Aand,,,,B));
}

static void test_power2(void)
{
  assert (M_POWEROF2_P(1));
  assert (M_POWEROF2_P(2));
  assert (!M_POWEROF2_P(3));
  assert (M_POWEROF2_P(4));
  assert (!M_POWEROF2_P(5));
  assert (!M_POWEROF2_P(6));
  assert (!M_POWEROF2_P(7));
  assert (M_POWEROF2_P(8));
}

static void test_incdec(void)
{
  assert (M_INC(0) == 1);
  assert (M_INC(1) == 2);
  assert (M_INC(2) == 3);
  assert (M_INC(3) == 4);
  assert (M_INC(4) == 5);
  assert (M_INC(5) == 6);
  assert (M_INC(6) == 7);
  assert (M_INC(7) == 8);
  assert (M_INC(8) == 9);
  assert (M_INC(9) == 10);
  assert (M_INC(10) == 11);
  assert (M_INC(11) == 12);
  assert (M_INC(12) == 13);
  assert (M_INC(13) == 14);
  assert (M_INC(14) == 15);
  assert (M_INC(15) == 16);
  assert (M_INC(16) == 17);
  assert (M_INC(17) == 18);
  assert (M_INC(18) == 19);
  assert (M_INC(19) == 20);
  assert (M_INC(20) == 21);
  assert (M_INC(21) == 22);
  assert (M_INC(22) == 23);
  assert (M_INC(23) == 24);
  assert (M_INC(24) == 25);
  assert (M_INC(25) == 26);
  assert (M_INC(26) == 27);
  assert (M_INC(27) == 28);
  assert (M_INC(28) == 29);

  assert ((0) == M_DEC(1));
  assert ((1) == M_DEC(2));
  assert ((2) == M_DEC(3));
  assert ((3) == M_DEC(4));
  assert ((4) == M_DEC(5));
  assert ((5) == M_DEC(6));
  assert ((6) == M_DEC(7));
  assert ((7) == M_DEC(8));
  assert ((8) == M_DEC(9));
  assert ((9) == M_DEC(10));
  assert ((10) == M_DEC(11));
  assert ((11) == M_DEC(12));
  assert ((12) == M_DEC(13));
  assert ((13) == M_DEC(14));
  assert ((14) == M_DEC(15));
  assert ((15) == M_DEC(16));
  assert ((16) == M_DEC(17));
  assert ((17) == M_DEC(18));
  assert ((18) == M_DEC(19));
  assert ((19) == M_DEC(20));
  assert ((20) == M_DEC(21));
  assert ((21) == M_DEC(22));
  assert ((22) == M_DEC(23));
  assert ((23) == M_DEC(24));
  assert ((24) == M_DEC(25));
  assert ((25) == M_DEC(26));
  assert ((26) == M_DEC(27));
  assert ((27) == M_DEC(28));
  assert ((28) == M_DEC(29));

  assert( M_ADD(2,5) == M_SUB(10,3));
  assert( M_ADD(5,15) == M_SUB(25,5));

  assert(M_EQUAL(2, 2));
  assert(!M_EQUAL(1, 2));
  assert(M_EQUAL(52, 52));

  assert(!M_NOTEQUAL(2, 2));
  assert(M_NOTEQUAL(1, 2));
  assert(!M_NOTEQUAL(52, 52));

  assert(!M_LESS_THAN_P(4, 2));
  assert(!M_LESS_THAN_P(4, 3));
  assert(!M_LESS_THAN_P(4, 4));
  assert(M_LESS_THAN_P(4, 5));
  assert(M_LESS_THAN_P(4, 6));
  assert(M_LESS_THAN_P(4, 7));

  assert(M_GREATER_OR_EQUAL_P(4, 2));
  assert(M_GREATER_OR_EQUAL_P(4, 3));
  assert(M_GREATER_OR_EQUAL_P(4, 4));
  assert(!M_GREATER_OR_EQUAL_P(4, 5));
  assert(!M_GREATER_OR_EQUAL_P(4, 6));
  assert(!M_GREATER_OR_EQUAL_P(4, 7));

  assert(M_GREATER_THAN_P(4, 2));
  assert(M_GREATER_THAN_P(4, 3));
  assert(!M_GREATER_THAN_P(4, 4));
  assert(!M_GREATER_THAN_P(4, 5));
  assert(!M_GREATER_THAN_P(4, 6));
  assert(!M_GREATER_THAN_P(4, 7));

  assert(!M_LESS_OR_EQUAL_P(4, 2));
  assert(!M_LESS_OR_EQUAL_P(4, 3));
  assert(M_LESS_OR_EQUAL_P(4, 4));
  assert(M_LESS_OR_EQUAL_P(4, 5));
  assert(M_LESS_OR_EQUAL_P(4, 6));
  assert(M_LESS_OR_EQUAL_P(4, 7));

  assert(!M_KEYWORD_P(M_UNDERFLOW, M_SUB(1, 1)));
  assert(M_KEYWORD_P(M_UNDERFLOW, M_SUB(1, 2)));
  assert(M_KEYWORD_P(M_UNDERFLOW, M_SUB(1, 6)));
  assert(M_KEYWORD_P(M_UNDERFLOW, M_SUB(1, 5)));
  assert(!M_KEYWORD_P(M_OVERFLOW, M_ADD(M_MAX_NB_ARGUMENT, 1)));
  assert(M_KEYWORD_P(M_OVERFLOW, M_ADD(M_MAX_NB_ARGUMENT, 2)));
  assert(M_KEYWORD_P(M_OVERFLOW, M_ADD(M_MAX_NB_ARGUMENT, 3)));
  // Test if the initial error is not lost in translation
  assert(M_KEYWORD_P(M_UNDERFLOW, M_INC(M_UNDERFLOW)));
  assert(M_KEYWORD_P(M_OVERFLOW, M_DEC(M_OVERFLOW)));

  assert(M_AND(0, 0) == 0);
  assert(M_AND(0, 1) == 0);
  assert(M_AND(1, 0) == 0);
  assert(M_AND(1, 1) == 1);

  assert(M_OR(0, 0) == 0);
  assert(M_OR(0, 1) == 1);
  assert(M_OR(1, 0) == 1);
  assert(M_OR(1, 1) == 1);

  assert(M_AND3(0, 0, 0) == 0);
  assert(M_AND3(0, 0, 1) == 0);
  assert(M_AND3(0, 1, 0) == 0);
  assert(M_AND3(0, 1, 1) == 0);
  assert(M_AND3(1, 0, 0) == 0);
  assert(M_AND3(1, 0, 1) == 0);
  assert(M_AND3(1, 1, 0) == 0);
  assert(M_AND3(1, 1, 1) == 1);

  assert(M_OR3(0, 0, 0) == 0);
  assert(M_OR3(0, 0, 1) == 1);
  assert(M_OR3(0, 1, 0) == 1);
  assert(M_OR3(0, 1, 1) == 1);
  assert(M_OR3(1, 0, 0) == 1);
  assert(M_OR3(1, 0, 1) == 1);
  assert(M_OR3(1, 1, 0) == 1);
  assert(M_OR3(1, 1, 1) == 1);
}

static void test_return(void)
{
  assert (M_RET_ARG1(1, 0));
  assert (M_RET_ARG2(0, 1, 0));
  assert (M_RET_ARG3(0, 0, 1, 0));
  assert (M_RET_ARG27(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0));
  assert (M_GET_AT((0,1,0),1));
  assert (M_GET_AT((1,0,0),0));
  assert (M_GET_AT((0,0,0,1),3));
}

static void test_invert(void)
{
  assert (M_RET_ARG1 (M_REVERSE(0,1)));
  assert (M_RET_ARG1 (M_REVERSE(0,0,1)));
  assert (M_RET_ARG1 (M_REVERSE(0,0,0,1)));
  assert (M_RET_ARG1 (M_REVERSE(0,0,0,0,1)));
}

static void test_min(void)
{
  assert (M_MIN(1,2) == 1);
  assert (M_MIN(1,-2) == -2);
  assert (M_MAX(1,2) == 2);
  assert (M_MAX(1,-2) == 1);
}

static void test_empty(void)
{
  assert (M_EMPTY_P() == true);
  assert (M_EMPTY_P(1) == false);
  assert (M_EMPTY_P(,) == false);
  assert (M_EMPTY_P(()) == false);
  assert (M_EMPTY_P("123") == false);
  assert (M_EMPTY_P(,1,2,3) == false);
  assert (M_EMPTY_P(+2) == false);
  assert (M_EMPTY_P(-2) == false);
  assert (M_EMPTY_P( () ) == false);
#define F0() int
#define F1() (void)
#define F2(x) x
#define F3() 
#define F4() 1,2  
  assert (M_EMPTY_P(F0) == false);
#ifndef M_COMMA_P_WORKAROUND
  // Disable tests on platform with a non working preprocessor
  assert (M_EMPTY_P(F1) == false);
  assert (M_EMPTY_P(F2) == false);
  assert (M_EMPTY_P(F3) == false);
#endif
  assert (M_EMPTY_P(F4) == false);
#define G0 int
#define G1 (void)
#define G2 x
#define G3 
#define G4 1,2  
  assert (M_EMPTY_P(G0) == false);
  assert (M_EMPTY_P(G1) == false);
  assert (M_EMPTY_P(G2) == false);
  assert (M_EMPTY_P(G3) == true);
  assert (M_EMPTY_P(G4) == false);
}

#define f(n) (n)*(n) +
#define f2(d,n) (d)*(n) +
#define f3(d,i,n) (d)*(i)*(n) +

static void test_map(void)
{
  assert (M_MAP (f, 1) 0 == 1);
  assert (M_MAP (f, 1, 2) 0 == 5);
  assert (M_MAP (f, 1, 2, 3) 0 == 14);
  assert (M_MAP (f, M_SEQ(1, 4)) 0 ==  1+4+9+16);

  assert (M_MAP2 (f2, 2, 1) 0 == 2);
  assert (M_MAP2 (f2, 3, 1, 2) 0 == 9);
  assert (M_MAP2 (f2, 4, 1, 2, 3) 0 == 24);

  assert (M_MAP3 (f3, 2, 2) 0 == 4);
  assert (M_MAP3 (f3, 3, 2, 4) 0 == 30);
  assert (M_MAP3 (f3, 4, 2, 4, 6) 0 == 8+32+12*6);

  assert (M_CROSS_MAP(f2, (1), (2)) 0 == 1*2);
  assert (M_CROSS_MAP(f2, (1), (2, 3)) 0 == 1*2+1*3);
  assert (M_CROSS_MAP(f2, (1, 3), (2)) 0 == 1*2+3*2);
  assert (M_CROSS_MAP(f2, (1, 3), (2,4)) 0 == 1*2 + 1*4 + 3*2 + 3*4);
  assert (M_CROSS_MAP(f2, (1, 3, 5), (2,4)) 0 == 1*2 + 1*4 + 3*2 + 3*4 + 5*2 + 5*4);

  assert (M_CROSS_MAP2(f3, 2, (1), (2)) 0 == (1*2) * 2);
  assert (M_CROSS_MAP2(f3, 2, (1), (2, 3)) 0 == (1*2+1*3) * 2);
  assert (M_CROSS_MAP2(f3, 2, (1, 3), (2)) 0 == (1*2+3*2) * 2);
  assert (M_CROSS_MAP2(f3, 2, (1, 3), (2,4)) 0 == (1*2 + 1*4 + 3*2 + 3*4) * 2);
  assert (M_CROSS_MAP2(f3, 2, (1, 3, 5), (2,4)) 0 == (1*2 + 1*4 + 3*2 + 3*4 + 5*2 + 5*4)*2);

}

static void test_let(void)
{
  bool b = false;
  M_LET(z, M_CLASSIC_OPLIST(testobj)) {
    testobj_set_ui(z, 12);
    M_LET((zz,z), M_CLASSIC_OPLIST(testobj)) {
      assert( testobj_cmp_ui(zz, 12) == 0);
    }
    M_LET((zz,(42)), M_OPEXTEND(M_CLASSIC_OPLIST(testobj), INIT_WITH(testobj_init_set_ui))) {
      assert( testobj_cmp_ui(zz, 42) == 0);
    }
    M_LET((zz,z), z2, M_CLASSIC_OPLIST(testobj)) {
      assert( testobj_cmp_ui(zz, 12) == 0);
    }
    M_LET(z3, (zz,(42)), M_OPEXTEND(M_CLASSIC_OPLIST(testobj), INIT_WITH(testobj_init_set_ui))) {
      assert( testobj_cmp_ui(zz, 42) == 0);
    }
    b = true;
    break;
    assert(0);
  }
  assert(b);

  // Test of recursive INIT_WITH within M_LET
#define OPL1 (TYPE(int), INIT_WITH(OPL1_F))
#define OPL1_F(d, x) ((d) = (x))
#define OPL2 (TYPE(int), INIT_WITH(API_1(OPL2_F)), OPLIST(OPL1) )
#define OPL2_F(opl, d, x) M_APPLY_API(M_GET_INIT_WITH M_GET_OPLIST opl, M_GET_OPLIST opl, d, x)
#define OPL3 (TYPE(int), INIT_WITH(API_1(OPL3_F)), OPLIST(OPL2) )
#define OPL3_F(opl, d, x) M_APPLY_API(M_GET_INIT_WITH M_GET_OPLIST opl, M_GET_OPLIST opl, d, x)
#define OPL4 (TYPE(int), INIT_WITH(API_1(OPL4_F)), OPLIST(OPL3) )
#define OPL4_F(opl, d, x) M_APPLY_API(M_GET_INIT_WITH M_GET_OPLIST opl, M_GET_OPLIST opl, d, x)
#define OPL5 (TYPE(int), INIT_WITH(API_1(OPL5_F)), OPLIST(OPL4) )
#define OPL5_F(opl, d, x) M_APPLY_API(M_GET_INIT_WITH M_GET_OPLIST opl, M_GET_OPLIST opl, d, x)
#define OPL6 (TYPE(int), INIT_WITH(API_1(OPL6_F)), OPLIST(OPL5) )
#define OPL6_F(opl, d, x) M_APPLY_API(M_GET_INIT_WITH M_GET_OPLIST opl, M_GET_OPLIST opl, d, x)
  
  M_LET( (x, 4585) , OPL1)
    assert (x == 4585);
  M_LET( (x, 4586) , OPL2)
    assert (x == 4586);
  M_LET( (x, 4587) , OPL3)
    assert (x == 4587);
  M_LET( (x, 4588) , OPL4)
    assert (x == 4588);
  M_LET( (x, 4589) , OPL5)
    assert (x == 4589);
  M_LET( (x, 4590) , OPL6)
    assert (x == 4590);
    
  // Test of M_LET_IF
  int c = 0;
  M_LET_IF( assert(c++ == 0), (assert(c++ == 1), true), assert(c++ == 3)) {
    assert(c++ == 2);
  }
  M_LET_IF( assert(c++ == 4), (assert(c++ == 5), false), assert(0)) {
    assert(0);
  }
  assert(c++ == 6);
  M_LET_IF(int *p = (int*) malloc(sizeof(int)), p!=0, free(p)) {
    assert(p != NULL);
    *p = 4;
  }

  M_LET_IF( assert(c++ == 7), (assert(c++ == 8), true), assert(c++ == 10), assert(0) ) {
    assert(c++ == 9);
  }
  M_LET_IF( assert(c++ == 11), (assert(c++ == 12), false), assert(0), assert(c++ == 13) ) {
    assert(0);
  }
  assert(c++ == 14);

  M_LET_IF( assert(c++ == 15), (assert(c++ == 16), true), assert(c++ == 18) ) {
    assert(c++ == 17);
    break;
    assert(0);
  }
  assert(c++ == 19);

  c = 7;
  int *p = (int*)malloc(sizeof(int));
  assert(p != NULL);
  M_DEFER ( (assert(c++ == 8), free(p)) ) {
    *p = 4;
    assert(c++ == 7);
  }
  assert(c++ == 9);
  M_DEFER ( assert(c++ == 11) ) {
    assert(c++ == 10);
    break;
    assert(0);
  }
  assert(c++ == 12);
}

static void test_va(void)
{
  assert (M_RET_ARG1 (M_VA(a),zz) == 1);
  assert (M_RET_ARG2 (M_VA(17),zz) == 17);
  assert (M_RET_ARG1 (M_VA(a,b,c),zz) == 3);
  assert (M_RET_ARG2 (M_VA(17,42,59),zz) == 17);
}

static void test_if(void)
{
  assert (M_IF(1)(true, false));
  assert (!M_IF(0)(true, false));
  assert (M_IF(x)(true, false));
}

static void test_test(void)
{
  assert(M_COMMA_P(,));
  assert(M_COMMA_P(1,2));
  assert(M_COMMA_P(1,2,aa));
  assert(!M_COMMA_P(1));
  assert(!M_COMMA_P(()));
  assert(!M_COMMA_P("aa"));
  assert(!M_COMMA_P(""));
  assert(!M_COMMA_P((,)));

  assert(M_PARENTHESIS_P(()));
  assert(M_PARENTHESIS_P((aa)));
  assert(M_PARENTHESIS_P((aa,bb)));
  assert(!M_PARENTHESIS_P(a()));
  assert(!M_PARENTHESIS_P(a(ada)));
  assert(!M_PARENTHESIS_P(a,b));
  assert(!M_PARENTHESIS_P(a));
  assert(!M_PARENTHESIS_P());
  assert(!M_PARENTHESIS_P(,));
  assert(!M_PARENTHESIS_P((),));
  assert(!M_PARENTHESIS_P(()x));

  assert(!M_KEYWORD_P(sum, sul));
  assert(M_KEYWORD_P(sum, sum));
  assert(M_KEYWORD_P(sum, add));
  assert(M_KEYWORD_P(product, product));
  assert(M_KEYWORD_P(product, mul));
  assert(!M_KEYWORD_P(product, mul2));
  assert(M_KEYWORD_P(_, _));
  assert(M_KEYWORD_P(_, _() ));

  assert(M_KEYWORD_P(LIST, LIST));
  assert(M_KEYWORD_P(LIST, LIST(1,23,4) ));
  assert(!M_KEYWORD_P(LIST, SEQ(1,23,4) ));

  assert(M_KEYWORD_TO_VA_ARGS(LIST, LIST(16)) == 16);
  assert(M_KEYWORD_TO_VA_ARGS(LIST, LIST(14)) == 14);
  assert(M_GET_AT( ( M_KEYWORD_TO_VA_ARGS(LIST, LIST(11, 22, 33)) ), 0) == 11);
  assert(M_GET_AT( ( M_KEYWORD_TO_VA_ARGS(LIST, LIST(11, 22, 33)) ), 1) == 22);
  assert(M_GET_AT( ( M_KEYWORD_TO_VA_ARGS(LIST, LIST(11, 22, 33)) ), 2) == 33);
  assert(M_KEYWORD_TO_VA_ARGS(_, _(10)) == 10);
}

static void test_list(void)
{
  assert(M_KEEP_ARGS(1, 1, 0, 0, 0, 0));
  assert(M_SKIP_ARGS(4, 0, 0, 0, 0, 1));
  assert(M_MID_ARGS(2, 1, 0, 0, 1, 0));

  const int tab[] = { M_REPLICATE_C(5, 1) };
  assert(sizeof tab == 5 * sizeof (int) );
  for(int i = 0; i < 5; i++) assert(tab[i] == 1);
  const char str[] = M_REPLICATE(10, "N");
  assert(strcmp(str, "NNNNNNNNNN") == 0);

  assert(M_EQUAL(M_REMOVE_PARENTHESIS( 17 ), 17));
  assert(M_EQUAL(M_REMOVE_PARENTHESIS( (17) ), 17));

  assert( M_FILTER(M_EQUAL, 17, 1, 42, 17, 89) );
  assert( M_FILTER_C(M_EQUAL, 17, 1, 42, 17, 89) );
  const int tab2[] = { M_FILTER_C(M_NOTEQUAL, 8, 1, 3, 4, 8, 9, 8, 10) };
  assert(sizeof tab2 == 5 * sizeof (int) );
  const int tabref[] = { 1, 3, 4, 9, 10 };
  assert (sizeof tab2 == sizeof tabref);
  for(int i = 0; i < 5 ; i++) assert(tab2[i] == tabref[i]);

#define TEST_MUL(a) M_PAIR_1 a * M_PAIR_2 a
#define TEST_ADD(a,b) a + b
  int red = M_REDUCE(TEST_MUL, TEST_ADD, M_OPFLAT M_MERGE_ARGLIST( (1, 2, 3), (4, 5, 6) ) );
  assert(red ==  1 * 4 + 2 * 5 + 3 * 6);
  red = M_REDUCE(TEST_MUL, TEST_ADD, M_OPFLAT M_MERGE_ARGLIST( (1), (4) ) );
  assert(red ==  1 * 4);
  red = M_REDUCE(TEST_MUL, TEST_ADD, M_OPFLAT M_MERGE_ARGLIST( (3, 5), (7, 11) ) );
  assert(red ==  3 * 7 + 5 * 11);

  red = M_REDUCE(M_ID, TEST_ADD, M_OPFLAT M_CAT_ARGLIST( , (3)));
  assert(red == 3);
  red = M_REDUCE(M_ID, TEST_ADD, M_OPFLAT M_CAT_ARGLIST((5, 4), ));
  assert(red == 5 + 4);
  red = M_REDUCE(M_ID, TEST_ADD, M_OPFLAT M_CAT_ARGLIST((5, 4), ( 9, 25, 111) ));
  assert(red == 5 + 4 + 9 + 25 + 111);

}

static void test_oplist(void)
{
#define TEST_OPLIST(op, oplist)                  \
  M_C(M_GET_, op) oplist

  assert (TEST_OPLIST(INIT, (INIT(true))));
  assert (TEST_OPLIST(INIT, (INIT(true),CLEAR(false))));
  assert (TEST_OPLIST(INIT, (INIT_SET(false), INIT(true),CLEAR(false))));
  assert (TEST_OPLIST(INIT, (INIT_SET(false), INIT(true))));

  assert (M_TEST_METHOD_P(INIT, (INIT(empty))));
  assert (M_TEST_METHOD_P(INIT, (INIT(empty),CLEAR(empty))));
  assert (M_TEST_METHOD_P(INIT, (INIT_SET(empty), INIT(empty),CLEAR(empty))));
  assert (M_TEST_METHOD_P(INIT, (INIT_SET(empty), INIT(empty))));
  assert (M_TEST_METHOD_P(OPLIST, (INIT_SET(empty), INIT(empty), OPLIST(a(INIT(empty), CLEAR(empty))), CLEAR(empty))));
  // True oplist are not supported for TEST_METHOD_P + OPLIST...
  assert (M_TEST_METHOD_P(OPLIST, (INIT_SET(empty), OPLIST(a(INIT(empty), CLEAR(empty))))));
  assert (M_TEST_METHOD_P(OPLIST, (INIT_SET(empty), M_IF_METHOD(CLEAR, (INIT(empty), CLEAR(empty)))(OPLIST(empty),))));

  assert (!M_TEST_METHOD_P(INIT2, (INIT(empty))));
  assert (!M_TEST_METHOD_P(INIT2, (INIT(empty),CLEAR(empty))));
  assert (!M_TEST_METHOD_P(INIT2, (INIT_SET(empty), INIT(empty),CLEAR(empty))));
  assert (!M_TEST_METHOD_P(INIT2, (INIT_SET(empty), INIT(empty))));

#define CHECK_OPLIST(op, oplist)                \
  M_IF_METHOD(op, oplist) (true, false)

  assert (CHECK_OPLIST (INIT, M_BASIC_OPLIST));
  assert (CHECK_OPLIST (INIT_SET, M_BASIC_OPLIST));
  assert (CHECK_OPLIST (SET, M_BASIC_OPLIST));
  assert (CHECK_OPLIST (CLEAR, M_BASIC_OPLIST));
  assert (!CHECK_OPLIST (CLEAR_TOOT, M_BASIC_OPLIST));

  assert (M_OPLIST_P(()));
  assert (M_OPLIST_P((INIT(init))));
  assert (M_OPLIST_P((INIT(init),CLEAR(clear))));
  assert (M_OPLIST_P((INIT(init),CLEAR(clear),)));
  assert (!M_OPLIST_P());
  assert (!M_OPLIST_P((())));
  assert (!M_OPLIST_P(INIT(init)));
  assert (!M_OPLIST_P((INIT(init)) a));
  assert (!M_OPLIST_P(func(INIT(init)) a));
  assert (!M_OPLIST_P(func(INIT(init),) a));

#define aop (INIT(0), CLEAR(clear))
  assert (M_TEST_DISABLED_METHOD_P(INIT, aop));
  assert (!M_TEST_DISABLED_METHOD_P(INIT2, aop));
  assert (!M_TEST_DISABLED_METHOD_P(CLEAR, aop));
  assert (!M_TEST_DISABLED_METHOD_P(INIT, ()));

#define M_OPL_op() (INIT(1), CLEAR(0))
  assert(M_GET_INIT M_GLOBAL_OPLIST(op));         
  assert(!M_GET_INIT M_GLOBAL_OPLIST(aop));

#define MAKE_OPLIST(op) (OPLIST(M_GLOBAL_OPLIST_OR_DEF(op)()))
#define M_OPL_uint() (INIT(0), CLEAR(1))
#define M_OPL_array_uint() (OPLIST(M_GLOBAL_OPLIST_OR_DEF(uint)()))
#define M_OPL_list_array_uint() MAKE_OPLIST(array_uint)
#define A1     A2(M_OPL_list_array_uint())
#define A2(op) A3(M_GET_OPLIST op)
#define A3(op) A4(M_GET_OPLIST op)
#define A4(op) M_GET_CLEAR op
  assert (A1);

#define A5   (NAME(1))
  assert (M_GET_NAME A5);

#define A6  (NAME(A6), INIT_WITH(API_1(M_INIT_WITH_NVAR)))
#define A6_init_with_1(x) 1
#define A6_init_with_2(x, y) 0
  assert (M_CALL_INIT_WITH (A6, 0));
  assert (!M_CALL_INIT_WITH (A6, 0, 0));
}

static void test_cast(void)
{
  float f1 = 1.0;
  float f2 = M_ASSIGN_CAST(float, f1);
  int   i1 = M_ASSIGN_CAST(int, f2);
  assert (i1 == 1);
  int *p1 = &i1;
  const int *p2 = M_CONST_CAST(int, p1);
  assert (*p2 == 1);
}

static void test_reduce(void)
{
#define id(x,a) a
#define add(a,b) a + b
  assert ( M_REDUCE2(id, add, d, M_SEQ(1, 5)) == 5 * 6 /2);
  assert ( M_REDUCE2(id, add, d, M_SEQ(1, 10)) == 10 * 11 /2);

  assert ( M_REDUCE2(id, add, d, M_SEQ(1, 1)) == 1);
  assert ( M_REDUCE2(id, add, d, M_SEQ(1, 2)) == 1+2);

  assert ( M_REDUCE(M_ID, add, M_SEQ(1, 5))
           + M_REDUCE(M_ID, add, M_SEQ(6, 10))
           == M_REDUCE(M_ID, add, M_SEQ(1, 10)));                    

#define add3(a,b,c) (a + b + c)
#define mul(a,b) (a) * (b)
  int d = 42;
  assert ( M_REDUCE3(add3, mul, d, 20, 4, 5) == 
          (d + 1 + 20) * ( 
            (d + 2 + 4) * (
              (d + 3 + 5)
            )
          )
  );

}

static void test_as_type(void)
{
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
  int i = 42;
  float f = 2.5;
  void *p = (void*) &i;
  struct as_type { int n; } n = { 17 };
  
  assert (M_AS_TYPE(int, i) == 42);
  assert (M_AS_TYPE(float, i) == 0.0);
  assert (M_AS_TYPE(void *, i) == 0);
  assert (M_AS_TYPE(struct as_type, i).n == 0 );

  assert (M_AS_TYPE(int, f) == 0);
  assert (M_AS_TYPE(float, f) == 2.5);
  assert (M_AS_TYPE(void *, f) == 0);
  assert (M_AS_TYPE(struct as_type, f).n == 0 );

  assert (M_AS_TYPE(int, p) == 0);
  assert (M_AS_TYPE(float, p) == 0.0);
  assert (M_AS_TYPE(void *, p) == &i);
  assert (M_AS_TYPE(struct as_type, p).n == 0 );

  assert (M_AS_TYPE(int, n) == 0);
  assert (M_AS_TYPE(float, n) == 0.0);
  assert (M_AS_TYPE(void *, n) == 0);
  assert (M_AS_TYPE(struct as_type, n).n == 17 );
#endif
}

static void test_parse_standard_c_type(void)
{
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
  char c = 0;
  M_PARSE_DEFAULT_TYPE(&c, "C", NULL);
  assert (c == 'C');

  short s = 0;
  M_PARSE_DEFAULT_TYPE(&s, "-2", NULL);
  assert (s == -2);

  int i = 0;
  M_PARSE_DEFAULT_TYPE(&i, "2", NULL);
  assert (i == 2);
  
  long l  = 0;
  M_PARSE_DEFAULT_TYPE(&l, "1742", NULL);
  assert (l == 1742);
  
  long long ll  = 0;
  M_PARSE_DEFAULT_TYPE(&ll, "-1742548676843540", NULL);
  assert (ll == -1742548676843540LL);

  unsigned short us = 0;
  M_PARSE_DEFAULT_TYPE(&us, "3", NULL);
  assert (us == 3);

  unsigned int ui = 0;
  M_PARSE_DEFAULT_TYPE(&ui, "2", NULL);
  assert (ui == 2);
  
  unsigned long ul  = 0;
  M_PARSE_DEFAULT_TYPE(&ul, "1742", NULL);
  assert (ul == 1742);
  
  unsigned long long ull  = 0;
  M_PARSE_DEFAULT_TYPE(&ull, "1742548676843540", NULL);
  assert (ull == 1742548676843540ULL);
  
  float f;
  M_PARSE_DEFAULT_TYPE(&f, "-0.5", NULL);
  assert (f == -0.5);

  double d;
  M_PARSE_DEFAULT_TYPE(&d, "2.5", NULL);
  assert (d == 2.5);
  
  long double ld;
  M_PARSE_DEFAULT_TYPE(&ld, "2.5", NULL);
  assert (ld == 2.5);
#endif
}

typedef int ti[1];
typedef struct { int b,c; } ts;

static void f_ti(ti x)
{
  ti y;
  M_MOVE_A1_DEFAULT(y,x);
  assert(y[0] == 9);
  assert(x[0] == 0);
}

static void f_ts(ts x)
{
  ts y;
  M_MOVE_DEFAULT(y,x);
  assert(y.b == 2);
  assert(y.c == 3);
  assert(x.b == 0);
  assert(x.c == 0);
}

static void f_ts2(ts *x)
{
  ts y;
  M_MOVE_DEFAULT(y,*x);
  assert(y.b == 2);
  assert(y.c == 3);
  assert(x->b == 0);
  assert(x->c == 0);
}

static void test_move_default(void)
{
  int o, p;
  o = 9;
  M_MOVE_DEFAULT(p, o);
  assert(p == 9);
  assert(o == 0);

  double u, k;
  k = -1.0;
  u = 9.5;
  M_MOVE_DEFAULT(k, u);
  assert(k == 9.5);
  assert(u == 0.0); // NOTE: memset(0) sets an IEEE-754 number to 0.0

  int *ptr = &o, *m;
  M_MOVE_DEFAULT(m, ptr);
  assert( ptr == NULL);
  assert( m == &o);
  
  ti x, y;
  x[0] = 9;
  M_MOVE_A1_DEFAULT(y, x);
  assert(y[0] == 9);
  assert(x[0] == 0);
  
  x[0] = 9;
  f_ti(x);
  assert(x[0] == 0);
  
  ts y2, x2 = {  2, 3};
  M_MOVE_DEFAULT(y2, x2);
  assert(y2.b == 2 && y2.c == 3);
  assert(x2.b == 0 && x2.c == 0);

  x2.b = 2; x2.c = 3;
  f_ts(x2);
  assert(x2.b == 2);
  assert(x2.c == 3);

  f_ts2(&x2);
  assert(x2.b == 0);
  assert(x2.c == 0);
}

static void test_builtin(void)
{
  assert(m_core_clz32(0) == 32);
  for(unsigned i = 0; i < 31; i++) {
    assert(m_core_clz32((uint32_t) (1UL<<i)) == 31U - i);
    assert(m_core_clz32((uint32_t) ((1UL<<i) | 1)) == 31U - i);
  }

  assert(m_core_clz64(0) == 64);
  for(unsigned i = 0; i < 63; i++) {
    assert(m_core_clz64((uint64_t) (1ULL<<i)) == 63U - i);
    assert(m_core_clz64((uint64_t) ((1ULL<<i) | 1)) == 63U - i);
  }

  assert(m_core_ctz32(0) == 32);
  for(unsigned i = 0; i < 31; i++) {
    assert(m_core_ctz32((uint32_t) (1UL<<i)) == i);
    assert(m_core_ctz32((uint32_t) ((1UL<<i) | 1UL<<31)) == i);
  }

  assert(m_core_ctz64(0) == 64);
  for(unsigned i = 0; i < 63; i++) {
    assert(m_core_ctz64((uint64_t) (1ULL<<i)) == i);
    assert(m_core_ctz64((uint64_t) ((1ULL<<i) | 1ULL<<63)) == i);
  }

  assert ( m_core_roundpow2(0) == 0);
  for(unsigned i = 0; i < 62; i++) {
    assert ( m_core_roundpow2(1ULL<<i) == 1ULL<<i);
    assert ( m_core_roundpow2((1ULL<<i)+1) == 1ULL<<(i+1) );
  }

  for(unsigned i = 0; i < 3000; i++) {
    assert( m_core_rotl32a(i, 1) == i * 2);
    assert( m_core_rotl32a(i, 2) == i * 4);
    assert( m_core_rotl32a((uint32_t) ((1UL<<31) + i), 1) == i * 2 + 1);
    assert( m_core_rotl32a((uint32_t) ((1UL<<31) + i), 2) == i * 4 + 2);
  }

  for(unsigned i = 0; i < 3000; i++) {
    assert( m_core_rotl64a(i, 1) == i * 2);
    assert( m_core_rotl64a(i, 2) == i * 4);
    assert( m_core_rotl64a((1ULL<<63) + i, 1) == i * 2 + 1);
    assert( m_core_rotl64a((1ULL<<63) + i, 2) == i * 4 + 2);
  }
}

const char str1[] = "A";
const char str2[] = "AB";
const char str3[] = "Hello";

static void test_str_hash(void)
{
  assert (M_CALL_HASH(M_CSTR_OPLIST, str1) != 0);
  assert (M_CALL_HASH(M_CSTR_OPLIST, str2) != 0);
  assert (M_CALL_HASH(M_CSTR_OPLIST, str3) != 0);
}

static void test_M_CSTR(void)
{
  int r;
  r = strcmp(M_CSTR("Len=%d", 17), "Len=17");
  assert (r == 0);
  r = strcmp(M_CSTR("Hello %s %c", "World", '!'), "Hello World !");
  assert (r == 0);

  // Reduce allocation to test truncation
#undef M_USE_CSTR_ALLOC
#define M_USE_CSTR_ALLOC 8
  // Disable GNUC warning as we want to test this behavior.
  // this warning is not supported by CLANG
#if defined (__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
  _Pragma("GCC diagnostic push")
  _Pragma("GCC diagnostic ignored \"-Wformat-truncation\"")
#endif
  r = strcmp(M_CSTR("Hello %s %c", "World", '!'), "Hello W");
#if defined(__GNUC__) && __GNUC__ >= 7 && !defined(__clang__)
  _Pragma("GCC diagnostic pop")
#endif
  assert (r == 0);
}

static void test_properties(void)
{
#define OP_PROP() ( INIT(init), PROPERTIES((INIT(1), IT_TYPE(2), INIT_WITH(priority))) )
  assert( M_GET_PROPERTY (OP_PROP(), INIT ) == 1);
  assert( M_GET_PROPERTY (OP_PROP(), CLEAR ) == 0);
  assert( M_GET_PROPERTY (OP_PROP(), IT_TYPE ) == 2);
  assert( M_KEYWORD_P( M_GET_PROPERTY (OP_PROP(), INIT_WITH ), priority));
}

static int init3(int *p, int a, int b)
{
  *p = b;
  return a;
}

static int cmp1(int a, int b)
{
  return a < b ? -1 : a > b;
}

static int cmp2(int *a, int *b)
{
  return *a < *b ? -1 : *a > *b;
}

#define cmp3(a, b)                              \
  (a < b ? -1 : a > b)

static void test_generic_api(void)
{
  assert( M_HEAD(17, 0) == 17);
  assert( M_TAIL(18, 19) == 19);
  assert( M_HEAD_2(18, 17, 0) == 17);
  assert( M_TAIL_2(18, 17, 8) == 8);

#define GO1 (INIT(API( init3, ARG1, ARGPTR2, 17, 43 )))
#define GO2 (INIT(API( init3, NONE, ARGPTR2, 0, ARG1 )))
#define GO3 (EQUAL(API( cmp2, EQ(0), ARGPTR1, ARGPTR2 )))
#define GO4 (EQUAL(API( cmp1, EQ(0), ARG1, ARG2 )))
#define GO5 (EQUAL(API( cmp3, EQ(0), ARG1, ARG2 )))

  int x = 0, y = 0;
  M_CALL_INIT(GO1, x, y);
  assert(x == 17);
  assert(y == 43);

  x = 89;
  M_CALL_INIT(GO2, x, y);
  assert( y == 89);
  assert( x == 89);

  x = y = 78;
  bool b = M_CALL_EQUAL(GO3, x, y);
  assert(b);
  x = 77;
  b = M_CALL_EQUAL(GO4, x, y);
  assert(!b);

  x = y = 78;
  b = M_CALL_EQUAL(GO5, x, y);
  assert(b);
  x = 77;
  b = M_CALL_EQUAL(GO5, x, y);
  assert(!b);
}

int main(void)
{
  test_cat();
  test_power2();
  test_incdec();
  test_return();
  test_invert();
  test_min();
  test_empty();
  test_list();
  test_map();
  test_let();
  test_va();
  test_if();
  test_test();
  test_oplist();
  test_cast();
  test_reduce();
  test_as_type();
  test_parse_standard_c_type();
  test_move_default();
  test_builtin();
  test_str_hash();
  test_M_CSTR();
  test_properties();
  test_generic_api();
  exit(0);
}
