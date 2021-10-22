#include "files.h"
#include "Stack.h"
#include "enum.h"

const int REG_NUM = 4;

// TODO argv as a reg number

#define DEF_CMD(num, name, argc, cmd)                     \
    case CMD_##name:                                      \
      fprintf (output, #name " ");                        \
      for (int argn = 0; argn < argc; argn++)             \
      {                                                   \
        type_t argv = *(type_t*)(proc->code + proc->ip);  \
        fprintf (output, "%lf ", (double)argv / 1000);    \
        proc->ip += sizeof (type_t);                      \
      }                                                   \
      fputc ('\n', output);                               \
      break;

enum ExitCodes
  {
    INVALID_SIGNATURE = -1,
    INVALID_CODE      = -2,
    INVALID_VERSION   = -3,
    INVALID_ARG       = -4,
  };

struct processor
{
  stack_t *stk;
  char *code;
  int bytes_num;
  int ip;
  type_t reg[REG_NUM];
  type_t RAM[RAM_MEM];
};

int dump_proc (processor *proc);

int read_code (processor *proc);

int get_header (processor *proc);

int disassemble (processor *proc, FILE *output);

int disasm_cmd (processor *proc, FILE *output);
