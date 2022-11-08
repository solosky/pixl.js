// program to find longest contiguous subsequence
// based on https://www.geeksforgeeks.org/longest-consecutive-subsequence/
#include <iostream>
#include <unordered_set>

using namespace std; 
  
// Returns length of the longest contiguous subsequence 
int cpp_find_longest(int arr[], int n) 
{ 
  unordered_set<int> S; 
  int ans = 0; 
  
  // Hash all the array elements 
  for (int i = 0; i < n; i++) 
    S.insert(arr[i]); 
  
  // check each possible sequence from 
  // the start then update optimal length 
  for (int i = 0; i < n; i++) { 
    // if current element is the starting 
    // element of a sequence 
    if (S.find(arr[i] - 1) == S.end()) { 
      // Then check for next elements 
      // in the sequence 
      int j = arr[i] + 1;
      while (S.find(j) != S.end()) 
        j++; 
      
      // update  optimal length if 
      // this length is more 
      ans = max(ans, j - arr[i]); 
    } 
  } 
  return ans; 
} 

#include "m-dict.h"

static inline bool oor_equal_p(int a, unsigned char c) { return a == ((c == 0) ? INT_MIN : INT_MAX); }
static inline void oor_set(int *a, unsigned char c) { *a = ((c == 0) ? INT_MIN : INT_MAX); }

DICT_OASET_DEF(dict_int, int, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)))

// Returns length of the longest contiguous subsequence 
int mlib_find_longest(int arr[], int n) 
{ 
  dict_int_t S;
  int ans = 0; 

  dict_int_init(S);
  
  // Hash all the array elements 
  for (int i = 0; i < n; i++) 
    dict_int_push(S, arr[i]);
  
  // check each possible sequence from 
  // the start then update optimal length 
  for (int i = 0; i < n; i++) { 
    // if current element is the starting 
    // element of a sequence
    int *p = dict_int_get(S, arr[i]-1);
    if (p == NULL) {
      // Then check for next elements 
      // in the sequence 
      int j = arr[i] + 1; 
      while (dict_int_get(S, j) != NULL)
        j++; 
      
      // update  optimal length if 
      // this length is more 
      ans = M_MAX(ans, j - arr[i]); 
    } 
  }

  dict_int_clear(S);
  return ans; 
} 

#include "common.h"

int main(int argc, const char *argv[]) 
{
  int n = argc == 1 ? 10000 : atoi(argv[1]);
  int *arr = (int*) malloc(n * sizeof(int));
  for(int i = 0; i < n; i++)
    arr[i] = rand_get();
  
  unsigned long long t0 = cputime();
  cout << "Length of the Longest contiguous subsequence is "
       << cpp_find_longest(arr, n) << endl; 
  
  unsigned long long t1 = cputime();
  
  cout << "Length of the Longest contiguous subsequence is "
       << mlib_find_longest(arr, n) << endl;
  unsigned long long t2 = cputime();
  
  cout << "Time C++: " << (t1-t0) << " Time M*LIB: " << (t2-t1) << endl;
  return 0; 
}
