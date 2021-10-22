#include "Disassembler.h"
#include "check_unique.h"

int main (int argc, const char **argv)
{
  processor proc = {};
  stack_t proc_stk = {};
  proc.stk = &proc_stk;

  read_code (&proc);

  int header_err = get_header (&proc);
  if (header_err)
    return header_err;

  FILE *output = NULL;
  open_file (&output, "disamed.txt", "wt");
  if (!output)
    return OPEN_FILE_FAILED;

  int runtime_err = disassemble (&proc, output);
  if (runtime_err)
    return runtime_err;

  fclose (output);
  return 0;
}
