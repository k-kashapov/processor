#include "Disassembler.h"

int disasm_cmd (processor *proc, FILE *output)
{
  unsigned char command = proc->code[proc->ip++];

  switch (command & 0x1F)
  {
    #include "commands.h"
    default:
      return INVALID_CODE;
  }

  return 0;
}

int disassemble (processor *proc, FILE *output)
{
  while (proc->ip < proc->bytes_num)
  {
    dump_proc (proc);

    int error = disasm_cmd (proc, output);

    if (error) return error;
  }

  return 0;
}
