#pragma once

#include "Stack.h"
#include "enum.h"
#include "files.h"
#include <fcntl.h>

const int MAX_NAME_LEN   =   32;
const int MAX_LABELS_NUM =   32;
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
    for (unsigned long byte = 0; byte < sizeof (val); byte++)                   \
    {                                                                           \
      bin_info->binary[byte] = _str[byte];                                      \
      printf ("%02X ", bin_info->binary[byte]);                                 \
    }                                                                           \
    putc ('\n', stdout);                                                        \
  }

#define SPUT(val) *bin_info->binary++ = (unsigned char)(val);

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
  case hash:                                                                    \
  {                                                                             \
    long bytes_written = get_args (text, CMD_##cmd, argc, bin_info);            \
    return bytes_written;                                                       \
  }                                                                             \
  break;

#define DEF_JMP_CMD(code, name, argc, action, hash)                                               \
    case hash:                                                                                    \
    {                                                                                             \
      long bytes_jmp_written = get_jmp_args (text, CMD_##name, bin_info, curr_ip, max_line);      \
      return bytes_jmp_written;                                                                   \
    }                                                                                             \
    break;

long get_args (const char *text, char cmd, int argc, BinaryInfo *bin_info);

long get_jmp_args (const char *text, char cmd, BinaryInfo *bin_info, unsigned long curr_ip, long long int max_line);

int get_io_args (int argc, const char **argv, Config *curr_config);

long ProcessCommand (const char *text, BinaryInfo *bin_info);

long Assemble (File_info *source, unsigned char *binary_arr);

int Link (BinaryInfo *bin_info);
