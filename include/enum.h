// AUTO-GENERATED
// DO NOT TOUCH IF YOU DON'T KNOW WHAT YOU'RE DOING

#define DEF_CMD(num, name) name##_CMD = num,

enum asm_commands
{
  #include "commands.h"
};

#undef DEF_CMD
