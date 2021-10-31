#define DEF_CMD(num, name, argc, code, hash) case CMD_##name: break;

void _check_unique ()
{
  switch (0)
  {
    #include "commands.h"
    default: break;
  }
}

#undef DEF_CMD
