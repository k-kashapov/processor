#include "Info.h"
#include "SyntaxProcessing.h"
#include "Stack.h"
#include <stdint.h>

int ProcessCommand (const char *text, FILE* output)
{
  char command[MAX_NAME_LEN] = {};
  int bytes_read = 0;

  sscanf (text, "%s %n", command, &bytes_read);
  text += bytes_read;

  printf("addr = %#06lX; command = %*s; ",
    ftell (output), MAX_NAME_LEN, command);

  #include "commands.h"
  {
    printf("\nCE: INVALID_SYNTAX command = %s\n", command);
    return INVALID_SYNTAX;
  }

  return 1;
}

int Assemble (file_info *source, FILE *output)
{
  for (int printed_len = 0; printed_len < sizeof (Header_t); printed_len++)
  {
    fputc (0, output);
  }

  int char_num = 0;
  int last_cmd_len = 0;

  for (int ip = 0; ip < source->lines_num; ip++)
  {
      last_cmd_len = ProcessCommand (source->strs[ip]->text, output);

      if (last_cmd_len < 0)
      {
        printf ("\n###############\
                 \nCOMPILATION ERROR: invalid command at ip %d\n", ip + 1);
        return INVALID_SYNTAX;
      }
      else if (last_cmd_len == 0) // hlt foundd
      {
        char_num ++;
        break;
      }
      char_num += last_cmd_len;
  }

  fputc (CMD_hlt, output);
  char_num++;

  Header_t header;
  header.char_num = char_num;
  char *header_ptr = (char *)&header;

  fseek (output, SEEK_SET, 0);
  for (int i = 0; i < sizeof (header); i++) {
    fputc (*header_ptr, output);
    header_ptr++;
  }

  printf("bytes total = %d\n", char_num);

  return 0;
}
