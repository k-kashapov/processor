#pragma once

#include "Stack.h"
#include "enum.h"
#include "files.h"

const int MAX_NAME_LEN   = 8;
const int MAX_LABELS_NUM = 20;

enum COMPILATION_ERRORS
{
  INVALID_SYNTAX   = -1,
  WRITE_FALIED     = -2,
  INVALID_ARG      = -3,
  LABEL_DUPLICATE  = -4,
};

enum MASKS
{
  MASK_IMM   = 0x40,
  MASK_REG   = 0x20,
  MASK_I2RAM = 0xC0,
  MASK_R2RAM = 0xA0,
};

struct Label
{
  uint64_t name_hash;
  type_t ip;
};

struct Label_info
{
  Label *labels;
  int num;
};

#define FPUT(ch)                                                                \
  int printed = fputc (ch, output);                                             \
  if (printed == EOF)                                                           \
  {                                                                             \
    printf("\nCE: WRITE TO FILE FAILED command = %s\n",                         \
    command);                                                                   \
    return WRITE_FALIED;                                                        \
  }

#define FPRINT_BYTES(val)                                                       \
  for (int i = 0; i < sizeof (val); i++)                                        \
  {                                                                             \
    unsigned char val_byte = (unsigned char) val;                               \
    FPUT (val_byte);                                                            \
    printf ("%02X ", val_byte);                                                 \
                                                                                \
    val >>= 8;                                                                  \
  }

#define GET_MASK(format, tmp_var, new_mask)                                     \
  if (sscanf (text, format, tmp_var))                                           \
  {                                                                             \
    mask = new_mask;                                                            \
    format_str = format;                                                        \
  }                                                                             \
  else

#define ADD_LABEL(cmd)                                                          \
  uint64_t cmd_hash = MurmurHash (cmd, bytes_read);                             \
  printf ("command = %*s; hash = %lx; len = %d; ",                              \
          MAX_NAME_LEN, cmd, cmd_hash, bytes_read);                             \
  for (int iter = 0; iter < labels_arr->num; iter++)                            \
  {                                                                             \
    if (labels_arr->labels[iter].name_hash == cmd_hash)                         \
    {                                                                           \
      printf("CE: LABEL DUPLICATE: %s\n", cmd);                                 \
      return LABEL_DUPLICATE;                                                   \
    }                                                                           \
  }                                                                             \
  labels_arr->labels[labels_arr->num].name_hash = cmd_hash;                     \
  labels_arr->labels[labels_arr->num].ip = ftell(output);                       \
  printf("ip = %lx;\n", labels_arr->labels[labels_arr->num].ip);                \
  labels_arr->num++;

#define DEF_CMD(num, cmd, argc, code, hash)                                     \
  if (hash == command_hash)                                                     \
  {                                                                             \
    int tmp_ch = 0;                                                             \
    double tmp_lf = 0;                                                          \
    char mask = 0x00;                                                           \
    const char *format_str = "";                                                \
                                                                                \
    GET_MASK (      "[%d]",         &tmp_ch, MASK_I2RAM)                        \
    GET_MASK ("[%1[a-d]x]", (char *)&tmp_ch, MASK_R2RAM)                        \
    GET_MASK (  "%1[a-d]x", (char *)&tmp_ch, MASK_REG)                          \
    GET_MASK (       "%lf",         &tmp_lf, MASK_IMM)                          \
    {                                                                           \
      printf("\nCE: IVALID ARGUMENT command = %s\n", command);                  \
      return INVALID_ARG;                                                       \
    }                                                                           \
    FPUT (CMD_##cmd | mask);                                                    \
    printf ("code = %02X; ", (unsigned char)(CMD_##cmd | mask));                \
                                                                                \
    int bytes_written = 1;                                                      \
                                                                                \
    for (int arg = 0; arg < argc; arg++)                                        \
    {                                                                           \
      int bytes_read = 0;                                                       \
                                                                                \
      if (mask == MASK_IMM)                                                     \
      {                                                                         \
        double argv = 0;                                                        \
        sscanf (text, format_str, &argv, &bytes_read);                          \
        printf("value = %06.3lf; bytes = ", argv);                              \
        type_t value = (type_t)(argv * 1000);                                   \
        FPRINT_BYTES (value);                                                   \
        bytes_written += sizeof(type_t);                                        \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        char argv = 0;                                                          \
        sscanf (text, format_str, &argv, &bytes_read);                          \
        printf("arg = %d; bytes = ", argv);                                     \
        FPRINT_BYTES (argv);                                                    \
        bytes_written += 1;                                                     \
      }                                                                         \
      text += bytes_read;                                                       \
    }                                                                           \
    printf("\n");                                                               \
    return bytes_written;                                                       \
  }                                                                             \
  else

#define DEF_JMP_CMD(code, name, hash)                                           \
    if (command_hash == hash)                                                   \
    {                                                                           \
      FPUT (code);                                                              \
      printf ("bytes = %02X ", code);                                           \
                                                                                \
      char jmp_arg[MAX_NAME_LEN] = {};                                          \
      int arg_len = 0;                                                          \
      sscanf (text, ":%[a-zA-Z0-9]%n", jmp_arg, &arg_len);                      \
      uint64_t arg_hash = MurmurHash (jmp_arg, arg_len - 1);                    \
                                                                                \
      for (int iter = 0; iter < labels_arr->num; iter++)                        \
      {                                                                         \
        if (labels_arr->labels[iter].name_hash == arg_hash)                     \
        {                                                                       \
          FPRINT_BYTES (labels_arr->labels[iter].ip);                           \
          putc ('\n', stdout);                                                  \
          return sizeof (type_t);                                               \
        }                                                                       \
      }                                                                         \
      type_t dummy = 0;                                                         \
      FPRINT_BYTES (dummy);                                                     \
      putc ('\n', stdout);                                                      \
      return sizeof (type_t);                                                   \
    }                                                                           \
    else                                                                        \

int get_io_args (int argc, const char **argv, config *curr_config);

int ProcessCommand (const char *text, FILE* output, Label_info *labels_arr);

int Assemble (file_info *source, FILE *output);
