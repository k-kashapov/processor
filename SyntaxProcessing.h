#pragma once

#include "files.h"

const int MAX_NAME_LEN = 10;

int ProcessCommand (const char *command, int val, FILE* output);

int Compile (file_info *source, FILE *output);
