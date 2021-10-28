// AUTO-GENERATED
// DO NOT TOUCH IF YOU DON'T KNOW WHAT YOU'RE DOING
#pragma once

#define DEF_CMD(num, name, argc, code, hash) CMD_##name = num,
#define DEF_JMP_CMD(num, name, argc, action, hash) CMD_##name = num,

enum asm_commands
{
  #include "commands.h"
  #include "jump.h"
};

#undef DEF_CMD
#undef DEF_JMP_CMD
