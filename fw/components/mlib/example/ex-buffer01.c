#include <stdio.h>

#include "m-buffer.h"
#include "m-shared.h"
#include "m-mutex.h"

/* Global variable to stop scheduling */
atomic_bool             continue_threading_g;

/**************************************************************/

/* Define a custom random big data structure 
   with its constructor and its destructor.
*/

#define BIGDATA_SIZE 1000
#define BIGDATA_NUM 10
#define BIGDATA_HEIGHT 2048
#define BIGDATA_WIDTH  4096

typedef struct bigdata_s {
  char bigdata[BIGDATA_SIZE];
  unsigned char *ptr;
  int count;
} bigdata_t[1];

int count_g = 1;

static void bigdata_init(bigdata_t d)
{
  memset(d->bigdata, 0, BIGDATA_SIZE);
  d->ptr = malloc (BIGDATA_HEIGHT*BIGDATA_WIDTH);
  if (!d->ptr) abort();
  d->count = count_g++;
  printf ("Init image %d\n", d->count);
}

static void bigdata_clear(bigdata_t d)
{
  printf ("Clear image %d\n", d->count);
  free(d->ptr);
}

/* Let's register the oplist of our custom type globally */
#define M_OPL_bigdata_t() (INIT(bigdata_init), CLEAR(bigdata_clear))


/**************************************************************/

/* Perform some random works on this custom type */

static void perform_acquisition(bigdata_t p)
{
  printf ("Acquiring image %d\n", p->count);
  // Let's say that rand is our acquisition function.
  for(int i = 0 ; i < BIGDATA_SIZE;i++)
    p->bigdata[i] = rand();
  for(int i = 0 ; i < BIGDATA_WIDTH*BIGDATA_HEIGHT;i++)
    p->ptr[i] = rand();
  // Let's stop arbitrary after 20 acquisitions
  if (p->count == 20)
    atomic_store(&continue_threading_g, false);
}

static void perform_computation2(bigdata_t p)
{
  printf ("Computation 2 on image %d\n", p->count);
  for(int i = 1 ; i < BIGDATA_WIDTH*BIGDATA_HEIGHT-1;i++)
    p->ptr[i] = (p->ptr[i-1] + p->ptr[i] + p->ptr[i+1] ) / 3;
}

static void perform_computation3(bigdata_t p)
{
  printf ("Computation 3 on image %d\n", p->count);
  for(int i = 1 ; i < BIGDATA_SIZE-1;i++)
    p->bigdata[i] = (p->bigdata[i-1] + p->bigdata[i] + p->bigdata[i+1] ) / 3;
}

static void perform_computation4(bigdata_t p)
{
  printf ("Reduction on image %d\n", p->count);
  unsigned long s = 0;
  for(int i = 0 ; i < BIGDATA_WIDTH*BIGDATA_HEIGHT;i++)
    s += p->ptr[i];
  for(int i = 0 ; i < BIGDATA_SIZE;i++)
    s += p->ptr[i];
  printf ("Result of computation 4 = %lu\n", s);
}

/**************************************************************/

/* Let's define a shared pointer interface around the bigdata structure */
SHARED_PTR_DEF(shared_bigdata, bigdata_t)

/* Let's define the communication queue between the thread:
 * - size of queue is 10,
 * - it is a queue BUFFER_QUEUE,
 * - it store shared pointer to our bigdata_t,
 * - we change the semantics of _push / _pop into buffer as push a new item and pop it as a move (ideal semantics for shared_ptr interface).
 */
#define MY_QUEUE_SIZE 10
BUFFER_DEF(buffer_bigdata, shared_bigdata_t, MY_QUEUE_SIZE,
           BUFFER_QUEUE|BUFFER_PUSH_INIT_POP_MOVE, SHARED_PTR_OPLIST(shared_bigdata))

/* Let's build a Thread synchros tree:
 *
 * Thread1 perform the simulated acquisition of the data.
 * Thread2 & Thread3 perform independent computation.
 * Thread4 finalizes the computation :
 *
 * Thread1 ---> Thread 2 +
 *          +-> Thread 3 --> Thread 4
 */
buffer_bigdata_t buf_t1tot2;
buffer_bigdata_t buf_t1tot3;
buffer_bigdata_t buf_t3tot4;
m_thread_t       t1, t2, t3, t4;

static void initbuf(void)
{
  buffer_bigdata_init(buf_t1tot2, MY_QUEUE_SIZE);
  buffer_bigdata_init(buf_t1tot3, MY_QUEUE_SIZE);
  buffer_bigdata_init(buf_t3tot4, MY_QUEUE_SIZE);
}

static void clearbuf(void)
{
  buffer_bigdata_clear(buf_t1tot2);
  buffer_bigdata_clear(buf_t1tot3);
  buffer_bigdata_clear(buf_t3tot4);
}

static void thread1 (void *arg)
{
  (void) arg;
  while (atomic_load(&continue_threading_g)) {
    shared_bigdata_t ptr;
    shared_bigdata_init_new (ptr);
    perform_acquisition(*shared_bigdata_ref(ptr));
    buffer_bigdata_push(buf_t1tot2, ptr);
    buffer_bigdata_push(buf_t1tot3, ptr);
    shared_bigdata_clear(ptr);
  }
}
static void thread2 (void *arg)
{
  (void) arg;
  while (atomic_load(&continue_threading_g)) {
    shared_bigdata_t ptr;
    buffer_bigdata_pop(&ptr, buf_t1tot2); // NOTE: Pop has been configured to init ptr
    perform_computation2(*shared_bigdata_ref(ptr));
    shared_bigdata_clear(ptr);
  }
}
static void thread3 (void *arg)
{
  (void) arg;
  while (atomic_load(&continue_threading_g)) {
    shared_bigdata_t ptr;
    buffer_bigdata_pop(&ptr, buf_t1tot3);  // NOTE: Pop has been configured to init ptr
    perform_computation3(*shared_bigdata_ref(ptr));
    buffer_bigdata_push(buf_t3tot4, ptr);
    shared_bigdata_clear(ptr);
  }
}
static void thread4 (void *arg)
{
  (void) arg;
  while (atomic_load(&continue_threading_g)) {
    shared_bigdata_t ptr;
    buffer_bigdata_pop(&ptr, buf_t3tot4); // NOTE: Pop has been configured to init ptr
    perform_computation4(*shared_bigdata_ref(ptr));
    shared_bigdata_clear(ptr);
  }
}

/**************************************************************/

static void init(void)
{
  initbuf();
  atomic_init(&continue_threading_g, true);
  m_thread_create(t1, thread1, NULL);
  m_thread_create(t2, thread2, NULL);
  m_thread_create(t3, thread3, NULL);
  m_thread_create(t4, thread4, NULL);
}

static void clear(void)
{
  m_thread_join(t1);
  m_thread_join(t2);
  m_thread_join(t3);
  m_thread_join(t4);
  clearbuf();
}

/**************************************************************/

int main(void)
{
  init();
  clear();
}
