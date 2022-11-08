#define NDEBUG

#include <QLinkedList>
#include <QVector>
#include <QSet>
#include <QMap>
#include <QHash>
#include <QtAlgorithms>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

using namespace std;

#include "common.h"

/********************************************************************************************/

static void test_list(size_t n)
{
  QLinkedList<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for (QLinkedList<unsigned int>::const_iterator ci1 = a1.begin(), ci2 = a2.begin (); ci1 != a1.end(); ++ci1, ++ci2) {
    s += *ci1 * *ci2;
  }
  g_result = s;
}

/********************************************************************************************/

static void test_array(size_t n)
{
  QVector<unsigned int> a1, a2;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
    a2.push_back(rand_get() );
  }
  unsigned int s = 0;
  for(unsigned long i = 0; i < n; i++) {
    s += a1[i] * a2[i];
  }
  g_result = s;
}

/********************************************************************************************/

/* NOTE: QSet doesn't perform full ordering. I can't find any container
   which is a set with total ordering within Qt. So we have to fake it. */
static void test_rbtree(size_t n)
{
  QMap<unsigned long, QHashDummyValue> tree;

  for (size_t i = 0; i < n; i++) {
    tree[rand_get()] = QHashDummyValue();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    QMap<unsigned long, QHashDummyValue>::iterator it = tree.find(rand_get());
    if (it != tree.end())
      s += it.key();
  }
  g_result = s;
}

/********************************************************************************************/

static void
test_dict1(size_t  n)
{
  QMap<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    QMap<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it.value();
  }
  g_result = s;
}

/********************************************************************************************/

static void
test_dict2(size_t  n)
{
  QHash<unsigned long, unsigned long> dict;

  for (size_t i = 0; i < n; i++) {
    dict[rand_get()] = rand_get();
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    QHash<unsigned long, unsigned long>::iterator it = dict.find(rand_get());
    if (it != dict.end())
      s += it.value();
  }
  g_result = s;
}

/********************************************************************************************/
struct char_array_s {
  char a[256];
  char_array_s () { a[0] = 0 ; }
  char_array_s ( const char_array_s & other) { strcpy(a, other.a); }
  bool operator==(const char_array_s &other) const { return strcmp(a, other.a) == 0; }
};

uint qHash(const char_array_s &k)
{
  size_t hash = 0;
  const char *s = k.a;
  while (*s) hash = hash * 31421 + (*s++) + 6927;
  return hash;
};

static void
test_dict_big(size_t  n)
{
  QHash<char_array_s, char_array_s> dict;

  for (size_t i = 0; i < n; i++) {
    char_array_s s1, s2;
    sprintf(s1.a, "%u", rand_get());
    sprintf(s2.a, "%u", rand_get());
    dict[s1] = s2;
  }
  rand_init();
  unsigned int s = 0;
  for (size_t i = 0; i < n; i++) {
    char_array_s s1;
    sprintf(s1.a, "%u", rand_get());
    QHash<char_array_s, char_array_s>::iterator it = dict.find(s1);
    if (it != dict.end())
      s ++;
  }
  g_result = s;
}

/********************************************************************************************/

static void test_sort(size_t n)
{
  QVector<float> a1;
  for(size_t i = 0; i < n; i++) {
    a1.push_back(rand_get() );
  }
  qSort(a1.begin(), a1.end());
  g_result = a1[0];
}

/********************************************************************************************/

const config_func_t table[] = {
  { 10,    "List", 10000000, 0, test_list, 0},
  { 20,   "Array", 100000000, 0, test_array, 0},
  { 30,  "Rbtree", 1000000, 0, test_rbtree, 0},
  { 40,    "dict(m)", 1000000, 0, test_dict1, 0},
  { 41, "dictBig", 1000000, 0, test_dict_big, 0},
  { 42,"dict(u)", 1000000, 0, test_dict2, 0},
  { 50,           "Sort",10000000, 0, test_sort, 0}
};

int main(int argc, const char *argv[])
{
  test("QT", numberof(table), table, argc, argv);
  exit(0);
}
