#include "enum.h"
#include "Info.h"
#include "Processor.h"
#include "files.h"
#include "time.h"

const int CMD_MASK = 0x1F;
#define ARTIFICIAL_SLOW() for (int sleep_iterator_my_personal_var_system = 0; sleep_iterator_my_personal_var_system < 1000; sleep_iterator_my_personal_var_system++);

int get_io_args (int argc, const char **argv, config *curr_config)
{
  while (--argc > 0)
  {
    argv++;

    if (!strncmp (*argv, "-i", 3))
    {
      curr_config->input_file = *(++argv);
      argc--;
    }
    else if (!strncmp (*argv, "-o", 3))
    {
      curr_config->output_file = *(++argv);
      argc--;
    }
  }
  return 0;
}

int read_code (processor *proc, config *io_config)
{
  FILE *code = fopen (io_config->input_file, "rb");
  assert (code && "file opened");

  proc->code = read_to_end (code);
  assert (proc->code);

  int closed = fclose (code);

  if (closed == EOF) return EOF;

  return 0;
}

int get_header (processor *proc)
{
  Header_t header = *((Header_t *)proc->code);
  #ifdef PROC_DUMP
    printf ("sign = %X\n", header.sign);
    printf ("version = %x\n", header.ver);
    printf ("bytes = %d\n------------\n", header.char_num);
  #endif

  if (header.sign != Signature)
  {
    printf ("###########################\n");
    printf ("FATAL: Invalid Signature\n");
    printf ("###########################\n");
    return INVALID_SIGNATURE;
  }

  if (header.ver != Version)
  {
    printf ("###########################\n");
    printf ("FATAL: Invalid version\n");
    printf ("###########################\n");
    return INVALID_VERSION;
  }

  proc->code += sizeof (Header_t);
  proc->bytes_num = header.char_num;
  proc->video_mem = proc->RAM + RAM_MEM - header.xRes * header.yRes;
  proc->xRes = header.xRes;
  proc->yRes = header.yRes;

  return 0;
}

int run_binary (processor *proc)
{
  StackInit (*proc->stk);

  while (proc->ip < proc->bytes_num)
  {
      #ifdef PROC_DUMP
        dump_proc (proc);
      #endif

      int result = process_command (proc);

      if (result == CMD_hlt)
      {
        return 0;
      }

      if (result)
      {
        printf ("\nFATAL: command = %d at %06X; error = %d\n",
        proc->code[proc->ip - 1], proc->ip - 1, error);
        return error;
      }
  }

  return 0;
}

int process_command (processor *proc)
{
  type_t val = 0;

  unsigned char command = proc->code[proc->ip++];

  #ifdef PROC_DUMP
    printf ("\ncom = %02x;\n", command);
  #endif

  switch (command & CMD_MASK)
  {
    #include "commands.h"
    #include "jump.h"
    default:
      return INVALID_CODE;
  }

  #ifdef PROC_DUMP
    printf ("val = %.3lf\n", (double) val / 1000);
  #endif

  return 0;
}

type_t *get_arg (processor *proc, char cmd)
{
  unsigned char command = proc->code[proc->ip];

  if (CHECK_MASK (cmd, MASK_S2RAM))
  {
    type_t addr = 0;
    GET_NEXT_CHAR;
    proc->ip += 1;

    type_t res = proc->reg[addr];
    res += *(type_t *)(proc->code + proc->ip);
    proc->ip += sizeof (type_t);

    #ifdef ARTIFICIAL_SLOW
      ARTIFICIAL_SLOW ()
    #endif

    return proc->RAM + res;
  }
  else if (CHECK_MASK (cmd, MASK_I2RAM))
  {
    type_t addr = 0;
    GET_NEXT_TYPE_T;
    proc->ip += sizeof (type_t);

    #ifdef ARTIFICIAL_SLOW
      ARTIFICIAL_SLOW ()
    #endif

    return (proc->RAM + addr);
  }
  else if (CHECK_MASK (cmd, MASK_R2RAM))
  {
    type_t addr = 0;
    GET_NEXT_CHAR;
    proc->ip += 1;

    #ifdef ARTIFICIAL_SLOW
      ARTIFICIAL_SLOW ()
    #endif

    return proc->RAM + proc->reg[addr] / 1000;
  }
  else if (CHECK_MASK (cmd, MASK_REG))
  {
    type_t addr = 0;
    GET_NEXT_CHAR;
    proc->ip += 1;

    return (type_t *)(proc->reg + addr);
  }
  else if (CHECK_MASK (cmd, MASK_IMM))
  {
    type_t *res = (type_t *)(proc->code + proc->ip);
    proc->ip += sizeof (type_t);
    return res;
  }
  else
    return NULL;
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
  fprintf(log, "\n%*s\n", proc->ip*3, "^");

  fprintf(log, "Stack size     = %d\n", proc->stk->size);
  fprintf(log, "Stack capacity = %d\n", proc->stk->capacity);

  fprintf(log, "Stack: ");
  for (int stk_elem = 0; stk_elem < proc->stk->capacity; stk_elem++)
  {
    fprintf (log, "%ld, ", proc->stk->buffer[stk_elem]);
  }

  fprintf(log, "\nRegs : ");
  for (int reg = 0; reg < REG_NUM; reg++)
  {
    fprintf (log, "%c: %ld, ", 'a' + reg, proc->reg[reg]);
  }

  fprintf(log, "\nRAM: ");
  for (int addr = 0; addr < RAM_MEM; addr++)
  {
    fprintf (log, "%ld, ", proc->RAM[addr]);
  }

  fprintf (log, "\n---------------------------------------------------\n</pre>");

  fclose (log);

  return 0;
}

int dumb_sleep (double ms)
{
  double start = clock();
  while (clock() - start < ms * 1000) ;
  return 0;
}
