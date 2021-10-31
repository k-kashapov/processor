#pragma once

#include "Stack.h"
#include "enum.h"
#include "files.h"
#include <fcntl.h>

const int MAX_NAME_LEN   =  32;
const int MAX_LABELS_NUM =  32;
const int MAX_BINARY_LEN = 2048;

enum COMPILATION_ERRORS
{
  INVALID_SYNTAX   = -1,
  WRITE_FALIED     = -2,
  INVALID_ARG      = -3,
  LABEL_DUPLICATE  = -4,
  STRAY_JUMP       = -5,
};

enum MASKS
{
  MASK_IMM   = 0x40, // Immediate (4)
  MASK_REG   = 0x20, // To register (ax)
  MASK_I2RAM = 0xC0, // Immediate to RAM ([4])
  MASK_R2RAM = 0xA0, // Register [ax] to RAM ([ax])
  MASK_S2RAM = 0xE0, // Summ to RAM ([ax+4])
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

struct BinaryInfo
{
  Label *labels;
  Jump *jumps;
  unsigned long jmp_num;
  unsigned long lbl_num;
  unsigned char *binary;
  unsigned char *binary_arr;
  type_t *lines;
};

#define GET_LABEL_HASH(addr) bin_info->labels[addr].name_hash
#define GET_LABEL_IP(addr) bin_info->labels[addr].ip
#define GET_JMP_HASH(addr) bin_info->jumps[addr].name_hash
#define GET_JMP_IP(addr) bin_info->jumps[addr].ip

#define SPRINT_BYTES(val)                                                       \
  {                                                                             \
    unsigned char *_str = (unsigned char *)&val;                                \
    for (int i = 0; i < sizeof (val); i++)                                      \
    {                                                                           \
      bin_info->binary[i] = _str[i];                                            \
      printf ("%02X ", bin_info->binary[i]);                                    \
    }                                                                           \
    putc ('\n', stdout);                                                        \
  }

#define SPUT(val) *bin_info->binary++ = (val);

#define GET_MASK(format, tmp_var, new_mask)                                     \
  if ((arg_scanned = sscanf (text, format, tmp_var)))                           \
  {                                                                             \
    mask = new_mask;                                                            \
  }                                                                             \
  else

#define ADD_LABEL(cmd)                                                          \
  uint64_t cmd_hash = MurmurHash (cmd, (unsigned long) bytes_read);             \
  printf ("command = %*s; hash = %lx; len = %d; ",                              \
          MAX_NAME_LEN / 2, cmd, cmd_hash, bytes_read);                         \
  for (unsigned long lbl = 0; lbl < bin_info->lbl_num; lbl++)                   \
  {                                                                             \
    if (GET_LABEL_HASH (lbl) == cmd_hash)                                       \
    {                                                                           \
      printf("\nCE: LABEL DUPLICATE: %s\n", cmd);                               \
      return LABEL_DUPLICATE;                                                   \
    }                                                                           \
  }                                                                             \
  GET_LABEL_HASH (bin_info->lbl_num) = cmd_hash;                                \
  GET_LABEL_IP (bin_info->lbl_num) = (type_t) curr_ip;                          \
  printf("ip = %ld;\n", GET_LABEL_IP (bin_info->lbl_num));                      \
  bin_info->lbl_num++;

#define DEF_CMD(num, cmd, argc, code, hash)                                     \
  if (hash == command_hash)                                                     \
  {                                                                             \
    type_t tmp_int = 0;                                                         \
    double tmp_lf = 0;                                                          \
    unsigned char mask = 0x00;                                                  \
    int arg_scanned = 0;                                                        \
    if (sscanf (text, "[%1[a-z]x + %ld]", (char *)&tmp_int, &tmp_int) == 2)     \
    {                                                                           \
      mask = MASK_S2RAM;                                                        \
    }                                                                           \
    else                                                                        \
    GET_MASK (     "[%ld]",         &tmp_int, MASK_I2RAM)                       \
    GET_MASK (       "%lf",          &tmp_lf,   MASK_IMM)                       \
    GET_MASK ("[%1[a-z]x]", (char *)&tmp_int, MASK_R2RAM)                       \
    GET_MASK (  "%1[a-z]x", (char *)&tmp_int,   MASK_REG)                       \
    {                                                                           \
      arg_scanned = EOF;                                                        \
    }                                                                           \
    if (arg_scanned == EOF && argc > 0)                                         \
    {                                                                           \
      printf("\nCE: INVALID ARGUMENT command = %s\n", command);                 \
      return INVALID_ARG;                                                       \
    }                                                                           \
    SPUT (CMD_##cmd | mask);                                                    \
    printf ("code  = %02X;\n", (unsigned char)(CMD_##cmd | mask));              \
                                                                                \
    long bytes_written = 0;                                                     \
                                                                                \
    for (int arg = 0; arg < argc; arg++)                                        \
    {                                                                           \
      int arg_bytes_read = 0;                                                   \
      if (mask == MASK_S2RAM)                                                   \
      {                                                                         \
        char regv = 0;                                                          \
        type_t offset = 0;                                                      \
        arg_scanned = sscanf (text, "[%1[a-z]x + %ld]", &regv, &offset);        \
        if (!arg_scanned)                                                       \
        {                                                                       \
          printf ("INVALID_ARG\n");                                             \
          return INVALID_ARG;                                                   \
        }                                                                       \
        printf ("args = %d and %ld; bytes = ", regv, offset);                   \
        SPRINT_BYTES (regv);                                                    \
        bin_info->binary += 1;                                                  \
        bytes_written += 1;                                                     \
        SPRINT_BYTES (offset);                                                  \
        bytes_written += (long) sizeof(type_t);                                 \
      }                                                                         \
      else if (mask == MASK_I2RAM)                                              \
      {                                                                         \
        type_t argv = 0;                                                        \
        arg_scanned = sscanf (text, "[%ld]", &argv);                            \
        if (!arg_scanned)                                                       \
        {                                                                       \
          printf ("INVALID_ARG\n");                                             \
          return INVALID_ARG;                                                   \
        }                                                                       \
        printf ("value = %ld; bytes = ", argv);                                 \
        type_t value = (argv * 1000);                                           \
        SPRINT_BYTES (value);                                                   \
        bytes_written += (long) sizeof(type_t);                                 \
      }                                                                         \
      else if (mask == MASK_IMM)                                                \
      {                                                                         \
        double argv = 0;                                                        \
        arg_scanned = sscanf (text, "%lf", &argv);                              \
        if (!arg_scanned)                                                       \
        {                                                                       \
          printf ("INVALID_ARG\n");                                             \
          return INVALID_ARG;                                                   \
        }                                                                       \
        printf ("value = %06.3lf; bytes = ", argv);                             \
        type_t value = (type_t)(argv * 1000);                                   \
        SPRINT_BYTES (value);                                                   \
        bytes_written += (long) sizeof(type_t);                                 \
      }                                                                         \
      else if (mask == MASK_R2RAM)                                              \
      {                                                                         \
        char argv = 0;                                                          \
        arg_scanned = sscanf (text, "[%1[a-z]x]", &argv);                       \
        if (!arg_scanned)                                                       \
        {                                                                       \
          printf ("INVALID_ARG\n");                                             \
          return INVALID_ARG;                                                   \
        }                                                                       \
        printf ("value = %d; bytes = ", argv);                                  \
        type_t value = (type_t)(argv * 1000);                                   \
        SPRINT_BYTES (value);                                                   \
        bytes_written += (long) sizeof(type_t);                                 \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        char argv = 0;                                                          \
        arg_scanned = sscanf (text, "%1[a-z]x", &argv);                         \
        if (!arg_scanned)                                                       \
        {                                                                       \
          printf ("INVALID_ARG\n");                                             \
          return INVALID_ARG;                                                   \
        }                                                                       \
        printf ("arg = %d; bytes = ", argv);                                    \
        SPRINT_BYTES (argv);                                                    \
        bytes_written += 1;                                                     \
      }                                                                         \
      text += arg_bytes_read;                                                   \
    }                                                                           \
    return bytes_written;                                                       \
  }                                                                             \
  else

#define DEF_JMP_CMD(code, name, argc, action, hash)                             \
    if (command_hash == hash)                                                   \
    {                                                                           \
      printf ("code  = %02X; bytes = \n", (unsigned int)code);                  \
      SPUT (code);                                                              \
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
        bin_info->jumps[bin_info->jmp_num].destination = dest_line;             \
        GET_JMP_IP (bin_info->jmp_num) = (type_t) curr_ip;                      \
        bin_info->jmp_num++;                                                    \
        SPRINT_BYTES (dummy);                                                   \
        return sizeof (type_t);                                                 \
      }                                                                         \
      uint64_t arg_hash = MurmurHash (jmp_arg, (unsigned long) arg_len - 1);    \
                                                                                \
      for (int iter = 0; iter < bin_info->lbl_num; iter++)                      \
      {                                                                         \
        if (GET_LABEL_HASH (iter) == arg_hash)                                  \
        {                                                                       \
          type_t dest_ip = GET_LABEL_IP (iter);                                 \
          SPRINT_BYTES (dest_ip);                                               \
          putc ('\n', stdout);                                                  \
          return sizeof (type_t);                                               \
        }                                                                       \
      }                                                                         \
      SPRINT_BYTES (dummy);                                                     \
      GET_JMP_IP (bin_info->jmp_num) = (type_t) curr_ip;                        \
      GET_JMP_HASH (bin_info->jmp_num) = arg_hash;                              \
      bin_info->jmp_num++;                                                      \
      return sizeof (type_t);                                                   \
    }                                                                           \
    else

int get_io_args (int argc, const char **argv, Config *curr_config);

long ProcessCommand (const char *text, FILE *output, BinaryInfo *bin_info);

long Assemble (File_info *source, unsigned char *binary_arr);

int Link (BinaryInfo *bin_info);
