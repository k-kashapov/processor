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

int ProcessCommand (const char *text, ARRAYS)
{
  char command[MAX_NAME_LEN] = {};
  int bytes_read = 0;
  int max_line = *lines;

  long unsigned int curr_ip = binary - binary_arr;

  printf("\naddr = %#06lx; text = %*s; ",
          curr_ip - sizeof (Header_t), MAX_NAME_LEN * 2, text);

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

int Assemble (file_info *source, char *binary_arr)
{
  Label labels[MAX_LABELS_NUM] = {};
  Jump   jumps[MAX_LABELS_NUM] = {};

  type_t *lines = (type_t *) calloc (source->lines_num + 1, sizeof (type_t));

  char *binary = binary_arr + sizeof (Header_t);

  lines[0] = source->lines_num;

  printf ("lines num = %d\n", source->lines_num);

  JL_info jl_arr = {};
  jl_arr.labels = labels;
  jl_arr.jumps = jumps;

  int char_num = 0;
  int last_cmd_len = 0;

  for (int ip = 0; ip < source->lines_num; ip++)
  {
    lines[ip + 1] = binary - binary_arr;
    last_cmd_len = ProcessCommand
    (
      source->strs[ip]->text, binary, binary_arr, &jl_arr, lines
    );

    if (last_cmd_len < 0)
    {
      printf ("\n###############\
               \nCOMPILATION ERROR: invalid command at line %d\n", ip + 1);
      return INVALID_SYNTAX;
    }

    binary += last_cmd_len;
    char_num += last_cmd_len;
  }

  Header_t header;
  header.char_num = char_num;

  printf ("Header:\n");

  binary = binary_arr;
  SPRINT_BYTES (header);


  Link (binary, binary_arr, lines, &jl_arr);
  printf("bytes total = %d\n", char_num);

  free (lines);

  return char_num + sizeof (Header_t);
}

int Link (char *binary, char *binary_arr, type_t *lines, JL_info *jl_arr)
{
  printf ("Linking: \n");

  for (int jmp = 0; jmp < jl_arr->jmp_num; jmp++)
  {
    int destination = jl_arr->jumps[jmp].destination;

    if (destination > 0)
    {
      printf ("LINE JMP to %x; dest ip = %lx; ", destination, lines[destination]);

      binary = binary_arr + (GET_JMP_IP (jmp) + 1);
      printf("print at %#06lx\nbytes = ", binary - binary_arr);

      SPRINT_BYTES (lines[destination]);
      continue;
    }

    int lbl = 0;

    for (lbl = 0; lbl < jl_arr->lbl_num; lbl++)
    {
      if (GET_JMP_HASH (jmp) == GET_LABEL_HASH (lbl))
      {
        printf ("HASH FOUND: JMP to %lx; dest ip = %lx; ",
                GET_LABEL_IP (lbl), GET_JMP_IP (jmp) + 1);

        binary = binary_arr + (GET_JMP_IP (jmp) + 1);

        printf("print at %#06lx\nbytes = ", binary - binary_arr);
        SPRINT_BYTES (GET_LABEL_IP (lbl));
        break;
      }
    }

    if (lbl >= jl_arr->lbl_num)
    {
      printf ("\nCE: STRAY JUMP");
      return STRAY_JUMP;
    }
  }

  printf("Linking Complete\n");
  return 0;
}
