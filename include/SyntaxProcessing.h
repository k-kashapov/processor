#pragma once

#include "files.h"

const int MAX_NAME_LEN = 6;

enum COMPILATION_ERRORS
{
  INVALID_SYNTAX = -1,
};

#define FPUT(val)                                            \
  for (int i = 0; i < sizeof (val); i++)                     \
  {                                                          \
    unsigned char val_byte = (unsigned char) val;            \
    fputc (val_byte, output);                                \
    printf ("%02X ", val_byte);                              \
                                                             \
    val >>= 8;                                               \
  }

#define DEF_CMD(num, cmd, argc, code)                        \
  if (!strcmp (command, #cmd))                               \
  {                                                          \
    char tmp = 0;                                            \
    int char_read = sscanf (text, "%1[a-d]x", &tmp);         \
    char mask = 0x20;                                        \
    if (!char_read) mask = 0x10;                             \
    fputc (CMD_##cmd | mask, output);                        \
    printf ("%02X ", CMD_##cmd | mask);                      \
                                                             \
    for (int arg = 0; arg < argc; arg++)                     \
    {                                                        \
      int bytes_read = 0;                                    \
                                                             \
      if (!char_read)                                        \
      {                                                      \
        double argv = 0;                                     \
        sscanf (text, "%lf %n", &argv, &bytes_read);         \
        printf("value = %06.3lf; bytes = ", argv);           \
        type_t value = (type_t)(argv * 1000);                \
        FPUT (value);                                        \
      }                                                      \
      else                                                   \
      {                                                      \
        char argv = 0;                                       \
        sscanf (text, "%1[a-d]x %n", &argv, &bytes_read);    \
        printf("register = %c; bytes = ", argv);             \
        FPUT (argv);                                         \
      }                                                      \
      text += bytes_read;                                    \
    }                                                        \
    printf("\n");                                            \
    return 1 + (sizeof(type_t) * argc);                      \
  }                                                          \
    else

int ProcessCommand (const char *text, FILE* output);

int Assemble (file_info *source, FILE *output);
