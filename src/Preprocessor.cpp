#include "../include/files.h"

int main (int argc, const char **argv)
{
  FILE* info = fopen ("Processor.h", "rt");

  char *text = read_to_end (info);

  printf ("int version = 1;\nenum ASM_CODES\n{\n");

  for (char *token = strtok (text, "\x23 ("); token; token = strtok (NULL, "\x23 ("))
  {
    if (!strcmp (token, "define"))
    {
      token = strtok (NULL, "\x23 (");
      char *ptr = token;
      while (*ptr)
      {
          *ptr = toupper (*ptr);
          ptr++;
      }
      printf("\t%s_CODE,\n", token);
    }
  }
  printf ("};");

  return 0;
}
