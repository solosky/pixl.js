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

/* Coverage helper header.
   Computing coverage for a generic macro expansing library is not trivial
   as there is no tool to do it normally.

   That's why a special way has been designed:
   *) Put the code to coverage within the macros START_COVERAGE & END_COVERAGE
   (without forgetting to include the header coverage.h before)
   #include "coverage.h"
   START_COVERAGE
   [code to be expanded & to be coveraged]
   END_COVERAGE

   Run the coverage in the makefile like this:
   *) make coverage-test/test-marray.c
   
   View the coverage file test-marray.c.gcov
   It'll contain the macro expanded & annotated.
*/
   
#ifdef WITHIN_COVERAGE
/* For coverage build, do not expand the
   typical macros assert, NULL, etc. */
#undef assert
#undef M_ASSERT
#undef M_ASSERT_SLOW
#undef M_ASSERT_INIT
#undef M_ASSERT_INDEX
#undef M_STATIC_FAILURE
#undef M_STATIC_ASSERT
#undef M_ASSUME
#undef NULL
#undef M_LIKELY
#undef M_UNLIKELY
#undef M_MEMORY_FULL
#undef M_SWAP
#undef M_ARRA4_CONTRACT
#undef M_B1TSET_CONTRACT
#undef M_BPTR33_NODE_CONTRACT
#undef M_BPTR33_NODE_CONTRACT
#undef M_BPTR33_CONTRACT
#undef M_BUFF3R_CONTRACT
#undef M_BUFF3R_PROTECTED_CONTRACT
#undef M_QU3UE_MPMC_CONTRACT
#undef M_QU3UE_MPMC_CONTRACT
#undef M_QU3UE_SPSC_CONTRACT
#undef M_QU3UE_SPSC_CONTRACT
#undef M_C0NCURRENT_CONTRACT
#undef M_D3QU3_CONTRACT
#undef M_D1CT_CONTRACT
#undef M_D1CT_OA_CONTRACT
#undef M_GEN1NT_CONTRACT
#undef M_IL1ST_CONTRACT
#undef M_IL1ST_NODE_CONTRACT
#undef M_L1ST_CONTRACT
#undef M_L1ST_DUAL_PUSH_CONTRACT
#undef M_M3MPOOL_CONTRACT
#undef M_RBTR33_CONTRACT
#undef M_RBTR33_CONTRACT_NODE
#undef M_SHAR3D_CONTRACT
#undef M_SHAR3D_RESOURCE_CONTRACT
#undef M_SNAPSH0T_SPSC_FLAGS_CONTRACT
#undef M_SNAPSH0T_SPSC_CONTRACT
#undef M_SNAPSH0T_SPMC_INT_CONTRACT
#undef M_SNAPSH0T_SPMC_CONTRACT
#undef M_STR1NG_CONTRACT
#undef M_BOUNDED_STR1NG_CONTRACT
#undef M_TUPL3_CONTRACT
#undef M_VAR1ANT_CONTRACT
#undef NDEBUG
#undef atomic_init
#undef atomic_load
#undef atomic_store
#undef atomic_compare_exchange_weak
#undef atomic_compare_exchange_strong
#undef atomic_fetch_add
#undef atomic_fetch_sub
#undef M_CONST_CAST
#undef M_ASSIGN_CAST
#undef M_CHECK_DEFAULT_TYPE
#undef M_CHECK_SAME
#undef M_TR33_NODE_CONTRACT
#undef M_TR33_CONTRACT
#undef M_TR33_IT_CONTRACT
#else
/* For normal builds, empty macros which does nothing */
#define START_COVERAGE
#define END_COVERAGE
#endif
