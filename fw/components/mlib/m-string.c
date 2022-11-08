#include "m-string.h"

#undef m_string_init_set
#undef m_string_set
#undef m_string_cat
#undef m_string_cmp
#undef m_string_equal_p
#undef m_string_strcoll
#undef m_string_search

bool m_str1ng_stack_p(const m_string_t s){
  // Function can be called when contract is not fulfilled
  return (s->ptr == NULL);
}

void m_str1ng_set_size(m_string_t s, size_t size){
  // Function can be called when contract is not fulfilled
  if (m_str1ng_stack_p(s)) {
    M_ASSERT (size < sizeof (m_str1ng_heap_ct) - 1);
    // The size of the string is stored as the last char of the buffer.
    s->u.stack.buffer[sizeof (m_str1ng_heap_ct) - 1] = (char) size;
  } else
    s->u.heap.size = size;
}

size_t m_string_size(const m_string_t s){
  // Function can be called when contract is not fulfilled
  // Reading both values before calling the '?' operator enables compiler to generate branchless code
  const size_t s_stack = (size_t) s->u.stack.buffer[sizeof (m_str1ng_heap_ct) - 1];
  const size_t s_heap  = s->u.heap.size;
  return m_str1ng_stack_p(s) ?  s_stack : s_heap;
}

size_t m_string_capacity(const m_string_t s){
  // Function can be called when contract is not fulfilled
  // Reading both values before calling the '?' operator enables compiler to generate branchless code
  const size_t c_stack = sizeof (m_str1ng_heap_ct) - 1;
  const size_t c_heap  = s->u.heap.alloc;
  return m_str1ng_stack_p(s) ?  c_stack : c_heap;
}

char* m_str1ng_get_cstr(m_string_t v){
  // Function can be called when contract is not fulfilled
  char *const ptr_stack = &v->u.stack.buffer[0];
  char *const ptr_heap  = v->ptr;
  return m_str1ng_stack_p(v) ?  ptr_stack : ptr_heap;
}

const char* m_string_get_cstr(const m_string_t v){
  // Function cannot be called when contract is not fulfilled
  // but it is called by contract (so no contract check to avoid infinite recursion).
  const char *const ptr_stack = &v->u.stack.buffer[0];
  const char *const ptr_heap  = v->ptr;
  return m_str1ng_stack_p(v) ?  ptr_stack : ptr_heap;
}

void m_string_init(m_string_t s){
  s->ptr = NULL;
  s->u.stack.buffer[0] = 0;
  m_str1ng_set_size(s, 0);
  M_STR1NG_CONTRACT(s);
}

void m_string_clear(m_string_t v){
  M_STR1NG_CONTRACT(v);
  if (!m_str1ng_stack_p(v)) {    
    M_MEMORY_FREE(v->ptr);
    v->ptr   = NULL;
  }
  /* This is not needed but is safer to make
     the string invalid so that it can be detected. */
  v->u.stack.buffer[sizeof (m_str1ng_heap_ct) - 1] = CHAR_MAX;
}

char * m_string_clear_get_cstr(m_string_t v){
  M_STR1NG_CONTRACT(v);
  char *p = v->ptr;
  if (m_str1ng_stack_p(v)) {
    // The string was stack allocated.
    p = v->u.stack.buffer;
    // Need to allocate a heap string to return the copy.
    size_t alloc = m_string_size(v)+1;
    char *ptr = M_MEMORY_REALLOC (char, NULL, alloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      return NULL;
    }
    M_ASSERT(ptr != NULL && p != NULL);
    memcpy(ptr, p, alloc);
    p = ptr;
  }
  v->ptr = NULL;
  v->u.stack.buffer[sizeof (m_str1ng_heap_ct) - 1] = CHAR_MAX;
  return p;
}

void m_string_reset(m_string_t v){
  M_STR1NG_CONTRACT (v);
  m_str1ng_set_size(v, 0);
  m_str1ng_get_cstr(v)[0] = 0;
  M_STR1NG_CONTRACT (v);
}

char m_string_get_char(const m_string_t v, size_t index){
  M_STR1NG_CONTRACT (v);
  M_ASSERT_INDEX(index, m_string_size(v));
  return m_string_get_cstr(v)[index];
}

void m_string_set_char(m_string_t v, size_t index, const char c){
  M_STR1NG_CONTRACT (v);
  M_ASSERT_INDEX(index, m_string_size(v));
  m_str1ng_get_cstr(v)[index] = c;
}

bool m_string_empty_p(const m_string_t v){
  M_STR1NG_CONTRACT (v);
  return m_string_size(v) == 0;
}

char * m_str1ng_fit2size (m_string_t v, size_t size_alloc){
  M_ASSERT_INDEX (0, size_alloc);
  // Note: this function may be called in context where the contract
  // is not fulfilled.
  const size_t old_alloc = m_string_capacity(v);
  // This line enables the compiler to completly remove this function
  // for very short constant strings.
  M_ASSUME(old_alloc >= sizeof (m_str1ng_heap_ct) - 1);
  if (M_UNLIKELY (size_alloc > old_alloc)) {
    // Insufficient current allocation to store the new string
    // Perform an allocation on the heap.
    size_t alloc = size_alloc + size_alloc / 2;
    if (M_UNLIKELY (alloc <= old_alloc)) {
      /* Overflow in alloc computation */
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is currently broken.
      abort();
      return NULL;
    }
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (M_UNLIKELY (ptr == NULL)) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      // NOTE: Return is currently broken.
      abort();
      return NULL;
    }
    // The pointer cannot be the stack buffer of the string.
    // as it is heap allocated
    M_ASSERT(ptr != &v->u.stack.buffer[0]);
    if (m_str1ng_stack_p(v)) {
      // The string was stack allocated.
      /* Copy the stack allocation into the new heap allocation */
      const size_t size = (size_t) v->u.stack.buffer[sizeof (m_str1ng_heap_ct) - 1] + 1U;
      M_ASSERT( size <= alloc);
      M_ASSERT( size <= sizeof (v->u.stack.buffer)-1);
      memcpy(ptr, &v->u.stack.buffer[0], size);
    }
    // The string cannot be stack allocated anymore.
    v->ptr = ptr;
    v->u.heap.alloc = alloc;
    return ptr;
  }
  return m_str1ng_get_cstr(v);
}

void m_string_reserve(m_string_t v, size_t alloc){
  M_STR1NG_CONTRACT (v);
  const size_t size = m_string_size(v);
  /* NOTE: Reserve below needed size, perform a shrink to fit */
  if (size + 1 > alloc) {
    alloc = size+1;
  }
  M_ASSERT (alloc > 0);
  if (alloc < sizeof (m_str1ng_heap_ct)) {
    // Allocation can fit in the stack space
    if (!m_str1ng_stack_p(v)) {
      /* Transform Heap Allocate to Stack Allocate */
      char *ptr = &v->u.stack.buffer[0];
      memcpy(ptr, v->ptr, size+1);
      M_MEMORY_FREE(v->ptr);
      v->ptr = NULL;
      m_str1ng_set_size(v, size);
    } else {
      /* Already a stack based alloc: nothing to do */
    }
  } else {
    // Allocation cannot fit in the stack space
    // Need to allocate in heap space
    // If the string is stack allocated, v->ptr is NULL
    // and it will therefore perform the initial allocation
    char *ptr = M_MEMORY_REALLOC (char, v->ptr, alloc);
    if (M_UNLIKELY (ptr == NULL) ) {
      M_MEMORY_FULL(sizeof (char) * alloc);
      return;
    }
    if (m_str1ng_stack_p(v)) {
      // Copy from stack space to heap space the string
      char *ptr_stack = &v->u.stack.buffer[0];
      memcpy(ptr, ptr_stack, size+1);
      v->u.heap.size = size;
    }
    v->ptr = ptr;
    v->u.heap.alloc = alloc;
  }
  M_STR1NG_CONTRACT (v);
}

void m_string_set_cstr(m_string_t v, const char str[]){
  M_STR1NG_CONTRACT (v);
  M_ASSERT(str != NULL);
  size_t size = strlen(str);
  char *ptr = m_str1ng_fit2size(v, size+1);
  // The memcpy will also copy the final null char of the string
  memcpy(ptr, str, size+1);
  m_str1ng_set_size(v, size);
  M_STR1NG_CONTRACT (v);
}

void m_string_set_cstrn(m_string_t v, const char str[], size_t n){
  M_STR1NG_CONTRACT (v);
  M_ASSERT(str != NULL);
  size_t len  = strlen(str);
  size_t size = M_MIN (len, n);
  char *ptr = m_str1ng_fit2size(v, size+1);
  // The memcpy will not copy the final null char of the string
  memcpy(ptr, str, size);
  ptr[size] = 0;
  m_str1ng_set_size(v, size);
  M_STR1NG_CONTRACT (v);
}

void m_string_set (m_string_t v1, const m_string_t v2){
  M_STR1NG_CONTRACT (v1);
  M_STR1NG_CONTRACT (v2);
  if (M_LIKELY (v1 != v2)) {
    const size_t size = m_string_size(v2);
    char *ptr = m_str1ng_fit2size(v1, size+1);
    memcpy(ptr, m_string_get_cstr(v2), size+1);
    m_str1ng_set_size(v1, size);
  }
  M_STR1NG_CONTRACT (v1);
}

void m_string_set_n(m_string_t v, const m_string_t ref, size_t offset, size_t length){
  M_STR1NG_CONTRACT (v);
  M_STR1NG_CONTRACT (ref);
  M_ASSERT_INDEX (offset, m_string_size(ref) + 1);
  size_t size = M_MIN (m_string_size(ref) - offset, length);
  char *ptr = m_str1ng_fit2size(v, size+1);
  // v may be equal to ref, so a memmove is needed instead of a memcpy
  memmove(ptr, m_string_get_cstr(ref) + offset, size);
  ptr[size] = 0;
  m_str1ng_set_size(v, size);
  M_STR1NG_CONTRACT (v);
}

void m_string_init_set(m_string_t v1, const m_string_t v2){
  m_string_init(v1);
  m_string_set(v1,v2);
}

void m_string_init_set_cstr(m_string_t v1, const char str[]){
  m_string_init(v1);
  m_string_set_cstr(v1, str);
}

void m_string_init_move(m_string_t v1, m_string_t v2){
  M_STR1NG_CONTRACT (v2);
  memcpy(v1, v2, sizeof (m_string_t));
  // Note: nullify v2 to be safer
  v2->ptr   = NULL;
  M_STR1NG_CONTRACT (v1);
}

void m_string_swap(m_string_t v1, m_string_t v2){
  M_STR1NG_CONTRACT (v1);
  M_STR1NG_CONTRACT (v2);
  // Even if it is stack based, we swap the heap representation
  // which alias the stack based
  M_SWAP (size_t, v1->u.heap.size,  v2->u.heap.size);
  M_SWAP (size_t, v1->u.heap.alloc, v2->u.heap.alloc);
  M_SWAP (char *, v1->ptr,   v2->ptr);
  M_STR1NG_CONTRACT (v1);
  M_STR1NG_CONTRACT (v2);
}

void m_string_move(m_string_t v1, m_string_t v2){
  m_string_clear(v1);
  m_string_init_move(v1,v2);
}

void m_string_push_back (m_string_t v, char c){
  M_STR1NG_CONTRACT (v);
  const size_t size = m_string_size(v);
  char *ptr = m_str1ng_fit2size(v, size+2);
  ptr[size+0] = c;
  ptr[size+1] = 0;
  m_str1ng_set_size(v, size+1);
  M_STR1NG_CONTRACT (v);
}

void m_string_cat_cstr(m_string_t v, const char str[]){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (str != NULL);
  const size_t old_size = m_string_size(v);
  const size_t size = strlen(str);
  char *ptr = m_str1ng_fit2size(v, old_size + size + 1);
  memcpy(&ptr[old_size], str, size + 1);
  m_str1ng_set_size(v, old_size + size);
  M_STR1NG_CONTRACT (v);
}

void m_string_cat(m_string_t v, const m_string_t v2){
  M_STR1NG_CONTRACT (v2);
  M_STR1NG_CONTRACT (v);
  const size_t size = m_string_size(v2);
  if (M_LIKELY (size > 0)) {
    const size_t old_size = m_string_size(v);
    char *ptr = m_str1ng_fit2size(v, old_size + size + 1);
    memcpy(&ptr[old_size], m_string_get_cstr(v2), size);
    ptr[old_size + size] = 0;
    m_str1ng_set_size(v, old_size + size);
  }
  M_STR1NG_CONTRACT (v);
}

int m_string_cmp_cstr(const m_string_t v1, const char str[]){
  M_STR1NG_CONTRACT (v1);
  M_ASSERT (str != NULL);
  return strcmp(m_string_get_cstr(v1), str);
}

int m_string_cmp(const m_string_t v1, const m_string_t v2){
  M_STR1NG_CONTRACT (v1);
  M_STR1NG_CONTRACT (v2);
  return strcmp(m_string_get_cstr(v1), m_string_get_cstr(v2));
}

bool m_string_equal_cstr_p(const m_string_t v1, const char str[]){
  M_STR1NG_CONTRACT(v1);
  M_ASSERT (str != NULL);
  return m_string_cmp_cstr(v1, str) == 0;
}

bool m_string_equal_p(const m_string_t v1, const m_string_t v2){
  /* m_string_equal_p can be called with (at most) one string which is an OOR value.
     In case of OOR value, .ptr is NULL and .alloc is the maximum or the maximum-1.
     As it will detect a stack based string, it will read the size from the alloc fied
     as 0xFF or 0xFE. In both cases, the size cannot be equal to a normal string
     so the test m_string_size(v1) == m_string_size(v2) is false in this case.
  */
  M_ASSERT(v1 != NULL);
  M_ASSERT(v2 != NULL);
  /* Optimization: both strings shall have at least the same size */
  return m_string_size(v1) == m_string_size(v2) && m_string_cmp(v1, v2) == 0;
}

int m_string_cmpi_cstr(const m_string_t v1, const char p2[]){
  M_STR1NG_CONTRACT (v1);
  M_ASSERT (p2 != NULL);
  // strcasecmp is POSIX only
  const char *p1 = m_string_get_cstr(v1);
  int c1, c2;
  do {
    // To avoid locale without 1 per 1 mapping.
    c1 = toupper((unsigned char) *p1++);
    c2 = toupper((unsigned char) *p2++);
    c1 = tolower((unsigned char) c1);
    c2 = tolower((unsigned char) c2);
  } while (c1 == c2 && c1 != 0);
  return c1 - c2;
}

int m_string_cmpi(const m_string_t v1, const m_string_t v2){
  return m_string_cmpi_cstr(v1, m_string_get_cstr(v2));
}

size_t m_string_search_char (const m_string_t v, char c, size_t start){
  M_STR1NG_CONTRACT (v);
  M_ASSERT_INDEX (start, m_string_size(v) + 1);
  const char *p = M_ASSIGN_CAST(const char*,
                                strchr(m_string_get_cstr(v)+start, c));
  return p == NULL ? M_STRING_FAILURE : (size_t) (p-m_string_get_cstr(v));
}

size_t m_string_search_rchar (const m_string_t v, char c, size_t start){
  M_STR1NG_CONTRACT (v);
  M_ASSERT_INDEX (start, m_string_size(v)+1);
  // NOTE: Can be implemented in a faster way than the libc function
  // by scanning backward from the bottom of the string (which is
  // possible since we know the size).
  // However, does it worth the effort?
  const char *p = M_ASSIGN_CAST(const char*,
                                strrchr(m_string_get_cstr(v)+start, c));
  return p == NULL ? M_STRING_FAILURE : (size_t) (p-m_string_get_cstr(v));
}

size_t m_string_search_cstr(const m_string_t v, const char str[], size_t start){
  M_STR1NG_CONTRACT (v);
  M_ASSERT_INDEX (start, m_string_size(v)+1);
  M_ASSERT (str != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
                                strstr(m_string_get_cstr(v)+start, str));
  return p == NULL ? M_STRING_FAILURE : (size_t) (p-m_string_get_cstr(v));
}

size_t m_string_search (const m_string_t v1, const m_string_t v2, size_t start){
  M_STR1NG_CONTRACT (v2);
  M_ASSERT_INDEX (start, m_string_size(v1)+1);
  return m_string_search_cstr(v1, m_string_get_cstr(v2), start);
}

size_t m_string_search_pbrk(const m_string_t v1, const char first_of[], size_t start){
  M_STR1NG_CONTRACT (v1);
  M_ASSERT_INDEX (start, m_string_size(v1)+1);
  M_ASSERT (first_of != NULL);
  const char *p = M_ASSIGN_CAST(const char*,
                                strpbrk(m_string_get_cstr(v1)+start, first_of));
  return p == NULL ? M_STRING_FAILURE : (size_t) (p-m_string_get_cstr(v1));
}

int m_string_strcoll_cstr(const m_string_t v, const char str[]){
  M_STR1NG_CONTRACT (v);
  return strcoll(m_string_get_cstr(v), str);
}

int m_string_strcoll (const m_string_t v1, const m_string_t v2){
  M_STR1NG_CONTRACT (v2);
  return m_string_strcoll_cstr(v1, m_string_get_cstr(v2));
}

size_t m_string_spn(const m_string_t v1, const char accept[]){
  M_STR1NG_CONTRACT (v1);
  M_ASSERT (accept != NULL);
  return strspn(m_string_get_cstr(v1), accept);
}

size_t m_string_cspn(const m_string_t v1, const char reject[]){
  M_STR1NG_CONTRACT (v1);
  M_ASSERT (reject != NULL);
  return strcspn(m_string_get_cstr(v1), reject);
}

void m_string_left(m_string_t v, size_t index){
  M_STR1NG_CONTRACT (v);
  const size_t size = m_string_size(v);
  if (index >= size)
    return;
  m_str1ng_get_cstr(v)[index] = 0;
  m_str1ng_set_size(v,index);
  M_STR1NG_CONTRACT (v);
}

void m_string_right(m_string_t v, size_t index){
  M_STR1NG_CONTRACT (v);
  char *ptr = m_str1ng_get_cstr(v);
  const size_t size = m_string_size(v);
  if (index >= size) {
    ptr[0] = 0;
    m_str1ng_set_size(v, 0);
    M_STR1NG_CONTRACT (v);
    return;
  }
  size_t s2 = size - index;
  memmove (&ptr[0], &ptr[index], s2+1);
  m_str1ng_set_size(v, s2);
  M_STR1NG_CONTRACT (v);
}

void m_string_mid (m_string_t v, size_t index, size_t size){
  m_string_right(v, index);
  m_string_left(v, size);
}

size_t m_string_replace_cstr (m_string_t v, const char str1[], const char str2[], size_t start){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (str1 != NULL && str2 != NULL);
  size_t i = m_string_search_cstr(v, str1, start);
  if (i != M_STRING_FAILURE) {
    const size_t str1_l = strlen(str1);
    const size_t str2_l = strlen(str2);
    const size_t size   = m_string_size(v);
    M_ASSERT(size + 1 + str2_l > str1_l);
    char *ptr = m_str1ng_fit2size (v, size + str2_l - str1_l + 1);
    if (str1_l != str2_l) {
      memmove(&ptr[i+str2_l], &ptr[i+str1_l], size - i - str1_l + 1);
      m_str1ng_set_size(v, size + str2_l - str1_l);
    }
    memcpy (&ptr[i], str2, str2_l);
    M_STR1NG_CONTRACT (v);
  }
  return i;
}

size_t m_string_replace (m_string_t v, const m_string_t v1, const m_string_t v2, size_t start){
  M_STR1NG_CONTRACT (v);
  M_STR1NG_CONTRACT (v1);
  M_STR1NG_CONTRACT (v2);
  return m_string_replace_cstr(v, m_string_get_cstr(v1), m_string_get_cstr(v2), start);
}

void m_string_replace_at (m_string_t v, size_t pos, size_t len, const char str2[]){
  M_STR1NG_CONTRACT (v);
  M_ASSERT(str2 != NULL);
  const size_t str1_l = len;
  const size_t str2_l = strlen(str2);
  const size_t size   = m_string_size(v);
  char *ptr;
  if (str1_l != str2_l) {
    // Move bytes from the string 
    M_ASSERT_INDEX (str1_l, size + str2_l + 1);
    ptr = m_str1ng_fit2size (v, size + str2_l - str1_l + 1);
    M_ASSERT_INDEX (pos + str1_l, size + 1);
    M_ASSUME (pos + str1_l < size + 1);
    memmove(&ptr[pos+str2_l], &ptr[pos+str1_l], size - pos - str1_l + 1);
    m_str1ng_set_size(v, size + str2_l - str1_l);
  } else {
    ptr = m_str1ng_get_cstr(v);
  }
  memcpy (&ptr[pos], str2, str2_l);
  M_STR1NG_CONTRACT (v);
}

void m_str1ng_replace_all_cstr_1ge2 (m_string_t v, const char str1[], size_t str1len, const char str2[], size_t str2len){
  M_STR1NG_CONTRACT (v);
  M_ASSERT(str1len >= str2len);

  /* str1len >= str2len so the string doesn't need to be resized */
  size_t vlen = m_string_size(v);
  char *org = m_str1ng_get_cstr(v);
  char *src = org;
  char *dst = org;

  // Go through all the characters of the string
  while (*src != 0) {
    // Get a new occurrence of str1 in the v string.
    char *occ = strstr(src, str1);
    if (occ == NULL) {
      // No new occurrence
      break;
    }
    M_ASSERT(occ >= src);
    // Copy the data until the new occurrence
    if (src != dst) {
      memmove(dst, src, (size_t) (occ - src));
    }
    dst += (occ - src);
    src  = occ;
    // Copy the replaced string
    memcpy(dst, str2, str2len);
    dst += str2len;
    // Advance src pointer
    src  = occ + str1len;
  }
  // Finish copying the string until the end
  M_ASSERT (src <= org + vlen );
  if (src != dst) {
    memmove(dst, src, (size_t) (org + vlen + 1 - src) );
  }
  // Update string size
  m_str1ng_set_size(v, (size_t) (dst + vlen - src) );
  M_STR1NG_CONTRACT (v);
}

char * m_str1ng_strstr_r(char org[], char src[], const char pattern[], size_t pattern_size){
  M_ASSERT(pattern_size >= 1);
  src -= pattern_size - 1;
  while (org <= src) {
    if (src[0] == pattern[0]
      && src[pattern_size-1] == pattern[pattern_size-1]
      && memcmp(src, pattern, pattern_size) == 0) {
        return src;
    }
    src --;
  }
  return NULL;
}

void m_str1ng_replace_all_cstr_1lo2 (m_string_t v, const char str1[], size_t str1len, const char str2[], size_t str2len){
  M_STR1NG_CONTRACT (v);
  M_ASSERT(str1len < str2len);

  /* str1len < str2len so the string may need to be resized
    Worst case if when v is composed fully of str1 substrings.
    Needed size : v.size / str1len * str2len
   */
  size_t vlen = m_string_size(v);
  size_t alloc = 1 + vlen / str1len * str2len;
  char *org = m_str1ng_fit2size(v, alloc);
  char *src = org + vlen - 1;
  char *end = org + m_string_capacity(v);
  char *dst = end;

  // Go through all the characters of the string in reverse !
  while (src >= org) {
    char *occ = m_str1ng_strstr_r(org, src, str1, str1len);
    if (occ == NULL) {
      break;
    }
    M_ASSERT(occ + str1len - 1 <= src);
    // Copy the data until the new occurrence
    dst -= (src - (occ + str1len - 1));
    memmove(dst, occ+str1len, (size_t) (src - (occ + str1len - 1)));
    // Copy the replaced string
    dst -= str2len;
    memcpy(dst, str2, str2len);
    // Advance src pointer
    src  = occ - 1;
  }
  // Finish moving data back to their natural place
  memmove(src + 1, dst, (size_t) (end - dst) );
  // Update string size
  vlen = (size_t) (src - org + end - dst + 1);
  org[vlen] = 0;
  m_str1ng_set_size(v, vlen );
  M_STR1NG_CONTRACT (v);
}

void m_string_replace_all_cstr (m_string_t v, const char str1[], const char str2[]){
  size_t str1_l = strlen(str1);
  size_t str2_l = strlen(str2);
  assert(str1_l > 0);
  if (str1_l >= str2_l) {
    m_str1ng_replace_all_cstr_1ge2(v, str1, str1_l, str2, str2_l );
  } else {
    m_str1ng_replace_all_cstr_1lo2(v, str1, str1_l, str2, str2_l );
  }
}

void m_string_replace_all (m_string_t v, const m_string_t str1, const m_string_t str2){
  size_t str1_l = m_string_size(str1);
  size_t str2_l = m_string_size(str2);
  assert(str1_l > 0);
  assert(str2_l > 0);
  if (str1_l >= str2_l) {
    m_str1ng_replace_all_cstr_1ge2(v, m_string_get_cstr(str1), str1_l, m_string_get_cstr(str2), str2_l );
  } else {
    m_str1ng_replace_all_cstr_1lo2(v, m_string_get_cstr(str1), str1_l, m_string_get_cstr(str2), str2_l );
  }
}

void m_string_set_ui(m_string_t v, unsigned int n){
  M_STR1NG_CONTRACT (v);
  // Compute the maximum number of characters needed for the buffer.
#if UINT_MAX == 4294967295U
  const size_t max_size = 10+1;
#elif UINT_MAX <= 18446744073709551615UL
  const size_t max_size = 20+1;
#else
# error Unexpected UINT_MAX value (workaround: Define M_USE_FAST_STRING_CONV to 0).
#endif
  char buffer[max_size];
  m_str1ng_fit2size(v, max_size);
  unsigned i = 0, j = 0;
  do {
    // 0123456789 are mandatory in this order as characters, as per C standard.
    buffer[i++] = (char) ('0' + (n % 10U));
    n /= 10U;
  } while (n != 0);
  M_ASSERT_INDEX(i, max_size);
  char *d = m_str1ng_get_cstr(v);
  while (i > 0) {
    d[j++] = buffer[--i];
  }
  d[j] = 0;
  m_str1ng_set_size(v, j);
  M_STR1NG_CONTRACT (v);
}

void m_string_set_si(m_string_t v, int n){
  M_STR1NG_CONTRACT (v);
  // Compute the maximum number of characters needed for the buffer.
#if INT_MAX == 2147483647
  const size_t max_size = 1+10+1;
#elif INT_MAX <= 9223372036854775807LL
  const size_t max_size = 1+20+1;
#else
# error Unexpected UINT_MAX value (workaround: Define M_USE_FAST_STRING_CONV to 0).
#endif
  char buffer[max_size];
  m_str1ng_fit2size(v, max_size);
  unsigned i = 0, j = 0;
  bool neg = n < 0;
  unsigned n0 = neg ? 0U -(unsigned) n : (unsigned) n;
  do {
    // 0123456789 are mandatory in this order as characters, as per C standard.
    buffer[i++] = (char) ('0' + (n0 % 10U));
    n0 /= 10U;
  } while (n0 != 0);
  M_ASSERT_INDEX(i, max_size);
  char *d = m_str1ng_get_cstr(v);
  if (neg) d[j++] = '-';
  while (i > 0) {
    d[j++] = buffer[--i];
  }
  d[j] = 0;
  m_str1ng_set_size(v, j);
  M_STR1NG_CONTRACT (v);
}

int m_string_vprintf (m_string_t v, const char format[], va_list args){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (format != NULL);
  int size;
  va_list args_org;
  va_copy(args_org, args);
  char *ptr = m_str1ng_get_cstr(v);
  size_t alloc = m_string_capacity(v);
  size = vsnprintf (ptr, alloc, format, args);
  if (size > 0 && ((size_t) size+1 >= alloc) ) {
    // We have to realloc our string to fit the needed size
    ptr = m_str1ng_fit2size (v, (size_t) size + 1);
    alloc = m_string_capacity(v);
    // and redo the parsing.
    va_copy(args, args_org);
    size = vsnprintf (ptr, alloc, format, args);
    M_ASSERT (size > 0 && (size_t)size < alloc);
  }
  if (M_LIKELY (size >= 0)) {
    m_str1ng_set_size(v, (size_t) size);
  } else {
    // An error occured during the conversion: Assign an empty string.
    m_str1ng_set_size(v, 0);
    ptr[0] = 0;
  }
  M_STR1NG_CONTRACT (v);
  return size;
}

int m_string_printf (m_string_t v, const char format[], ...){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (format != NULL);
  va_list args;
  va_start (args, format);
  int ret = m_string_vprintf(v, format, args);
  va_end (args);
  return ret;
}

int m_string_cat_printf (m_string_t v, const char format[], ...){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (format != NULL);
  va_list args;
  int size;
  size_t old_size = m_string_size(v);
  char  *ptr      = m_str1ng_get_cstr(v);
  size_t alloc    = m_string_capacity(v);
  va_start (args, format);
  size = vsnprintf (&ptr[old_size], alloc - old_size, format, args);
  if (size > 0 && (old_size+(size_t)size+1 >= alloc) ) {
    // We have to realloc our string to fit the needed size
    ptr = m_str1ng_fit2size (v, old_size + (size_t) size + 1);
    alloc = m_string_capacity(v);
    // and redo the parsing.
    va_end (args);
    va_start (args, format);
    size = vsnprintf (&ptr[old_size], alloc - old_size, format, args);
    M_ASSERT (size >= 0);
  }
  if (size >= 0) {
    m_str1ng_set_size(v, old_size + (size_t) size);
  } else {
    // vsnprintf may have output some characters before returning an error.
    // Undo this to have a clean state
    ptr[old_size] = 0;
  }
  va_end (args);
  M_STR1NG_CONTRACT (v);
  return size;
}

bool m_string_fgets(m_string_t v, FILE *f, m_string_fgets_t arg){
  M_STR1NG_CONTRACT(v);
  M_ASSERT (f != NULL);
  char *ptr = m_str1ng_fit2size (v, 100);
  size_t size = 0;
  size_t alloc = m_string_capacity(v);
  ptr[0] = 0;
  bool retcode = false; /* Nothing has been read yet */
  /* alloc - size is very unlikely to be bigger than INT_MAX
    but fgets accepts an int as the size argument */
  while (fgets(&ptr[size], (int) M_MIN( (alloc - size), (size_t) INT_MAX ), f) != NULL) {
    retcode = true; /* Something has been read */
    // fgets doesn't return the number of characters read, so we need to count.
    size += strlen(&ptr[size]);
    assert(size >= 1);
    if (arg != M_STRING_READ_FILE && ptr[size-1] == '\n') {
      if (arg == M_STRING_READ_PURE_LINE) {
        size --;
        ptr[size] = 0;         /* Remove EOL */
      }
      m_str1ng_set_size(v, size);
      M_STR1NG_CONTRACT(v);
      return retcode; /* Normal terminaison */
    } else if (ptr[size-1] != '\n' && !feof(f)) {
      /* The string buffer is not big enough:
         increase it and continue reading */
      /* v cannot be stack alloc */
      ptr   = m_str1ng_fit2size (v, alloc + alloc/2);
      alloc = m_string_capacity(v);
    }
  }
  m_str1ng_set_size(v, size);
  M_STR1NG_CONTRACT (v);
  return retcode; /* Abnormal terminaison */
}

bool m_string_fget_word (m_string_t v, const char separator[], FILE *f){
  char buffer[128];
  char c = 0;
  int d;
  M_STR1NG_CONTRACT(v);
  M_ASSERT (f != NULL);
  M_ASSERT_INDEX (1+20+2+strlen(separator)+3, sizeof buffer);
  size_t size = 0;
  bool retcode = false;

  /* Skip separator first */
  do {
    d = fgetc(f);
    if (d == EOF) {
      return false;
    }
  } while (strchr(separator, d) != NULL);
  ungetc(d, f);

  size_t alloc = m_string_capacity(v);
  char *ptr    = m_str1ng_get_cstr(v);
  ptr[0] = 0;

  /* NOTE: We generate a buffer which we give to scanf to parse the string,
     that it is to say, we generate the format dynamically!
     The format is like " %49[^ \t.\n]%c"
     So in this case, we parse up to 49 chars, up to the separators char,
     and we read the next char. If the next char is a separator, we successful
     read a word, otherwise we have to continue parsing.
     The user shall give a constant string as the separator argument,
     as a control over this argument may give an attacker
     an opportunity for stack overflow */
  while (snprintf(buffer, sizeof buffer -1, " %%%zu[^%s]%%c", (size_t) alloc-1-size, separator) > 0
         /* We may read one or two argument(s) */
         && m_core_fscanf(f, buffer, m_core_arg_size(&ptr[size], alloc-size), &c) >= 1) {
    retcode = true;
    size += strlen(&ptr[size]);
    /* If we read only one argument 
       or if the final read character is a separator */
    if (c == 0 || strchr(separator, c) != NULL)
      break;
    /* Next char is not a separator: continue parsing */
    m_str1ng_set_size(v, size);
    ptr = m_str1ng_fit2size (v, alloc + alloc/2);
    alloc = m_string_capacity(v);
    M_ASSERT (alloc > size + 1);
    ptr[size++] = c;
    ptr[size] = 0;
    // Invalid c character for next iteration
    c= 0;
  }
  m_str1ng_set_size(v, size);
  M_STR1NG_CONTRACT(v);  
  return retcode;
}

bool m_string_fputs(FILE *f, const m_string_t v){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (f != NULL);
  return fputs(m_string_get_cstr(v), f) >= 0;
}

bool m_string_start_with_str_p(const m_string_t v, const char str[]){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (str != NULL);
  const char *v_str = m_string_get_cstr(v);
  while (*str){
    if (*str != *v_str)
      return false;
    str++;
    v_str++;
  }
  return true;
}

bool m_string_start_with_string_p(const m_string_t v, const m_string_t v2){
  M_STR1NG_CONTRACT (v2);
  return m_string_start_with_str_p (v, m_string_get_cstr(v2));
}

bool m_string_end_with_str_p(const m_string_t v, const char str[]){
  M_STR1NG_CONTRACT (v);
  M_ASSERT (str != NULL);
  const char *v_str  = m_string_get_cstr(v);
  const size_t v_l   = m_string_size(v);
  const size_t str_l = strlen(str);
  if (v_l < str_l)
    return false;
  return (memcmp(str, &v_str[v_l - str_l], str_l) == 0);
}

bool m_string_end_with_string_p(const m_string_t v, const m_string_t v2){
  M_STR1NG_CONTRACT (v2);
  return m_string_end_with_str_p (v, m_string_get_cstr(v2));
}

size_t m_string_hash(const m_string_t v){
  M_STR1NG_CONTRACT (v);
  return m_core_hash(m_string_get_cstr(v), m_string_size(v));
}

bool m_str1ng_strim_char(char c, const char charac[]){
  for(const char *s = charac; *s; s++) {
    if (c == *s)
      return true;
  }
  return false;
}

void m_string_strim(m_string_t v, const char charac[]){
  M_STR1NG_CONTRACT (v);
  char *ptr = m_str1ng_get_cstr(v);
  char *b   = ptr;
  size_t size = m_string_size(v);
  while (size > 0 && m_str1ng_strim_char(b[size-1], charac))
    size --;
  if (size > 0) {
    while (m_str1ng_strim_char(*b, charac))
      b++;
    M_ASSERT (b >= ptr &&  size >= (size_t) (b - ptr) );
    size -= (size_t) (b - ptr);
    memmove (ptr, b, size);
  }
  ptr[size] = 0;
  m_str1ng_set_size(v, size);
  M_STR1NG_CONTRACT (v);
}

bool m_string_oor_equal_p(const m_string_t s, unsigned char n){
  return (s->ptr == NULL) & (s->u.heap.alloc == ~(size_t)n);
}

void m_string_oor_set(m_string_t s, unsigned char n){
  s->ptr = NULL;
  s->u.heap.alloc = ~(size_t)n;
}

void m_string_get_str(m_string_t v, const m_string_t v2, bool append){
  M_STR1NG_CONTRACT(v2);
  M_STR1NG_CONTRACT(v);
  M_ASSERT (v != v2); // Limitation
  size_t size = append ? m_string_size(v) : 0;
  size_t v2_size = m_string_size(v2);
  size_t targetSize = size + v2_size + 3;
  char *ptr = m_str1ng_fit2size(v, targetSize);
  ptr[size ++] = '"';
  for(size_t i = 0 ; i < v2_size; i++) {
    const char c = m_string_get_char(v2,i);
    switch (c) {
    case '\\':
    case '"':
    case '\n':
    case '\t':
    case '\r':
      // Special characters which can be displayed in a short form.
      m_str1ng_set_size(v, size);
      ptr = m_str1ng_fit2size(v, ++targetSize);
      ptr[size ++] = '\\';
      // This string acts as a perfect hashmap which supposes an ASCII mapping
      // and (c^(c>>5)) is the hash function
      ptr[size ++] = " tn\" r\\"[(c ^ (c >>5)) & 0x07];
      break;
    default:
      if (M_UNLIKELY (!isprint(c))) {
        targetSize += 3;
        m_str1ng_set_size(v, size);
        ptr = m_str1ng_fit2size(v, targetSize);
        int d1 = c & 0x07, d2 = (c>>3) & 0x07, d3 = (c>>6) & 0x07;
        ptr[size ++] = '\\';
        ptr[size ++] = (char) ('0' + d3);
        ptr[size ++] = (char) ('0' + d2);
        ptr[size ++] = (char) ('0' + d1);
      } else {
        ptr[size ++] = c;
      }
      break;
    }
  }
  ptr[size ++] = '"';
  ptr[size] = 0;
  m_str1ng_set_size(v, size);
  M_ASSERT (size <= targetSize);
  M_STR1NG_CONTRACT (v);
}

void m_string_out_str(FILE *f, const m_string_t v){
  M_STR1NG_CONTRACT(v);
  M_ASSERT (f != NULL);
  fputc('"', f);
  size_t size = m_string_size(v);
  for(size_t i = 0 ; i < size; i++) {
    const char c = m_string_get_char(v, i);
    switch (c) {
    case '\\':
    case '"':
    case '\n':
    case '\t':
    case '\r':
      fputc('\\', f);
      fputc(" tn\" r\\"[(c ^ c >>5) & 0x07], f);
      break;
    default:
      if (M_UNLIKELY (!isprint(c))) {
        int d1 = c & 0x07, d2 = (c>>3) & 0x07, d3 = (c>>6) & 0x07;
        fputc('\\', f);
        fputc('0' + d3, f);
        fputc('0' + d2, f);
        fputc('0' + d1, f);
      } else {
        fputc(c, f);
      }
      break;
    }
  }
  fputc('"', f);
}

bool m_string_in_str(m_string_t v, FILE *f){
  M_STR1NG_CONTRACT(v);
  M_ASSERT (f != NULL);
  int c = fgetc(f);
  if (c != '"') return false;
  m_string_reset(v);
  c = fgetc(f);
  while (c != '"' && c != EOF) {
    if (M_UNLIKELY (c == '\\')) {
      c = fgetc(f);
      switch (c) {
      case 'n':
      case 't':
      case 'r':
      case '\\':
      case '\"':
        // This string acts as a perfect hashmap which supposes an ASCII mapping
        // and (c^(c>>5)) is the hash function
        c = " \r \" \n\\\t"[(c^(c>>5))& 0x07];
        break;
      default:
        if (!(c >= '0' && c <= '7'))
          return false;
        int d1 = c - '0';
        c = fgetc(f);
        if (!(c >= '0' && c <= '7'))
          return false;
        int d2 = c - '0';
        c = fgetc(f);
        if (!(c >= '0' && c <= '7'))
          return false;
        int d3 = c - '0';
        c = (d1 << 6) + (d2 << 3) + d3;
        break;
      }
    }
    m_string_push_back (v, (char) c);
    c = fgetc(f);
  }
  return c == '"';
}

bool m_string_parse_str(m_string_t v, const char str[], const char **endptr){
  M_STR1NG_CONTRACT(v);
  M_ASSERT (str != NULL);
  bool success = false;
  int c = *str++;
  if (c != '"') goto exit;
  m_string_reset(v);
  c = *str++;
  while (c != '"' && c != 0) {
    if (M_UNLIKELY (c == '\\')) {
      c = *str++;
      switch (c) {
      case 'n':
      case 't':
      case 'r':
      case '\\':
      case '\"':
        // This string acts as a perfect hashmap which supposes an ASCII mapping
        // and (c^(c>>5)) is the hash function
        c = " \r \" \n\\\t"[(c^(c>>5))& 0x07];
        break;
      default:
        if (!(c >= '0' && c <= '7'))
          goto exit;
        int d1 = c - '0';
        c = *str++;
        if (!(c >= '0' && c <= '7'))
          goto exit;
        int d2 = c - '0';
        c = *str++;
        if (!(c >= '0' && c <= '7'))
          goto exit;
        int d3 = c - '0';
        c = (d1 << 6) + (d2 << 3) + d3;
        break;
      }
    }
    m_string_push_back (v, (char) c);
    c = *str++;
  }
  success = (c == '"');
 exit:
  if (endptr != NULL) *endptr = str;
  return success;
}

m_serial_return_code_t m_string_out_serial(m_serial_write_t serial, const m_string_t v){
  M_ASSERT (serial != NULL && serial->m_interface != NULL);
  return serial->m_interface->write_string(serial, m_string_get_cstr(v), m_string_size(v) );
}

m_serial_return_code_t m_string_in_serial(m_string_t v, m_serial_read_t serial){
  M_ASSERT (serial != NULL && serial->m_interface != NULL);
  return serial->m_interface->read_string(serial, v);
}

void m_str1ng_utf8_decode(char c, m_str1ng_utf8_state_e *state,                     m_string_unicode_t *unicode){
  const unsigned int type = m_core_clz32((unsigned char)~c) - (unsigned) (sizeof(uint32_t) - 1) * CHAR_BIT;
  const m_string_unicode_t mask1 = (UINT32_MAX - (m_string_unicode_t)(*state != M_STR1NG_UTF8_STARTING) + 1);
  const m_string_unicode_t mask2 = (0xFFU >> type);
  *unicode = ((*unicode << 6) & mask1) | ((unsigned int) c & mask2);
  *state = (m_str1ng_utf8_state_e) M_STR1NG_UTF8_STATE_TAB[(unsigned int) *state + type];
}

bool m_str1ng_utf8_valid_str_p(const char str[]){
  m_str1ng_utf8_state_e s = M_STR1NG_UTF8_STARTING;
  m_string_unicode_t u = 0;
  while (*str) {
    m_str1ng_utf8_decode(*str, &s, &u);
    if ((s == M_STR1NG_UTF8_ERROR)
        ||(s == M_STR1NG_UTF8_STARTING
           &&(u > 0x10FFFF /* out of range */
              ||(u >= 0xD800 && u <= 0xDFFF) /* surrogate halves */)))
      return false;
    str++;
  }
  return true;
}

bool m_str1ng_utf8_start_p(unsigned char val){
  return ((val & 0xC0u) != 0x80u);
}

size_t m_str1ng_utf8_length(const char str[]){
  size_t size = 0;
  while (*str) {
    unsigned char val = (unsigned char) *str++;
    size += m_str1ng_utf8_start_p(val);
  }
  return size;
}

int m_str1ng_utf8_encode(char buffer[5], m_string_unicode_t u){
  if (M_LIKELY (u <= 0x7Fu)) {
    buffer[0] = (char) u;
    buffer[1] = 0;
    return 1;
  } else if (u <= 0x7FFu) {
    buffer[0] = (char) (0xC0u | (u >> 6));
    buffer[1] = (char) (0x80 | (u & 0x3Fu));
    buffer[2] = 0;
    return 2;
  } else if (u <= 0xFFFFu) {
    buffer[0] = (char) (0xE0u | (u >> 12));
    buffer[1] = (char) (0x80u | ((u >> 6) & 0x3Fu));
    buffer[2] = (char) (0x80u | (u & 0x3Fu));
    buffer[3] = 0;
    return 3;
  } else {
    buffer[0] = (char) (0xF0u | (u >> 18));
    buffer[1] = (char) (0x80u | ((u >> 12) & 0x3Fu));
    buffer[2] = (char) (0x80u | ((u >> 6) & 0x3Fu));
    buffer[3] = (char) (0x80u | (u & 0x3F));
    buffer[4] = 0;
    return 4;
  }
}

void m_string_it(m_string_it_t it, const m_string_t str){
  M_STR1NG_CONTRACT(str);
  M_ASSERT(it != NULL);
  it->ptr      = m_string_get_cstr(str);
  it->u        = 0;
  it->string   = str;
  M_STR1NG_IT_CONTRACT(it);
} 

void m_string_it_end(m_string_it_t it, const m_string_t str){
  M_STR1NG_CONTRACT(str);
  M_ASSERT(it != NULL);
  it->ptr      = &m_string_get_cstr(str)[m_string_size(str)];
  it->u        = 0;
  it->string   = str;
  M_STR1NG_IT_CONTRACT(it);
}

void m_string_it_set(m_string_it_t it, const m_string_it_t itsrc){
  M_ASSERT(it != NULL && itsrc != NULL);
  M_STR1NG_IT_CONTRACT(itsrc);
  it->ptr      = itsrc->ptr;
  it->u        = itsrc->u;
  it->string   = itsrc->string;
  M_STR1NG_IT_CONTRACT(it);
}

void m_string_it_pos(m_string_it_t it, const m_string_t str, const size_t n){
  M_ASSERT(it != NULL);
  M_STR1NG_CONTRACT(str);
  // The offset shall be within the string
  M_ASSERT(n <= m_string_size(str));
  // The offset shall reference the first Byte of an UTF 8 Code point
  M_ASSERT(m_str1ng_utf8_start_p ((unsigned char)m_string_get_cstr(str)[n]));
  it->ptr      = &m_string_get_cstr(str)[n];
  it->u        = 0;
  it->string   = str;
  M_STR1NG_IT_CONTRACT(it);
}

size_t m_string_it_get_pos(m_string_it_t it){
  M_STR1NG_IT_CONTRACT(it);
  return (size_t) (it->ptr - m_string_get_cstr(it->string));
}

bool m_string_end_p (m_string_it_t it){
  M_STR1NG_IT_CONTRACT(it);
  return it->ptr[0] == 0;
}

bool m_string_it_equal_p(const m_string_it_t it1, const m_string_it_t it2){
  M_STR1NG_IT_CONTRACT(it1);
  M_STR1NG_IT_CONTRACT(it2);
  return it1->ptr == it2->ptr;
}

void m_string_next (m_string_it_t it){
  M_STR1NG_IT_CONTRACT(it);
  const char *ptr = it->ptr;
  while (*ptr != 0) {
    ptr ++;
    if (m_str1ng_utf8_start_p((unsigned char) *ptr) ) {
      /* Start of an UTF 8 code point */
      break;
    }
  }
  it->ptr = ptr;
  return;
}

void m_string_previous(m_string_it_t it){
  M_STR1NG_IT_CONTRACT(it);
  const char *ptr = it->ptr;
  const char *org = m_string_get_cstr(it->string);
  while (ptr > org) {
    ptr --;
    if (m_str1ng_utf8_start_p((unsigned char) *ptr) ) {
      /* Start of an UTF 8 code point */
      it->ptr = ptr;
      return;
    }
  }
  /* We reach the start of the string: mark the iterator to the end */
  it->ptr = &org[m_string_size(it->string)];
  M_STR1NG_IT_CONTRACT(it);
}

m_string_unicode_t m_string_get_cref (const m_string_it_t it){
  M_STR1NG_IT_CONTRACT(it);
  M_ASSERT(*it->ptr != 0);

  m_str1ng_utf8_state_e state =  M_STR1NG_UTF8_STARTING;
  m_string_unicode_t u = 0;
  const char *str = it->ptr;
  do {
    m_str1ng_utf8_decode(*str, &state, &u);
    str++;
  } while (state != M_STR1NG_UTF8_STARTING && state != M_STR1NG_UTF8_ERROR && *str != 0);
  // Save where the current unicode value ends in the UTF8 steam
  M_ASSERT( (str > it->ptr) && (str - it->ptr) <= M_STR1NG_MAX_BYTE_UTF8);
  // Save the decoded unicode value
  return M_UNLIKELY (state == M_STR1NG_UTF8_ERROR) ? M_STRING_UNICODE_ERROR : u;
}

const m_string_unicode_t * m_string_cref (m_string_it_t it){
  M_STR1NG_IT_CONTRACT(it);
  it->u = m_string_get_cref(it);
  return &it->u;
}

void m_string_push_u (m_string_t str, m_string_unicode_t u){
  M_STR1NG_CONTRACT(str);
  char buffer[4+1];
  m_str1ng_utf8_encode(buffer, u);
  m_string_cat_cstr(str, buffer);
}

size_t m_string_length_u(m_string_t str){
  M_STR1NG_CONTRACT(str);
  return m_str1ng_utf8_length(m_string_get_cstr(str));
}

bool m_string_utf8_p(m_string_t str){
  M_STR1NG_CONTRACT(str);
  return m_str1ng_utf8_valid_str_p(m_string_get_cstr(str));
}

