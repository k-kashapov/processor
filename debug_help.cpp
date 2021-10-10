#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const **argv)
{
  struct stat statbuf_src = {};
  stat ("source.txt", &statbuf_src);
  struct stat statbuf_code = {};
  stat ("code.dead", &statbuf_code);

  int is_old_enough = statbuf_src.st_atime <= statbuf_code.st_atime;

  printf ("src_time = %d %s ", 0, is_old_enough ? "<=" : ">");
  printf ("code_time = %ld\n", statbuf_code.st_atime - statbuf_src.st_atime);

  if (!is_old_enough) system ("./comp.exe");
  system ("./proc.exe");

  return 0;
}

// 
// if (!strcmp (command, "push")) fwrite (PUSH_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "in")) fwrite (IN_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "pop")) fwrite (POP_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "add")) fwrite (ADD_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "sub")) fwrite (SUB_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "mul")) fwrite (MUL_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "div")) fwrite (DIV_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "out")) fwrite (OUT_CODE, sizeof (char), 1, output);
// else if (!strcmp (command, "hlt"))
// {
//   fwrite (HLT_CODE, sizeof (char), 1, output);
//   break;
// }

// if (!strcmp (command, "push")) fprintf (output, "%d%f", PUSH_CODE, val);
// else if (!strcmp (command, "in")) fprintf (output, "%d", IN_CODE);
// else if (!strcmp (command, "pop")) fprintf (output, "%d", POP_CODE);
// else if (!strcmp (command, "add")) fprintf (output, "%d", ADD_CODE);
// else if (!strcmp (command, "sub")) fprintf (output, "%d", SUB_CODE);
// else if (!strcmp (command, "mul")) fprintf (output, "%d", MUL_CODE);
// else if (!strcmp (command, "div")) fprintf (output, "%d", DIV_CODE);
// else if (!strcmp (command, "out")) fprintf (output, "%d", OUT_CODE);
// else if (!strcmp (command, "hlt"))
// {
//   fprintf (output, "%d", HLT_CODE);
//   break;
// }
