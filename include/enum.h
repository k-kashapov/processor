// AUTO-GENERATED
// DO NOT TOUCH IF YOU DON'T KNOW WHAT YOU'RE DOING
#pragma once

#define DEF_CMD(num, name, argc, code) CMD_##name = num,

enum asm_commands
{
  #include "commands.h"
};

#undef DEF_CMD
