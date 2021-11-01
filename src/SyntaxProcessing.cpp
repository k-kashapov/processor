#include "Info.h"
#include "SyntaxProcessing.h"
#include <stdint.h>

int get_io_args (int argc, const char **argv, Config *curr_config)
{
  while (--argc > 0)
  {
    argv++;

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

long ProcessCommand (const char *text, BinaryInfo *bin_info)
{
  char command[MAX_NAME_LEN] = {};
  int bytes_read = 0;
  type_t max_line = *bin_info->lines;

  long unsigned int curr_ip = (unsigned long) (bin_info->binary - bin_info->binary_arr);

  printf("\naddr = %#06lx; text = %*s; ",
          curr_ip - sizeof (Header_t), MAX_NAME_LEN, text);

  int scanned = sscanf (text, "%[a-zA-Z0-9]%n", command, &bytes_read);
  if (scanned && text[bytes_read] == ':')
  {
    text += bytes_read;
    ADD_LABEL (command)
    return 0;
  }

  int cmd_len = 0;
  sscanf (text, "%s%n %n", command, &cmd_len, &bytes_read);
  text += bytes_read;

  if (*command == ';') return 0;

  printf ("command = %*s; ", MAX_NAME_LEN / 2, command);

  uint64_t command_hash = MurmurHash (command, (unsigned long) cmd_len);
  printf ("hash = %lx; cmd len = %d", command_hash, cmd_len);

  switch (command_hash)
  {
    #include "commands.h"
    #include "jump.h"
    default:
      printf("\nCE: INVALID_SYNTAX command = %s\n", command);
      return INVALID_SYNTAX;
  }

  return 1;
}

long Assemble (File_info *source, unsigned char *binary_arr)
{
  Label labels[MAX_LABELS_NUM] = {};
  Jump   jumps[MAX_LABELS_NUM] = {};

  type_t *lines = (type_t *) calloc ((size_t) source->lines_num + 1, sizeof (type_t));

  lines[0] = source->lines_num;

  printf ("lines num = %ld\n", source->lines_num);

  BinaryInfo bin_info = {};
  bin_info.labels = labels;
  bin_info.jumps = jumps;
  bin_info.binary = binary_arr + sizeof (Header_t);
  bin_info.binary_arr = binary_arr;
  bin_info.lines = lines;

  long char_num = 0;
  long last_cmd_len = 0;

  for (int ip = 0; ip < source->lines_num; ip++)
  {
    lines[ip + 1] = bin_info.binary - binary_arr;
    last_cmd_len = ProcessCommand (source->strs[ip]->text, &bin_info);

    if (last_cmd_len < 0)
    {
      printf ("\n###############\
               \nCOMPILATION ERROR: invalid command at line %d\n", ip + 1);
      return INVALID_SYNTAX;
    }
    printf ("bytes written = %ld\n", last_cmd_len + 1);
    bin_info.binary += last_cmd_len;
    char_num += last_cmd_len + 1;
  }

  Header_t header;
  header.char_num = (unsigned long) char_num;

  printf ("Header:\n");

  *(Header_t *)binary_arr = header;

  int linking_err = Link (&bin_info);
  if (linking_err) return 0;
  printf("bytes total = %ld\n", char_num);

  free (lines);

  return char_num + (long) sizeof (Header_t);
}

int Link (BinaryInfo *bin_info)
{
  printf ("Linking: \n");

  for (unsigned long jmp = 0; jmp < bin_info->jmp_num; jmp++)
  {
    unsigned int destination = (unsigned int) bin_info->jumps[jmp].destination;

    if (destination > 0)
    {
      printf ("LINE JMP to %x; dest ip = %lx; ",
              destination, (unsigned long) bin_info->lines[destination]);

      bin_info->binary = bin_info->binary_arr + (GET_JMP_IP (jmp) + 1);
      printf("print at %#06lx\nbytes = ",
              (unsigned long) (bin_info->binary - bin_info->binary_arr));

      SPRINT_BYTES (bin_info->lines[destination]);
      continue;
    }

    unsigned long lbl = 0;

    for (lbl = 0; lbl < bin_info->lbl_num; lbl++)
    {
      if (GET_JMP_HASH (jmp) == GET_LABEL_HASH (lbl))
      {
        printf ("HASH FOUND: JMP to %lx; dest ip = %lx; ",
                (unsigned long) (GET_LABEL_IP (lbl)), (unsigned long) (GET_JMP_IP (jmp) + 1));

        bin_info->binary = bin_info->binary_arr + (GET_JMP_IP (jmp) + 1);

        printf("print at %#06lx\nbytes = ",
                (unsigned long) (bin_info->binary - bin_info->binary_arr));

        SPRINT_BYTES (GET_LABEL_IP (lbl));
        break;
      }
    }

    if (lbl >= bin_info->lbl_num)
    {
      printf ("\nCE: STRAY JUMP\n");
      return STRAY_JUMP;
    }
  }

  printf("Linking Complete\n");
  return 0;
}
