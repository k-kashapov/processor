#include "Stack.h"
#include "SyntaxProcessing.h"

#undef DEF_CMD
#undef DEF_JMP_CMD

#define DEF_CMD(cmd, name, argv, code, hash)  \
  printf ("%*s 0x%X\n", 11, #name " = ", MurmurHashStr (#name));

#define DEF_JMP_CMD(cmd, name, hash)          \
  printf ("0x%X\n", MurmurHashStr (#name));

int main (int argc, const char **argv)
{
  #include "commands.h"
  printf("################################\n");
  #include "jump.h"

  return 0;
}
