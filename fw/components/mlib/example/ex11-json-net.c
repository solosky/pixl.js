// Based on https://github.com/nanopb/nanopb/blob/master/examples/simple/simple.c
#include <stdio.h>

#include "m-serial-json.h"
#include "m-tuple.h"
#include "m-string.h"

// Define a structure with one field
TUPLE_DEF2(SimpleMessage,
           (lucky_number, int))

int main(void)
{
  string_t buffer;
  m_serial_return_code_t status;

  string_init(buffer);
  /* Encode our message */
  {
    SimpleMessage_t message;
    SimpleMessage_init(message);
    
    /* Create a stream that will write to our buffer. */
    m_serial_str_json_write_t stream;
    m_serial_str_json_write_init(stream, buffer);
        
    /* Fill in the lucky number */
    message->lucky_number = 13;
        
    /* Now we are ready to encode the message! */
    status = SimpleMessage_out_serial(stream, message);
    
    /* Then just check for any errors.. */
    if (status != M_SERIAL_OK_DONE)
      {
        printf("Encoding failed: %d\n", status);
        return 1;
      }

    /* Clear the objets */
    m_serial_str_json_write_clear(stream);
    SimpleMessage_clear(message);
  }
  
  /* Now we could transmit the message over network, store it in a file
   * but we are doing it right now.
   */
  
  {
    /* Allocate another space for the decoded message. */
    SimpleMessage_t message;
    SimpleMessage_init(message);
    
    /* Create a stream that reads from the buffer. */
    m_serial_str_json_read_t stream;
    m_serial_str_json_read_init(stream, string_get_cstr(buffer));
    
    /* Now we are ready to decode the message. */
    status = SimpleMessage_in_serial(message, stream);
    
    /* Check for errors... */
    if (status != M_SERIAL_OK_DONE)
      {
        printf("Decoding failed: %d\n", status);
        return 1;
      }
    
    /* Print the data contained in the message. */
    printf("Your lucky number was %d!\n", message->lucky_number);

    /* Clear the objets */
    m_serial_str_json_read_clear(stream);
    SimpleMessage_clear(message);
  }

  string_clear(buffer);
  
  return 0;
}
