#pragma once

#include "files.h"

const int MAX_NAME_LEN = 6;

enum COMPILATION_ERRORS
{
  INVALID_SYNTAX = -1,
};

#define FPUT_TYPE_T(val)                            \
  for (int i = 0; i < sizeof (val); i++)            \
  {                                                 \
    unsigned char val_byte = (unsigned char) val;   \
    fputc (val_byte, output);                       \
    printf ("%02X ", val_byte);                     \
                                                    \
    val >>= 8;                                      \
  }

#define DEF_CMD(num, cmd, argc)                     \
  if (!strcmp (command, #cmd))                      \
  {                                                 \
    fputc (CMD_##cmd, output);                      \
    printf ("%02X ", CMD_##cmd);                    \
                                                    \
    for (int arg = 0; arg < argc; arg++)            \
    {                                               \
      double argv = 0;                              \
      int bytes_read = 0;                           \
      sscanf (text, "%lf %n", &argv, &bytes_read);  \
      printf("value = %06.3lf; bytes = ", argv);    \
      text += bytes_read;                           \
                                                    \
      type_t value = (type_t)(argv * 1000);         \
      FPUT_TYPE_T (value);                          \
    }                                               \
    printf("\n");                                   \
    return 1 + (sizeof(type_t) * argc);             \
  }                                                 \
    else

int ProcessCommand (const char *text, FILE* output);

int Assemble (file_info *source, FILE *output);
