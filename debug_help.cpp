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
