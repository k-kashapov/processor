#pragma once

#include "files.h"

const int MAX_NAME_LEN = 6;

enum COMPILATION_ERRORS
{
  INVALID_SYNTAX = -1,
};

#define DEF_CMD(cmd)                              \
  if (!strcmp (command, #cmd)) code = cmd##_CMD;                 

int ProcessCommand (const char *command, int val, FILE* output);

int Compile (file_info *source, FILE *output);
