#include "SyntaxProcessing.h"

int main (int argc, const char **argv)
{
  file_info source;
  config io_config;

  read_all_lines (&source, io_config.input_file);

  FILE *output = fopen("code.dead","wb");
  assert (output);

  Compile (&source, output);

  fclose (output);

  return 0;
}
