#include "Stack.h"

#define pop_ab(cmd_name)                                                  \
  uint64_t pop_err = 0;                                                   \
  type_t b = StackPop (proc->stk, &pop_err);                              \
  type_t a = StackPop (proc->stk, &pop_err);                              \
  if (pop_err)                                                            \
  {                                                                       \
    printf("RUNTIME ERROR: in function: " #cmd_name                       \
    "; error: %06lX\n", pop_err);                                         \
    return pop_err;                                                       \
  }

#define push_code                                                         \
  {                                                                       \
    val = *(type_t *)(proc->code + proc->ip);                             \
    StackPush (proc->stk, val);                                           \
    proc->ip += sizeof (type_t);                                               \
  }

#define in_code                                                           \
  {                                                                       \
    double val = 0;                                                       \
    printf ("Waiting for input...\n");                                    \
    scanf("%lf", &val);                                                   \
    StackPush(proc->stk, (type_t) (val * 1000));                          \
  }

#define pop_code StackPop (proc->stk, NULL)

#define add_code                                                          \
  {                                                                       \
    pop_ab (add);                                                         \
    StackPush (proc->stk, a + b);                                         \
  }

#define sub_code                                                          \
  {                                                                       \
    pop_ab (sub);                                                         \
    StackPush (proc->stk, a - b);                                         \
  }

#define mul_code                                                          \
  {                                                                       \
    pop_ab (mul);                                                         \
    StackPush (proc->stk, a * b / 1000);                                  \
  }

#define div_code                                                          \
  {                                                                       \
    pop_ab (div);                                                         \
    if (b == 0)                                                           \
    {                                                                     \
      printf("RUNTIME ERROR: division by zero\n");                        \
      return DIV_BY_ZERO;                                                 \
    }                                                                     \
    StackPush (proc->stk, 1000.0f / a * b);                               \
  }

#define out_code                                                          \
  {                                                                       \
    uint64_t pop_err = 0;                                                 \
    type_t num = StackPop (proc->stk, &pop_err);                          \
    if (pop_err)                                                          \
    {                                                                     \
      printf("RUNTIME ERROR: in function: out; error: %02lX\n", pop_err); \
      return pop_err;                                                     \
    }                                                                     \
    printf ("%.3lf\n", ((double)num) / 1000);                             \
  }

#define hlt_code                                                          \
  return CMD_hlt;

#define DEF_CMD(num, name, argc, code)                                    \
  case CMD_##name:                                                        \
    code;                                                                 \
    break;

enum ExitCodes
{
  INVALID_SIGNATURE = -1,
  INVALID_CODE      = -2,
  DIV_BY_ZERO       = -3,
};

#define PROC_DUMP

const int REG_NUM = 4;

struct processor
{
  stack_t *stk;
  char *code;
  int bytes_num;
  int ip;
  type_t reg[REG_NUM];
};

int read_code (processor *proc);

int get_header (processor *proc);

int run_binary (processor *proc);

int process_command (processor *proc);

int dump_proc (processor *proc);
