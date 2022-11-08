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
#include <stdio.h>

#include "m-string.h"
#include "m-bitset.h"

static void test1(void)
{
  bitset_t set;

  bitset_init(set);
  assert(bitset_empty_p(set) == true);
  for(int i = 0; i < 100; i ++) {
    bitset_push_back(set, i%2);
    assert (bitset_back(set) == M_ASSIGN_CAST(bool, i%2));
  }
  for(int i = 0; i < 100; i ++) {
    assert (bitset_get(set, (size_t) i) == M_ASSIGN_CAST(bool, i%2));
  }
  assert(bitset_size(set) == 100);
  assert(bitset_empty_p(set) == false);
  assert(bitset_front(set) == false);
  assert(bitset_back(set) == true);
  
  bitset_t set2;
  bitset_init_set(set2, set);
  
  bitset_reset(set);
  assert(bitset_size(set) == 0);
  assert(bitset_empty_p(set) == true);

  assert(bitset_size(set2) == 100);
  assert(bitset_empty_p(set2) == false);
  bitset_resize(set2, 50);
  assert(bitset_size(set2) == 50);
  assert(bitset_empty_p(set2) == false);
  for(int i = 0; i < 50; i ++) {
    assert (bitset_get(set2, (size_t) i) == M_ASSIGN_CAST(bool, i%2));
  }
  bitset_reserve(set2, 100);
  assert(bitset_capacity(set2) >= 100); // rounded
  bitset_reserve(set2, 0);
  assert(bitset_capacity(set2) >= 50 && bitset_capacity(set2) < 100); // rounded
  bitset_resize(set2, 100);
  for(int i = 0; i < 50; i ++) {
    assert (bitset_get(set2, (size_t) i) == M_ASSIGN_CAST(bool, i%2));
  }
  for(int i = 50; i < 100; i ++) {
    assert (bitset_get(set2, (size_t) i) == false);
  }
  bitset_resize(set2, 128);
  for(int i = 0; i < 50; i ++) {
    assert (bitset_get(set2, (size_t) i) == M_ASSIGN_CAST(bool, i%2));
  }
  for(int i = 50; i < 128; i ++) {
    assert (bitset_get(set2, (size_t) i) == false);
  }
  bitset_resize(set2, 64);
  for(int i = 0; i < 50; i ++) {
    assert (bitset_get(set2, (size_t) i) == M_ASSIGN_CAST(bool, i%2));
  }
  for(int i = 50; i < 64; i ++) {
    assert (bitset_get(set2, (size_t) i) == false);
  }
  bitset_resize(set2, 50);

  // Push from 0
  bitset_push_at(set2, 0, 1);
  assert (bitset_get(set2, 0) == 1);
  for(int i = 1; i < 51; i ++) {
    assert (bitset_get(set2, (size_t) i) == M_ASSIGN_CAST(bool, (i-1)%2));
  }
  // Push from not aligned bit
  bitset_push_at(set2, 9, 1);
  for(int i = 1; i < 9; i ++) {
    assert (bitset_get(set2, (size_t) i) == M_ASSIGN_CAST(bool, (i-1)%2));
  }
  assert (bitset_get(set2, 9) == 1);
  for(int i = 10; i < 52; i ++) {
    assert (bitset_get(set2, (size_t) i) == M_ASSIGN_CAST(bool, i%2));
  }

  bool b;
  bitset_pop_back(NULL, set2);
  bitset_pop_back(&b, set2);
  assert(b == false);
  assert(bitset_size(set2) == 50);

  for(int i = 0; i < 200; i++)
    bitset_push_back(set, M_ASSIGN_CAST(bool, i%2));
  bitset_pop_at(&b, set, 0);
  assert (b == false);
  assert (bitset_size(set) == 199);
  bitset_pop_at(&b, set, 0);
  assert (b == true);
  assert (bitset_size(set) == 198);
  bitset_pop_at(&b, set, 9);
  assert (b == true);
  assert (bitset_size(set) == 197);
  bitset_pop_at(&b, set, 71);
  assert (b == false);
  assert (bitset_size(set) == 196);

  bitset_parse_str(set, "[1010101010101010101000000000000000000000000000000000000000000000000000001111]", NULL);
  bitset_push_at(set, 23, true);
  bitset_t set3;
  bitset_init(set3);
  bitset_parse_str(set3, "[10101010101010101010000100000000000000000000000000000000000000000000000001111]", NULL);
  assert(bitset_equal_p(set, set3));
  bitset_push_at(set, 25, true);
  const char *sp;
  bitset_parse_str(set3, "[101010101010101010100001010000000000000000000000000000000000000000000000001111]", &sp);
  assert(bitset_equal_p(set, set3));
  assert (*sp == 0);
  bitset_clear(set3);
  
  bitset_reset(set);
  for(int i = 0; i < 200; i++)
    bitset_push_back(set, ((size_t) i*17547854547UL)&1) ;
  bitset_it_t it;
  int n = 0;
  for(bitset_it(it, set); ! bitset_end_p(it); bitset_next(it)) {
    bool v = ((size_t) n*17547854547UL)&1;
    bool c = *bitset_cref(it);
    assert (v == c);
    n++;
  }

  n = 0;
  for M_EACH (item, set, BITSET_OPLIST) {
    bool v = ((size_t) n*17547854547UL)&1;
    bool c = *item;
    assert (v == c);
    n++;
    }

  assert(bitset_size(set2) == 50);
  assert(bitset_size(set) == 200);
  bitset_swap (set, set2);
  assert(bitset_size(set) == 50);
  assert(bitset_size(set2) == 200);
  assert(!bitset_equal_p(set, set2));
  
  bitset_reserve(set, 0);
  assert(bitset_capacity (set) > 0);
  bitset_reset(set);
  bitset_reserve(set, 0);
  assert(bitset_capacity (set) == 0);

  bitset_set_at (set2, 2, true);
  bitset_set_at (set2, 75, false);
  bitset_swap_at (set2, 2, 75);
  assert (bitset_get(set2, 2) == false);
  assert (bitset_get(set2, 75) == true);

  bitset_set_at (set2, 3, false);
  bitset_set_at (set2, 76, true);
  bitset_flip_at (set2, 2);
  bitset_flip_at (set2, 75);
  assert (bitset_get(set2, 2) == true);
  assert (bitset_get(set2, 3) == false);
  assert (bitset_get(set2, 75) == false);
  assert (bitset_get(set2, 76) == true);

  bitset_set(set, set2);
  bitset_set_at(set, 199, !bitset_get(set, 199));
  assert(!bitset_equal_p(set, set2));
  bitset_set_at(set, 99, !bitset_get(set, 99));
  assert(!bitset_equal_p(set, set2));

  bitset_it_last (it, set);
  assert (bitset_last_p(it));
  assert (!bitset_end_p(it));
  bitset_next(it);
  assert (bitset_last_p(it));
  assert (bitset_end_p(it));
  bitset_it_end (it, set);
  assert (bitset_last_p(it));
  assert (bitset_end_p(it));
  bitset_it_t it2;
  bitset_it(it, set);
  bitset_it_set(it2, it);
  assert (!bitset_last_p(it2));
  assert (!bitset_end_p(it2));
  assert (bitset_it_equal_p(it, it2));
  bitset_next(it);
  assert (!bitset_it_equal_p(it, it2));
  bitset_next(it2);
  assert (bitset_it_equal_p(it, it2));
  bitset_previous(it);
  assert (!bitset_it_equal_p(it, it2));
  bitset_previous(it2);
  assert (bitset_it_equal_p(it, it2));
  bitset_previous(it);
  assert (bitset_end_p(it));

  assert (bitset_hash(set) != 0);

  bitset_reset(set);
  bitset_it(it, set);
  assert(bitset_end_p(it));
  assert(bitset_last_p(it));

  bitset_clear(set);
  bitset_clear(set2);
}

static void test2(void)
{
  M_LET(set1, bitset_t) {
    bitset_t set2;
    bitset_push_back(set1, true);
    bitset_init_move(set2, set1);
    bool b;
    bitset_pop_back(&b, set2);
    assert(b);
    bitset_init(set1);
    bitset_move(set1, set2);
  }
}

static void test_str(void)
{
  bitset_t set1, set2;

  bitset_init(set1);
  bitset_init(set2);

  for(int i = 0; i < 200; i++)
    bitset_push_back(set1, ((size_t) i*17547854547UL >> 4)&1) ;
  
  FILE *f = m_core_fopen ("a-mbitset.dat", "wt");
  if (!f) abort();
  bitset_out_str(f, set1);
  fclose (f);

  f = m_core_fopen ("a-mbitset.dat", "rt");
  if (!f) abort();
  bool b = bitset_in_str (set2, f);
  assert (b == true);
  assert (bitset_equal_p (set1, set2));
  fclose(f);

  string_t s;
  string_init (s);
  bitset_get_str(s, set1, false);
  assert (string_equal_str_p (s, "[01010110101101011010100101001010010101101011010110101001010010100101011010110101101010010100101001010110101101011010100101001010010101101011010110101001010010100101011010110101101010010100101001010110]"));

  string_clear(s);
  bitset_clear(set1);
  bitset_clear(set2);
}

static void test_logic(void)
{
  M_LET(s1, s2, BITSET_OPLIST) {
    bitset_set_str(s1, "[1010]");
    bitset_set_str(s2, "[1100]");
    bitset_and (s1, s2);
    bitset_set_str(s2, "[1000]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010]");
    bitset_set_str(s2, "[1100]");
    bitset_or (s1, s2);
    bitset_set_str(s2, "[1110]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010]");
    bitset_set_str(s2, "[1100]");
    bitset_xor (s1, s2);
    bitset_set_str(s2, "[0110]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010]");
    bitset_not (s1);
    bitset_set_str(s2, "[0101]");
    assert(bitset_equal_p(s1, s2));

    // Bigger set
    bitset_set_str(s1, "[1010101010101010101000000000000000000000000000000000000000000000]");
    bitset_set_str(s2, "[11000000000000000000000000000000000000000000000000000000000000000]");
    bitset_and (s1, s2);
    bitset_set_str(s2, "[1000000000000000000000000000000000000000000000000000000000000000]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[101000000000000000000100000000000000000000000000000000000000000000]");
    bitset_set_str(s2, "[11000000000000000000000000000000000000000000000000000000000000000]");
    bitset_or (s1, s2);
    bitset_set_str(s2, "[11100000000000000000010000000000000000000000000000000000000000000]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[10100000000000000000011111111111100000000000000000000000000000000]");
    bitset_set_str(s2, "[110000000000000000011111111111111100000000000000000000000000000000]");
    bitset_xor (s1, s2);
    bitset_set_str(s2, "[01100000000000000001100000000000010000000000000000000000000000000]");
    assert(bitset_equal_p(s1, s2));

    bitset_set_str(s1, "[1010000000000000000000000000000000000000000000000000000000000000001]");
    bitset_not (s1);
    bitset_set_str(s2, "[0101111111111111111111111111111111111111111111111111111111111111110]");
    assert(bitset_equal_p(s1, s2));

  }
}

static void test_let(void)
{
  M_LET(s1, bitset_t) {
    bitset_push_back(s1, true);
  }
}

static void test_clz(void)
{
  M_LET(s1, bitset_t) {
    assert (bitset_clz(s1) == 0);
    bitset_push_back(s1, false);
    assert (bitset_clz(s1) == 1);
    bitset_push_back(s1, false);
    assert (bitset_clz(s1) == 2);
    bitset_set_at(s1, 0, true);
    assert (bitset_clz(s1) == 1);
    bitset_set_at(s1, 1, true);
    assert (bitset_clz(s1) == 0);
    for(int i = 0; i < 62; i++)
      bitset_push_back(s1, false);
    assert (bitset_clz(s1) == 62);
    bitset_set_at(s1, 63, true);
    assert (bitset_clz(s1) == 0);
    bitset_push_back(s1, false);
    assert (bitset_clz(s1) == 1);
    bitset_set_at(s1, 64, true);
    assert (bitset_clz(s1) == 0);
    for(size_t i = 0; i < 620; i++) {
      bitset_push_back(s1, false);
      assert (bitset_clz(s1) == i+1);
    }
    bitset_set_at(s1, 64+620, true);
    assert (bitset_clz(s1) == 0);
    for(size_t i = 1; i < 64+1+620; i++) {
      bitset_set_at(s1, i, false);
    }
    assert (bitset_clz(s1) == 64+620);
    bitset_set_at(s1, 0, false);
    assert (bitset_clz(s1) == 64+620+1);
  }

  M_LET(s1, bitset_t) {
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000]");
    assert(bitset_popcount(s1) == 0);
    bitset_set_str(s1, "[10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000]");
    assert(bitset_popcount(s1) == 1);
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_popcount(s1) == 1);
    bitset_set_str(s1, "[10000011100000110010101010110000011111111111111110000001110001110001110001110000]");
    assert(bitset_popcount(s1) == 40);
  }

  M_LET(s1, bitset_t) {
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000]");
    assert(bitset_ctz(s1) == 116);
    bitset_set_str(s1, "[10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000]");
    assert(bitset_ctz(s1) == 0);
    bitset_set_str(s1, "[01000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000]");
    assert(bitset_ctz(s1) == 1);
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_ctz(s1) == 115);
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010]");
    assert(bitset_ctz(s1) == 114);
    bitset_set_str(s1, "[000001]");
    assert(bitset_ctz(s1) == 5);
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_ctz(s1) == 64);
    bitset_set_str(s1, "[000000000000000000000000000000000000000000000000000000000000000010000]");
    assert(bitset_ctz(s1) == 64);
    bitset_set_str(s1, "[0000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_ctz(s1) == 63);
    bitset_set_str(s1, "[0000000000000000000000000000000000000000000000000000000000000001000]");
    assert(bitset_ctz(s1) == 63);
    bitset_set_str(s1, "[000000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_ctz(s1) == 65);
    bitset_set_str(s1, "[0000000000000000000000000000000000000000000000000000000000000000010000]");
    assert(bitset_ctz(s1) == 65);
    bitset_set_str(s1, "[00001000001110000011001010101011000001111111111111111000000111000111000111000111000]");
    assert(bitset_ctz(s1) == 4);
    bitset_set_str(s1, "[000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_ctz(s1) == 128);
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_ctz(s1) == 127);
    bitset_set_str(s1, "[0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001]");
    assert(bitset_ctz(s1) == 129);
    bitset_set_str(s1, "[0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000100000000000000000001]");
    assert(bitset_ctz(s1) == 128);
    bitset_set_str(s1, "[000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000010000000000000000001000]");
    assert(bitset_ctz(s1) == 127);
    bitset_set_str(s1, "[00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000011000000000000000000000000]");
    assert(bitset_ctz(s1) == 129);
  }
}

// Detect bug on bitset_resize function in case of increase of size.
// the first limb may not be cleared properly in some cases
static void test_resize(void)
{
  M_LET(set, bitset_t) {
    for(size_t i = 1; i < 257; i++) {
      bitset_resize(set, i);
      assert( bitset_get(set, i-1) == false);
      bitset_set_at(set, 0, true);
      bitset_set_at(set, i-1, true);
      bitset_reset(set);
      bitset_resize(set, i);
      assert( bitset_get(set, 0) == false);
      assert( bitset_get(set, i-1) == false);
      bitset_set_at(set, 0, true);
      bitset_set_at(set, i-1, true);
      bitset_resize(set, i-1);
      bitset_resize(set, i);
      assert( bitset_get(set, i-1) == false);
    }
  }
}

int main(void)
{
  test1();
  test2();
  test_str();
  test_logic();
  test_let();
  test_clz();
  test_resize();
  exit(0);
}
