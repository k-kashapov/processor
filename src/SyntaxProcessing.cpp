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

long get_args (const char *text, char cmd, int argc, BinaryInfo *bin_info)
{
  type_t tmp_int = 0;
  double tmp_lf = 0;
  unsigned char mask = 0x00;
  int arg_scanned = 0;
  if (sscanf (text, "[%1[a-z]x + %ld]", (char *)&tmp_int, &tmp_int) == 2)
  {
    mask = MASK_S2RAM;
  }
  else
  GET_MASK (     "[%ld]",         &tmp_int, MASK_I2RAM)
  GET_MASK (       "%lf",          &tmp_lf,   MASK_IMM)
  GET_MASK ("[%1[a-z]x]", (char *)&tmp_int, MASK_R2RAM)
  GET_MASK (  "%1[a-z]x", (char *)&tmp_int,   MASK_REG)
  {
    arg_scanned = EOF;
  }
  if (arg_scanned == EOF && argc > 0)
  {
    printf("\nCE: INVALID ARGUMENT command = %d\n", cmd);
    return INVALID_ARG;
  }
  SPUT (cmd | mask);
  printf ("code  = %02X;\n", (unsigned char)(cmd | mask));

  long bytes_written = 0;

  for (int arg = 0; arg < argc; arg++)
  {
    int arg_bytes_read = 0;
    if (mask == MASK_S2RAM)
    {
      char regv = 0;
      type_t offset = 0;
      arg_scanned = sscanf (text, "[%1[a-z]x + %ld]", &regv, &offset);
      if (!arg_scanned)
      {
        printf ("INVALID_ARG\n");
        return INVALID_ARG;
      }
      printf ("args = %d and %ld; bytes = ", regv, offset);
      SPRINT_BYTES (regv);
      bin_info->binary += 1;
      bytes_written += 1;
      SPRINT_BYTES (offset);
      bytes_written += (long) sizeof(type_t);
    }
    else if (mask == MASK_I2RAM)
    {
      type_t argv = 0;
      arg_scanned = sscanf (text, "[%ld]", &argv);
      if (!arg_scanned)
      {
        printf ("INVALID_ARG\n");
        return INVALID_ARG;
      }
      printf ("value = %ld; bytes = ", argv);
      type_t value = (argv * 1000);
      SPRINT_BYTES (value);
      bytes_written += (long) sizeof(type_t);
    }
    else if (mask == MASK_IMM)
    {
      double argv = 0;
      arg_scanned = sscanf (text, "%lf", &argv);
      if (!arg_scanned)
      {
        printf ("INVALID_ARG\n");
        return INVALID_ARG;
      }
      printf ("value = %06.3lf; bytes = ", argv);
      type_t value = (type_t)(argv * 1000);
      SPRINT_BYTES (value);
      bytes_written += (long) sizeof(type_t);
    }
    else if (mask == MASK_R2RAM)
    {
      int argv = 0;
      arg_scanned = sscanf (text, "[%1[a-z]x]", (char *)&argv);
      if (!arg_scanned)
      {
        printf ("INVALID_ARG\n");
        return INVALID_ARG;
      }
      printf ("value = %d; bytes = ", argv);
      type_t value = (type_t)(argv * 1000);
      SPRINT_BYTES (value);
      bytes_written += (long) sizeof(type_t);
    }
    else
    {
      int argv = 0;
      arg_scanned = sscanf (text, "%1[a-z]x", (char *)&argv);
      if (!arg_scanned)
      {
        printf ("INVALID_ARG\n");
        return INVALID_ARG;
      }
      printf ("arg = %d; bytes = ", argv);
      SPRINT_BYTES (argv);
      bytes_written += 1;
    }
    text += arg_bytes_read;
  }
  return bytes_written;
}

long get_jmp_args (const char *text, char cmd, BinaryInfo *bin_info, unsigned long curr_ip, long long int max_line)
{
  printf ("code  = %02X; bytes = \n", (unsigned int)cmd);
  SPUT (cmd);

  char jmp_arg[MAX_NAME_LEN] = {};
  int arg_len = 0;
  int scanned = sscanf (text, ": %[a-zA-Z0-9]%n", jmp_arg, &arg_len);
  type_t dummy = 0;
  if (!scanned)
  {
    int dest_line = 0;
    scanned = sscanf (text, "%d", &dest_line);
    if (!scanned)
    {
      printf("\nCE: IVALID ARGUMENT command = %d\n", cmd);
      return INVALID_ARG;
    }
    if (dest_line > max_line)
    {
      printf("\nCE: LINE NUMBER IS TOO BIG command = %d\n", cmd);
      return INVALID_ARG;
    }
    bin_info->jumps[bin_info->jmp_num].destination = dest_line;
    GET_JMP_IP (bin_info->jmp_num) = (type_t) curr_ip;
    bin_info->jmp_num++;
    SPRINT_BYTES (dummy);
    return sizeof (type_t);
  }
  uint64_t arg_hash = MurmurHash (jmp_arg, (unsigned long) arg_len - 1);

  for (unsigned long lbl_iter = 0; lbl_iter < bin_info->lbl_num; lbl_iter++)
  {
    if (GET_LABEL_HASH (lbl_iter) == arg_hash)
    {
      type_t dest_ip = GET_LABEL_IP (lbl_iter);
      SPRINT_BYTES (dest_ip);
      putc ('\n', stdout);
      return sizeof (type_t);
    }
  }
  SPRINT_BYTES (dummy);
  GET_JMP_IP (bin_info->jmp_num) = (type_t) curr_ip;
  GET_JMP_HASH (bin_info->jmp_num) = arg_hash;
  bin_info->jmp_num++;
  return sizeof (type_t);
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
