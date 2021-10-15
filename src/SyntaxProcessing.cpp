#include "Info.h"
#include "SyntaxProcessing.h"
#include <stdint.h>
#include "enum.h"

int ProcessCommand (const char *command, uint64_t val, FILE* output)
{
  printf("addr = %#06x; command = %*s; value = %012.3lf; bytes = ", ftell (output), MAX_NAME_LEN, command, (double)val / 1000);

  char code = -1;

  if (!strcmp (command, "push"))
  {
    fputc (PUSH_CMD, output);
    printf("%02X ", PUSH_CMD);

    for (int i = 0; i < sizeof (type_t); i++)
    {
      unsigned char val_byte = (unsigned char) val;
      fputc (val_byte, output);
      printf ("%02X ", val_byte);

      val >>= 8;
    }
    printf("\n");
    return 1 + sizeof (type_t);
  }
  else DEF_CMD (in)
  else DEF_CMD (pop)
  else DEF_CMD (add)
  else DEF_CMD (sub)
  else DEF_CMD (mul)
  else DEF_CMD (div)
  else DEF_CMD (out)
  else if (!strcmp (command, "hlt"))
  {
    fputc (HLT_CMD, output);
    printf("%02x HALT\n", HLT_CMD);
    return 0;
  }
  else return INVALID_SYNTAX;

  fputc (code, output);
  printf ("%02X\n", code);

  return 1;
}

int Compile (file_info *source, FILE *output)
{
  for (int printed_len = 0; printed_len < sizeof (Header_t); printed_len++)
  {
    fputc (0, output);
  }

  int char_num = 0;

  for (int ip = 0; ip < source->lines_num; ip++)
  {
      char command[MAX_NAME_LEN] = {};
      double fval = 0.0;

      sscanf (source->strs[ip]->text, "%s %lf", command, &fval);

      uint64_t val = (uint64_t) (fval * 1000);

      int command_len = ProcessCommand (command, val, output);

      if (command_len < 0)
      {
        printf ("\n###############\
                 \nCOMPILATION ERROR: invalid command: %s ### with value = %lu ### at ip %d\n",
                command, val, ip + 1);
        return INVALID_SYNTAX;
      }
      else if (command_len == 0) // hlt foundd
      {
        char_num ++;
        break;
      }

      char_num += command_len;
  }

  Header_t header;
  header.char_num = char_num;
  char *header_ptr = (char *)&header;

  fseek (output, SEEK_SET, 0);
  for (int i = 0; i < sizeof (header); i++) {
    fputc (*header_ptr, output);
    header_ptr++;
  }

  printf("bytes = %d\n", char_num);

  return 0;
}
