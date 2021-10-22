#include "Stack.h"
#include "enum.h"
#include "Info.h"

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
    if ((command & 0xC0) == 0xC0)                                         \
    {                                                                     \
      char addr = *(proc->code + proc->ip);                               \
      if (addr < 0 || addr > RAM_MEM)                                     \
      {                                                                   \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",          \
        command, addr);                                                   \
      }                                                                   \
      val = proc->RAM[addr];                                              \
      proc->ip += 1;                                                      \
    }                                                                     \
    else if ((command & 0xA0) == 0xA0)                                    \
    {                                                                     \
      char reg = *(proc->code + proc->ip) - 'a';                          \
      if (reg < 0 || reg > REG_NUM)                                       \
      {                                                                   \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",          \
        command, reg);                                                    \
      }                                                                   \
      val = proc->RAM[proc->reg[reg]];                                    \
      proc->ip += 1;                                                      \
    }                                                                     \
    else if ((command & 0x20) == 0x20)                                    \
    {                                                                     \
      char reg = *(proc->code + proc->ip) - 'a';                          \
      if (reg < 0 || reg > REG_NUM)                                       \
      {                                                                   \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",          \
        command, reg);                                                    \
      }                                                                   \
      val = proc->reg[reg];                                               \
      proc->ip += 1;                                                      \
    }                                                                     \
    else if ((command & 0x40) == 0x40)                                    \
    {                                                                     \
      val = *(type_t *)(proc->code + proc->ip);                           \
      proc->ip += sizeof (type_t);                                        \
    }                                                                     \
    StackPush (proc->stk, val);                                           \
  }

#define in_code                                                           \
  {                                                                       \
    double val = 0;                                                       \
    printf ("Waiting for input...\n");                                    \
    scanf("%lf", &val);                                                   \
    StackPush(proc->stk, (type_t) (val * 1000));                          \
  }

#define pop_code                                                          \
  {                                                                       \
    uint64_t pop_err = 0;                                                 \
    type_t pop_val = StackPop (proc->stk, &pop_err);                      \
                                                                          \
    if (pop_err)                                                          \
    {                                                                     \
      printf("RUNTIME ERROR: command = %d;error: %06lX\n",                \
      command, pop_err);                                                  \
      return pop_err;                                                     \
    }                                                                     \
                                                                          \
    if ((command & 0x20) == 0x20 || (command & 0xA0) == 0xA0)             \
    {                                                                     \
      char addr = *(proc->code + proc->ip) - 'a';                         \
      if (addr < 0 || addr > REG_NUM)                                     \
      {                                                                   \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",          \
        command, addr);                                                   \
      }                                                                   \
                                                                          \
      printf ("addr = %d\n", addr);                   \
      if ((command & 0x20) == 0x20)                                       \
        proc->reg[addr] = pop_val;                                        \
      else                                                                \
        proc->RAM[proc->reg[addr]] = pop_val;                             \
    }                                                                     \
    else if ((command & 0xC0) == 0xC0)                                    \
    {                                                                     \
      char addr = *(proc->code + proc->ip);                               \
      if (addr < 0 || addr > REG_NUM)                                     \
      {                                                                   \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",          \
        command, addr);                                                   \
      }                                                                   \
                                                                          \
      proc->RAM[addr] = pop_val;                                          \
    }                                                                     \
    proc->ip += 1;                                                        \
  }

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
    type_t num = StackTop (proc->stk, &pop_err);                          \
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
  INVALID_VERSION   = -4,
  INVALID_ARG       = -5,
};

#define PROC_DUMP

struct processor
{
  stack_t *stk;
  char *code;
  int bytes_num;
  int ip;
  type_t reg[REG_NUM];
  type_t RAM[RAM_MEM];
};

int read_code (processor *proc);

int get_header (processor *proc);

int run_binary (processor *proc);

int process_command (processor *proc);

int dump_proc (processor *proc);
