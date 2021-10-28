#pragma once

#include "Stack.h"
#include "enum.h"
#include "files.h"

const int MAX_NAME_LEN   = 8;
const int MAX_LABELS_NUM = 32;

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

struct Jump
{
  uint64_t name_hash;
  type_t ip;
  int destination;
};

struct JL_info
{
  Label *labels;
  Jump *jumps;
  int jmp_num;
  int lbl_num;
};

#define GET_LABEL_HASH(addr) jl_arr->labels[addr].name_hash
#define GET_LABEL_IP(addr) jl_arr->labels[addr].ip
#define GET_JMP_HASH(addr) jl_arr->jumps[addr].name_hash
#define GET_JMP_IP(addr) jl_arr->jumps[addr].ip


#define FPUT(ch)                                                                \
  int printed = fputc (ch, output);                                             \
  if (printed == EOF)                                                           \
  {                                                                             \
    printf("\nCE: WRITE TO FILE FAILED\n");                                     \
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
  }                                                                             \
  putc ('\n', stdout);


#define GET_MASK(format, tmp_var, new_mask)                                     \
  if (scanned = sscanf (text, format, tmp_var))                                 \
  {                                                                             \
    mask = new_mask;                                                            \
    format_str = format;                                                        \
  }                                                                             \
  else

#define ADD_LABEL(cmd)                                                          \
  uint64_t cmd_hash = MurmurHash (cmd, bytes_read);                             \
  printf ("command = %*s; hash = %lx; len = %d; ",                              \
          MAX_NAME_LEN, cmd, cmd_hash, bytes_read);                             \
  for (int iter = 0; iter < jl_arr->lbl_num; iter++)                            \
  {                                                                             \
    if (GET_LABEL_HASH (iter) == cmd_hash)                                      \
    {                                                                           \
      printf("\nCE: LABEL DUPLICATE: %s\n", cmd);                               \
      return LABEL_DUPLICATE;                                                   \
    }                                                                           \
  }                                                                             \
  GET_LABEL_HASH (jl_arr->lbl_num) = cmd_hash;                                  \
  GET_LABEL_IP (jl_arr->lbl_num) = curr_ip;                                     \
  printf("ip = %lx;\n", GET_LABEL_IP (jl_arr->lbl_num));                        \
  jl_arr->lbl_num++;

#define DEF_CMD(num, cmd, argc, code, hash)                                     \
  if (hash == command_hash)                                                     \
  {                                                                             \
    int tmp_ch = 0;                                                             \
    double tmp_lf = 0;                                                          \
    char mask = 0x00;                                                           \
    const char *format_str = "";                                                \
    int scanned = 0;                                                            \
    GET_MASK (      "[%d]",         &tmp_ch, MASK_I2RAM)                        \
    GET_MASK (       "%lf",         &tmp_lf, MASK_IMM)                          \
    GET_MASK ("[%1[a-d]x]", (char *)&tmp_ch, MASK_R2RAM)                        \
    GET_MASK (  "%1[a-d]x", (char *)&tmp_ch, MASK_REG)                          \
    {                                                                           \
      printf("\nCE: IVALID ARGUMENT command = %s\n", command);                  \
      return INVALID_ARG;                                                       \
    }                                                                           \
    if (scanned == EOF && argc > 0)                                             \
    {                                                                           \
      printf("\nCE: IVALID ARGUMENT command = %s\n", command);                  \
      return INVALID_ARG;                                                       \
    }                                                                           \
    FPUT (CMD_##cmd | mask);                                                    \
    printf ("code  = %02X;\n", (unsigned char)(CMD_##cmd | mask));              \
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
    return bytes_written;                                                       \
  }                                                                             \
  else

#define DEF_JMP_CMD(code, name, argc, action, hash)                             \
    if (command_hash == hash)                                                   \
    {                                                                           \
      printf ("code  = %02X;\n", (unsigned char)(code));                        \
      FPUT (code);                                                              \
      printf ("bytes = %02X ", code);                                           \
                                                                                \
      char jmp_arg[MAX_NAME_LEN] = {};                                          \
      int arg_len = 0;                                                          \
      scanned = sscanf (text, ": %[a-zA-Z0-9]%n", jmp_arg, &arg_len);           \
      type_t dummy = 0;                                                         \
      if (!scanned)                                                             \
      {                                                                         \
        int dest_line = 0;                                                      \
        scanned = sscanf (text, "%d", &dest_line);                              \
        if (!scanned)                                                           \
        {                                                                       \
          printf("\nCE: IVALID ARGUMENT command = %s\n", command);              \
          return INVALID_ARG;                                                   \
        }                                                                       \
        if (dest_line > max_line)                                               \
        {                                                                       \
          printf("\nCE: LINE NUMBER IS TOO BIG command = %s\n", command);       \
          return INVALID_ARG;                                                   \
        }                                                                       \
        jl_arr->jumps[jl_arr->jmp_num].destination = dest_line;                 \
        GET_JMP_IP (jl_arr->jmp_num) = curr_ip;                                 \
        jl_arr->jmp_num++;                                                      \
        FPRINT_BYTES (dummy);                                                   \
        return sizeof (type_t);                                                 \
      }                                                                         \
      uint64_t arg_hash = MurmurHash (jmp_arg, arg_len - 1);                    \
                                                                                \
      for (int iter = 0; iter < jl_arr->lbl_num; iter++)                        \
      {                                                                         \
        if (GET_LABEL_HASH (iter) == arg_hash)                                  \
        {                                                                       \
          FPRINT_BYTES (GET_LABEL_IP (iter));                                   \
          putc ('\n', stdout);                                                  \
          return sizeof (type_t);                                               \
        }                                                                       \
      }                                                                         \
      FPRINT_BYTES (dummy);                                                     \
      GET_JMP_IP (jl_arr->jmp_num) = curr_ip;                                   \
      GET_JMP_HASH (jl_arr->jmp_num) = arg_hash;                                \
      jl_arr->jmp_num++;                                                        \
      return sizeof (type_t);                                                   \
    }                                                                           \
    else

int get_io_args (int argc, const char **argv, config *curr_config);

int ProcessCommand (const char *text, FILE* output, JL_info *jl_arr, type_t *lines);

int Assemble (file_info *source, FILE *output);

int Link (FILE *output, type_t *lines, JL_info *jl_arr);
