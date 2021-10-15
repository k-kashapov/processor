#include "files.h"
#include "Processor.h"
#include "Info.h"

int main(int argc, const char** argv)
{
    FILE *code = fopen ("code.dead", "rb");
    assert (code);

    char *bytes = read_to_end (code);

    Header_t header = *((Header_t *)bytes);
    #ifdef PROC_DUMP
      printf ("sign = %X\n", header.signature);
      printf ("version = %x\n", header.version);
      printf ("bytes = %d\n", header.char_num);
    #endif

    if (header.signature != 'KEEK')
    {
      printf ("###########################\n");
      printf ("FATAL: Invalid file\n");
      printf ("###########################\n");
      return INVALID_FILE;
    }

    bytes += sizeof (Header_t);

    int runtime_err = run_binary (bytes, header.char_num);
    if (runtime_err)
      return runtime_err;

    return 0;
}
