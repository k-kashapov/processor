// AUTO-GENERATED
// DO NOT TOUCH IF YOU DON'T KNOW WHAT YOU'RE DOING

#define DEF_CMD(num, name, argc, code) CMD_##name = num,

enum asm_commands
{
  #include "commands.h"
};

#undef DEF_CMD

#define DEF_CMD(num, name, argc, code) case CMD_##name: break;

void _check_unique ()
{
  switch (0)
  {
    #include "commands.h"
  }
}

#undef DEF_CMD
