#include "Info.h"
#include "enum.h"
#include "Processor.h"
#include "files.h"

int read_code (processor *proc)
{
  FILE *code = fopen ("code.dead", "rb");
  assert (code);

  proc->code = read_to_end (code);
  assert (proc->code);

  return 0;
}

int get_header (processor *proc)
{
  Header_t header = *((Header_t *)proc->code);
  #ifdef PROC_DUMP
    printf ("sign = %X\n", header.signature);
    printf ("version = %x\n", header.version);
    printf ("bytes = %d\n", header.char_num);
  #endif

  if (header.signature != 'KEEK')
  {
    printf ("###########################\n");
    printf ("FATAL: Invalid file\n");
    printf ("###########################\n");
    return INVALID_SIGNATURE;
  }

  proc->code += sizeof (Header_t);
  proc->bytes_num = header.char_num;

  return 0;
}

int run_binary (processor *proc)
{
  StackInit (*proc->stk);

  unsigned char *bytes = (unsigned char *)proc->code;

  while (proc->ip < proc->bytes_num)
  {
      #ifdef PROC_DUMP
        dump_proc (proc);
      #endif

      int error = process_command (proc);

      if (error == CMD_hlt)
      {
        return 0;
      }

      if (error)
      {
        printf ("\nFATAL: command = %d at %d\n", proc->code[proc->ip - 1], proc->ip - 1);
        return error;
      }
  }

  return 0;
}

int process_command (processor *proc)
{
  type_t val = 0;

  unsigned char command = proc->code[proc->ip++];

  switch (command)
  {
    #include "commands.h"
    default:
      return INVALID_CODE;
  }

  #ifdef PROC_DUMP
    printf ("com = %d; val = %.3lf\n", command, (double) val / 1000);
  #endif

  return 0;
}

int dump_proc (processor *proc)
{
  assert (proc->code && "Bytes array for dump is empty!");
  FILE *log = fopen ("processor_log.html", "at");
  assert (log);

  fprintf(log, "<pre>");
  for (int printer = 0; printer < proc->bytes_num; printer++)
  {
    fprintf (log, "%02X ", (unsigned char)printer);
  }
  fprintf(log, "\n");
  for (int printer = 0; printer < proc->bytes_num; printer++)
  {
    fprintf (log, "%02X ", ((unsigned char*)proc->code)[printer]);
  }
  fprintf(log, "\n%*s", proc->ip*3, "^");

  fprintf (log, "\n---------------------------------------------------\n</pre>");

  fclose (log);

  return 0;
}
