#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "m-tuple.h"
#include "m-tree.h"
#include "m-worker.h"
#include "m-string.h"
#include "m-atomic.h"

/* Sort the atomic unsigned int with the biggest value first. */
static inline int atomic_uint_cmp(const atomic_uint *a, const atomic_uint *b)
{
  unsigned ai = atomic_load(a), bi = atomic_load(b);
  return ai > bi ? -1 : ai < bi;
}
/* Disable HASH for atomic as the default hash method won't work */
#define M_OPL_atomic_uint() M_OPEXTEND(M_BASIC_OPLIST, HASH(0), CMP(API_6(atomic_uint_cmp)))

/* Define a directory as the number of lines of the files in this directory
   and the name of the directory 
   (Only sort with the number value by disable the sort compare for the string).
*/
TUPLE_DEF2( dir, (nlines, atomic_uint), (name, string_t, M_OPEXTEND(STRING_OPLIST, CMP(0))))
#define M_OPL_dir_t() TUPLE_OPLIST(dir, M_BASIC_OPLIST, M_STRING_OPLIST)

/* A hierarchical tree of directory */
M_TREE_DEF(tree, dir_t)

/* Maximum number of characters read by file */
#define MAX_READ_BUFFER 8192

/* Maximum number of directory to scan */
#define MAX_DIRECTORY 100000

static tree_it_t
add_directory(tree_it_t parent, const string_t dirname)
{
  // Miss: return dir_emplace_child(parent, 0, dirname); to simplify this function.
  // which is too complex for what she does.
  dir_t data;
  tree_it_t it;
  dir_init_emplace(data, 0, dirname);
  if (tree_end_p(parent)) {
    it = tree_set_root( tree_tree(parent), data);
  } else {
    it = tree_insert_child(parent, data);
  }
  dir_clear(data);
  return it;
}

/* count the number of end of line character in the buffer */
static unsigned
count_eol(const char buffer[], size_t num)
{
  unsigned count = 0;
  for(size_t i = 0; i < num; i++) {
    count += buffer[i] == '\n';
  }
  return count;
}

static bool
is_a_c_file(const string_t filename)
{
  return (string_end_with_str_p(filename, ".c")
    || string_end_with_str_p(filename, ".h")
    || string_end_with_str_p(filename, ".cpp")
    || string_end_with_str_p(filename, ".hpp"));
}

static void
scan_file(tree_it_t parent, const string_t filename)
{
  if (!is_a_c_file(filename)) return;

  FILE *f = fopen(string_get_cstr(filename), "rt");
  if (f == NULL) {
    fprintf(stderr, "ERROR: Cannot open %s as a text file.\n", string_get_cstr(filename) );
    exit(1);
  }

  /* Read the FILE per bulk of MAX_READ_BUFFER */
  char buffer[MAX_READ_BUFFER];
  unsigned count = 0;
  size_t num;
  do {
    num = fread (buffer, 1, MAX_READ_BUFFER, f);
    count += count_eol(buffer, num);
  } while (num == MAX_READ_BUFFER);
  fclose(f);

  /* Adding the number of lines to the parent directory */
  dir_t *d = tree_ref(parent);
  atomic_fetch_add( &(*d)->nlines, count );
}

static void
scan_directories(tree_it_t parent, const string_t dirname)
{
  tree_it_t it = add_directory(parent, dirname);

  string_t filename;
  string_init(filename);
  DIR *dir = opendir(string_get_cstr(dirname));
  if (dir == NULL) {
    fprintf(stderr, "ERROR: Cannot open %s as a directory.\n", string_get_cstr(dirname) );
    exit(1);
  }

  // Scan all entries in the directory
  struct dirent *entry = readdir(dir);
  while (entry) {
    // Ignore hidden entries (including '.' and '..')
    if (entry->d_name[0] != '.') {
      // Construct the path to the filename
      string_sets(filename, dirname, "/", entry->d_name);
      // Get the property of this filename
      struct stat buf;
      int err = stat(string_get_cstr(filename), &buf);
      if (err != 0) {
        fprintf(stderr, "ERROR: Cannot stat %s\n", string_get_cstr(filename));
        exit(2);
      }
      // Further scanning in function of the kind of entry
      if (S_ISDIR(buf.st_mode)) {
        scan_directories(it, filename);
      } else {
        scan_file(it, filename);
      }
    }
    // Next entry in the directory
    entry = readdir(dir);
  }
  
  closedir(dir);
  string_clear(filename);
}

static void
consolidate_directories(tree_t directories)
{
  // Scan all entries, first the childreen, then the parent
  for(tree_it_t it = tree_it_post(directories) ; !tree_end_p(it) ; tree_next_post(&it) ) {
    dir_t *parent = tree_up_ref(it);
    dir_t *myself = tree_ref(it);
    if (parent != NULL) {
      // Add the child number of lines to the parent
      atomic_fetch_add_explicit(&(*parent)->nlines, (*myself)->nlines, memory_order_relaxed);
    }
    // Sort the data of the childreen by the number of lines
    tree_sort_child(it);
  }
}

static void
print_result(tree_t directories)
{
  // Scan all entries, first the parent, then the childreen
  for(tree_it_t it = tree_it(directories) ; !tree_end_p(it) ; tree_next(&it) ) {
    int i, depth = tree_depth(it);
    for(i = 0; i < depth; i++) printf("+");
    for(     ; i < 8; i++) printf(" ");
    const dir_t *d = tree_cref(it);
    printf("%6u %s\n", (unsigned) (*d)->nlines, string_get_cstr( (*d)->name ) );
  }
}

int main(int argc, const char *argv[])
{
  tree_t directories;
  string_t str;

  printf("Count the number of C/C++ lines of code\n");
  if (argc < 2) {
    fprintf(stderr, "ERROR. Usage is '%s <directory>'.\n", argv[0]);
    exit(1);
  }

  /* Init the tree, reserve for at least the number of max directory
    and prevent any further increase of allocation:
    this is to ensure that other threads can deref a dir safely */
  tree_init(directories);
  tree_reserve(directories, MAX_DIRECTORY);
  tree_lock(directories, true);
  string_init_set_str(str, argv[1]);
  tree_it_t it = tree_it_end(directories);

  /* Main processing */
  scan_directories(it, str);
  consolidate_directories(directories);
  print_result(directories);

  /* Clear everything & quit */
  string_clear(str);
  tree_clear(directories);
  exit(0);
}
