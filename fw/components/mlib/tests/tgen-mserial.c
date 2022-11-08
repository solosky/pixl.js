#include "m-serial-json.h"
#include "m-tuple.h"
#include "m-string.h"

TUPLE_DEF2(entry,
           (x, float),
           (y, float),
           (name, string_t) )

void save(FILE *f, const entry_t ent)
{
  m_serial_write_t serial;
  m_serial_json_write_init(serial, f);
  entry_out_serial(serial, ent);
  m_serial_json_write_clear(serial);
}

