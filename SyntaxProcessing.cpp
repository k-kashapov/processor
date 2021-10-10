#include "Info.h"
#include "SyntaxProcessing.h"

int ProcessCommand (const char *command, int val, FILE* output)
{
  if (!strcmp (command, "push"))
  {
    fputc (PUSH_CODE, output);

    for (int i = 0; i < sizeof (int); i++)
    {
      fputc (val, output);
      val >>= 8;
    }
    return 5;
  }
  else if (!strcmp (command, "in"))  fputc (IN_CODE,  output);
  else if (!strcmp (command, "pop")) fputc (POP_CODE, output);
  else if (!strcmp (command, "add")) fputc (ADD_CODE, output);
  else if (!strcmp (command, "sub")) fputc (SUB_CODE, output);
  else if (!strcmp (command, "mul")) fputc (MUL_CODE, output);
  else if (!strcmp (command, "div")) fputc (DIV_CODE, output);
  else if (!strcmp (command, "out")) fputc (OUT_CODE, output);
  else if (!strcmp (command, "hlt"))
  {
    fputc (HLT_CODE, output);
    return 0;
  }
  else return -1;

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
      int val = 0;

      sscanf (source->strs[line]->text, "%s %d", command, &val);

      int command_len = ProcessCommand (command, val, output);

      if (command_len == 0)
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

  printf("chars = %d\n", char_num);

  return 0;
}
