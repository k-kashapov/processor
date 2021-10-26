#include "Info.h"
#include "SyntaxProcessing.h"
#include <stdint.h>

int get_io_args (int argc, const char **argv, config *curr_config)
{
  while (--argc > 0)
  {
    printf("%s\n", *argv);
    argv++;
    printf("%s\n", *argv);

    if (!strncmp (*argv, "-i", 3))
    {
      curr_config->input_file = *(++argv);
      argc--;
    }
    else if (!strncmp (*argv, "-o", 3))
    {
      curr_config->output_file = *(++argv);
      argc--;
    }
  }
  return 0;
}

int ProcessCommand (const char *text, FILE* output, Label_info *labels_arr)
{
  char command[MAX_NAME_LEN] = {};
  int bytes_read = 0;

  printf("addr = %#06lx; text = %*s; ",
          ftell (output), MAX_NAME_LEN * 2, text);

  int scanned = sscanf (text, "%[a-zA-Z0-9]%n", command, &bytes_read);
  if (scanned && text[bytes_read] == ':')
  {
    text += bytes_read;
    ADD_LABEL (command)
    putc ('\n', stdout);
    return 0;
  }

  int cmd_len = 0;
  sscanf (text, "%s%n %n", command, &cmd_len, &bytes_read);
  text += bytes_read;
  printf ("command = %*s; ", MAX_NAME_LEN, command);

  uint64_t command_hash = MurmurHash (command, cmd_len);
  printf ("hash = %lx; ", command_hash);

  #include "commands.h"
  #include "jump.h"
  {
    printf("\nCE: INVALID_SYNTAX command = %s\n", command);
    return INVALID_SYNTAX;
  }

  return 1;
}

int Assemble (file_info *source, FILE *output)
{
  Label labels[MAX_LABELS_NUM] = {};

  Label_info labels_arr = {};
  labels_arr.labels = labels;
  labels_arr.num = 0;

  for (int printed_len = 0; printed_len < sizeof (Header_t); printed_len++)
  {
    fputc (0, output);
  }

  int char_num = 0;
  int last_cmd_len = 0;

  for (int ip = 0; ip < source->lines_num; ip++)
  {
      last_cmd_len = ProcessCommand
      (
        source->strs[ip]->text, output, &labels_arr
      );

      if (last_cmd_len < 0)
      {
        printf ("\n###############\
                 \nCOMPILATION ERROR: invalid command at ip %d\n", ip + 1);
        return INVALID_SYNTAX;
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
