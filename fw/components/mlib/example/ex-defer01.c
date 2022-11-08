#include "m-core.h"

// Stubs defined for the example. Real code will use soundio library.
struct SoundIo { int x; };
struct SoundIoDevice { int x; };
struct SoundIoOutStream { int x; };

static struct SoundIo *soundio_create(void) { return (struct SoundIo *) malloc(sizeof(struct SoundIo)); }
static void soundio_destroy(struct SoundIo *s) { free(s); }

static struct SoundIoDevice *soundio_get_device(struct SoundIo *io, int def) { (void) io; (void) def; return (struct SoundIoDevice *) malloc(sizeof(struct SoundIoDevice)); }
static void soundio_device_unref(struct SoundIoDevice *s) { free(s); }

static struct SoundIoOutStream *soundio_outstream_create(struct SoundIoDevice *device) { (void) device ; return (struct SoundIoOutStream *) malloc(sizeof(struct SoundIoOutStream)); }
static void soundio_outstream_destroy(struct SoundIoOutStream *s) { free(s); }

static bool soundio_wait_events(struct SoundIo *s) { (void) s; return false; }
// End of stubs

int main(void)
{
  int err = 1;

  /* Example of using M_LET_IF macro to simplify writing error handling code.
     The following code creates some object, test if the object creation succeeds, register the destructor, and print an error if something went wrong
  */
  M_LET_IF( struct SoundIo *soundio = soundio_create(), soundio != 0 , soundio_destroy(soundio) , fprintf(stderr, "out of memory for soundio\n") )
    M_LET_IF(struct SoundIoDevice *device = soundio_get_device(soundio, -1), device != 0, soundio_device_unref(device),  fprintf(stderr, "out of memory for device\n"))
    M_LET_IF(struct SoundIoOutStream *outstream = soundio_outstream_create(device), outstream != 0, soundio_outstream_destroy(outstream), fprintf(stderr, "out of memory for stream\n")) {
    err = 0;
    bool cont = true;
    while (cont)
      cont = soundio_wait_events(soundio);
  }
  return err;
}
