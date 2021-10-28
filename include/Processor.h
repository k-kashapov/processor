#include "Stack.h"
#include "Info.h"
#include "files.h"

//#define PROC_DUMP

enum MASKS
{
  MASK_IMM   = 0x40,
  MASK_REG   = 0x20,
  MASK_I2RAM = 0xC0,
  MASK_R2RAM = 0xA0,
};

#define CHECK_MASK(val, msk) ((val & msk) == msk)

#define pop_ab(cmd_name)                                                        \
  uint64_t pop_err = 0;                                                         \
  type_t b = StackPop (proc->stk, &pop_err);                                    \
  type_t a = StackPop (proc->stk, &pop_err);                                    \
  if (pop_err)                                                                  \
  {                                                                             \
    printf("RUNTIME ERROR: in function: " #cmd_name                             \
    "; error: %06lX\n", pop_err);                                               \
    return pop_err;                                                             \
  }

#define push_code                                                               \
  {                                                                             \
    if (CHECK_MASK (command, MASK_I2RAM))                                       \
    {                                                                           \
      char addr = *(proc->code + proc->ip);                                     \
      if (addr < 0 || addr > RAM_MEM)                                           \
      {                                                                         \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",                \
        command, addr);                                                         \
        return INVALID_ARG;                                                     \
      }                                                                         \
      val = proc->RAM [addr];                                                   \
      dumb_sleep (500);       /*intentional sleep*/                             \
      proc->ip += 1;                                                            \
    }                                                                           \
    else if (CHECK_MASK (command, MASK_R2RAM))                                  \
    {                                                                           \
      char reg = *(proc->code + proc->ip) - 'a';                                \
      if (reg < 0 || reg > REG_NUM)                                             \
      {                                                                         \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",                \
        command, reg);                                                          \
        return INVALID_ARG;                                                     \
      }                                                                         \
      val = proc->RAM [proc->reg[reg]];                                         \
      dumb_sleep (500);       /*intentional sleep*/                             \
      proc->ip += 1;                                                            \
    }                                                                           \
    else if (CHECK_MASK (command, MASK_REG))                                    \
    {                                                                           \
      char reg = *(proc->code + proc->ip) - 'a';                                \
      if (reg < 0 || reg > REG_NUM)                                             \
      {                                                                         \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",                \
        command, reg);                                                          \
        return INVALID_ARG;                                                     \
      }                                                                         \
      val = proc->reg[reg];                                                     \
      proc->ip += 1;                                                            \
    }                                                                           \
    else if (CHECK_MASK (command, MASK_IMM))                                    \
    {                                                                           \
      val = *(type_t *)(proc->code + proc->ip);                                 \
      proc->ip += sizeof (type_t);                                              \
    }                                                                           \
    StackPush (proc->stk, val);                                                 \
  }

#define in_code                                                                 \
  {                                                                             \
    double val = 0;                                                             \
    printf ("Waiting for input...\n");                                          \
    scanf("%lf", &val);                                                         \
    StackPush(proc->stk, (type_t) (val * 1000));                                \
  }

#define pop_code                                                                \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t pop_val = StackPop (proc->stk, &pop_err);                            \
                                                                                \
    if (pop_err)                                                                \
    {                                                                           \
      printf("RUNTIME ERROR: command = %d;error: %06lX\n",                      \
      command, pop_err);                                                        \
      return pop_err;                                                           \
    }                                                                           \
                                                                                \
    if (CHECK_MASK (command, MASK_REG) || CHECK_MASK (command, MASK_R2RAM))     \
    {                                                                           \
      char addr = *(proc->code + proc->ip) - 'a';                               \
      if (addr < 0 || addr > REG_NUM)                                           \
      {                                                                         \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",                \
        command, addr);                                                         \
        return INVALID_ARG;                                                     \
      }                                                                         \
      if (CHECK_MASK (command, MASK_R2RAM))                                     \
      {                                                                         \
        proc->RAM [proc->reg[addr]] = pop_val;                                  \
        dumb_sleep (500);       /*intentional sleep*/                           \
      }                                                                         \
      else                                                                      \
        proc->reg[addr] = pop_val;                                              \
    }                                                                           \
    else if (CHECK_MASK (command, MASK_I2RAM))                                  \
    {                                                                           \
      char addr = *(proc->code + proc->ip);                                     \
      if (addr < 0 || addr > REG_NUM)                                           \
      {                                                                         \
        printf("RE: INVALID ARGUMENT; command = %d; arg = %d\n",                \
        command, addr);                                                         \
        return INVALID_ARG;                                                     \
      }                                                                         \
                                                                                \
      proc->RAM [addr] = pop_val;                                               \
      dumb_sleep (500);       /*intentional sleep*/                             \
    }                                                                           \
    proc->ip += 1;                                                              \
  }

#define add_code                                                                \
  {                                                                             \
    pop_ab (add);                                                               \
    StackPush (proc->stk, a + b);                                               \
  }

#define sub_code                                                                \
  {                                                                             \
    pop_ab (sub);                                                               \
    StackPush (proc->stk, a - b);                                               \
  }

#define mul_code                                                                \
  {                                                                             \
    pop_ab (mul);                                                               \
    StackPush (proc->stk, a * b / 1000);                                        \
  }

#define div_code                                                                \
  {                                                                             \
    pop_ab (div);                                                               \
    if (b == 0)                                                                 \
    {                                                                           \
      printf("RUNTIME ERROR: division by zero\n");                              \
      return DIV_BY_ZERO;                                                       \
    }                                                                           \
    StackPush (proc->stk, 1000.0f / a * b);                                     \
  }

#define out_code                                                                \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t num = StackTop (proc->stk, &pop_err);                                \
    if (pop_err)                                                                \
    {                                                                           \
      printf("RUNTIME ERROR: in function: out; error: %02lX\n", pop_err);       \
      return pop_err;                                                           \
    }                                                                           \
    printf ("%.3lf\n", ((double)num) / 1000);                                   \
  }

#define hlt_code return CMD_hlt;

#define ret_code                                                                \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t val = StackPop (proc->stk, &pop_err);                                \
    printf("value to ret to = %lx\n", val);                                     \
    if (pop_err)                                                                \
    {                                                                           \
      printf("RUNTIME ERROR: in function: out; error: %02lX\n", pop_err);       \
      return pop_err;                                                           \
    }                                                                           \
    proc->ip = val;                                                             \
}

#define call_action !StackPush (proc->stk, proc->ip + sizeof (type_t))

#ifdef PROC_DUMP
  #define DEF_CMD(num, name, argc, code, hash)                                  \
    case CMD_##name:                                                            \
      printf("Command = " #name "\n");                                          \
      code;                                                                     \
      break;
#else
  #define DEF_CMD(num, name, argc, code, hash)                                  \
    case CMD_##name:                                                            \
      code;                                                                     \
      break;
#endif

#define DEF_JMP_CMD(num, name, argc, action, hash)                              \
  case CMD_##name:                                                              \
    {                                                                           \
      printf("Command = " #name "\n");                                          \
      type_t a = 0;                                                             \
      type_t b = 0;                                                             \
      if (argc > 0)                                                             \
      {                                                                         \
        uint64_t pop_err = 0;                                                   \
        b = StackPop (proc->stk, &pop_err);                                     \
        a = StackPop (proc->stk, &pop_err);                                     \
        if (pop_err)                                                            \
        {                                                                       \
          printf("RUNTIME ERROR: in function: out; error: %02lX\n", pop_err);   \
          return pop_err;                                                       \
        }                                                                       \
      }                                                                         \
      uint64_t res = action;                                                    \
      if (res)                                                                  \
      {                                                                         \
        printf("JUMP from %x\n", proc->ip);                                     \
        val = *(type_t *)(proc->code + proc->ip);                               \
        proc->ip = val - sizeof (Header_t);                                     \
        printf ("Moved ip to %lx\n", val);                                      \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        printf ("JUMP condition not worked: " #action " result = %lu\n", res);  \
        proc->ip += sizeof (type_t);                                            \
      }                                                                         \
    }                                                                           \
    break;

enum ExitCodes
{
  INVALID_SIGNATURE = -1,
  INVALID_CODE      = -2,
  DIV_BY_ZERO       = -3,
  INVALID_VERSION   = -4,
  INVALID_ARG       = -5,
};


struct processor
{
  stack_t *stk;
  char *code;
  int bytes_num;
  int ip;
  type_t reg[REG_NUM];
  type_t RAM [RAM_MEM];
};

int get_io_args (int argc, const char **argv, config *curr_config);

int read_code (processor *proc, config *io_config);

int get_header (processor *proc);

int run_binary (processor *proc);

int process_command (processor *proc);

int dump_proc (processor *proc);

int dumb_sleep (double ms);
