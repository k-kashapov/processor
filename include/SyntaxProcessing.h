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

#define FPUT(ch)                                                               \
  int printed = fputc (ch, output);                                            \
  if (printed == EOF)                                                          \
  {                                                                            \
    printf("\nCE: WRITE TO FILE FAILED command = %s\n",                        \
    command);                                                                  \
    return WRITE_FALIED;                                                       \
  }

#define FPRINT_BYTES(val)                                                      \
  for (int i = 0; i < sizeof (val); i++)                                       \
  {                                                                            \
    unsigned char val_byte = (unsigned char) val;                              \
    FPUT (val_byte);                                                           \
    printf ("%02X ", val_byte);                                                \
                                                                               \
    val >>= 8;                                                                 \
  }

#define DEF_CMD(num, cmd, argc, code)                                          \
  if (!strcmp (command, #cmd))                                                 \
  {                                                                            \
    char tmp_ch = 0;                                                           \
    double tmp_lf = 0;                                                         \
    int char_read = sscanf (text, "%1[a-d]x", &tmp_ch);                        \
    char mask = 0x20;                                                          \
    if (!char_read)                                                            \
    {                                                                          \
      if (sscanf (text, "%lf", &tmp_lf))                                       \
        mask = 0x40;                                                           \
      else                                                                     \
      {                                                                        \
        printf("\nCE: IVALID ARGUMENT command = %s\n",                         \
        command);                                                              \
        return INVALID_ARG;                                                    \
      }                                                                        \
    }                                                                          \
    FPUT (CMD_##cmd | mask);                                                   \
    printf ("code = %02X; ", CMD_##cmd | mask);                                \
                                                                               \
    int bytes_written = 1;                                                     \
                                                                               \
    for (int arg = 0; arg < argc; arg++)                                       \
    {                                                                          \
      int bytes_read = 0;                                                      \
                                                                               \
      if (!char_read)                                                          \
      {                                                                        \
        double argv = 0;                                                       \
        sscanf (text, "%lf %n", &argv, &bytes_read);                           \
        printf("value = %06.3lf; bytes = ", argv);                             \
        type_t value = (type_t)(argv * 1000);                                  \
        FPRINT_BYTES (value);                                                  \
        bytes_written += sizeof(type_t);                                       \
      }                                                                        \
      else                                                                     \
      {                                                                        \
        char argv = 0;                                                         \
        sscanf (text, "%1[a-d]x %n", &argv, &bytes_read);                      \
        printf("register = %3c; bytes = ", argv);                              \
        FPRINT_BYTES (argv);                                                   \
        bytes_written += 1;                                                    \
      }                                                                        \
      text += bytes_read;                                                      \
    }                                                                          \
    printf("\n");                                                              \
    return bytes_written;                                                      \
  }                                                                            \
    else

int ProcessCommand (const char *text, FILE* output);

int Assemble (file_info *source, FILE *output);
