#pragma once

#include "enum.h"
#include "files.h"

const int MAX_NAME_LEN = 6;

enum COMPILATION_ERRORS
{
  INVALID_SYNTAX   = -1,
  WRITE_FALIED     = -2,
  INVALID_ARG      = -3,
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
  if (sscanf (text, format, &tmp_var))                                          \
  {                                                                             \
    mask = new_mask;                                                            \
    format_str = format;                                                        \
  }                                                                             \
  else

#define DEF_CMD(num, cmd, argc, code)                                           \
  if (!strcmp (command, #cmd))                                                  \
  {                                                                             \
    int tmp_ch = 0;                                                             \
    double tmp_lf = 0;                                                          \
    char mask = 0x00;                                                           \
    const char *format_str = "";                                                \
                                                                                \
    GET_MASK (  "%1[a-d]x", tmp_ch, 0x20)                                       \
    GET_MASK (       "%lf", tmp_lf, 0x40)                                       \
    GET_MASK ("[%1[a-d]x]", tmp_ch, 0xA0)                                       \
    GET_MASK (      "[%d]", tmp_ch, 0xC0)                                       \
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
      if (mask == 0x40)                                                         \
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
        printf("arg = ");                                                       \
        printf("%d", argv);                                               \
        printf ("; bytes = ");                                                  \
        FPRINT_BYTES (argv);                                                    \
        bytes_written += 1;                                                     \
      }                                                                         \
      text += bytes_read;                                                       \
    }                                                                           \
    printf("\n");                                                               \
    return bytes_written;                                                       \
  }                                                                             \
    else

int ProcessCommand (const char *text, FILE* output);

int Assemble (file_info *source, FILE *output);
