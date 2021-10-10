#include "Info.h"
#include "SyntaxProcessing.h"
#include "stdint.h"

int ProcessCommand (const char *command, uint64_t val, FILE* output)
{
  printf("addr = %#06x; command = %*s; value = %06lu; bytes = ", ftell (output), MAX_NAME_LEN, command, val);

  char code = -1;

  if (!strcmp (command, "push"))
  {
    fputc (PUSH_CODE, output);
    printf("%02X ", PUSH_CODE);

    for (int i = 0; i < sizeof (int); i++)
    {
      unsigned char val_byte = (unsigned char) val;
      fputc (val_byte, output);
      printf ("%02X ", val_byte);

      val >>= 8;
    }
    printf("\n");
    return 5;
  }
  else if (!strcmp (command, "in"))  code = IN_CODE;
  else if (!strcmp (command, "pop")) code = POP_CODE;
  else if (!strcmp (command, "add")) code = ADD_CODE;
  else if (!strcmp (command, "sub")) code = SUB_CODE;
  else if (!strcmp (command, "mul")) code = MUL_CODE;
  else if (!strcmp (command, "div")) code = DIV_CODE;
  else if (!strcmp (command, "out")) code = OUT_CODE;
  else if (!strcmp (command, "hlt"))
  {
    fputc (HLT_CODE, output);
    printf("%02x HALT\n", HLT_CODE);
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

  for (int line = 0; line < source->lines_num; line++)
  {
      char command[MAX_NAME_LEN] = {};
      double fval = 0.0;

      sscanf (source->strs[line]->text, "%s %lf", command, &fval);

      uint64_t val = (uint64_t) (fval * 1000);

      int command_len = ProcessCommand (command, val, output);

      if (command_len < 0)
      {
        printf ("\n###############\
                 \nCOMPILATION ERROR: invalid command: %s ### with value = %lu ### at line %d\n",
                command, val, line + 1);
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
