#include "Stack.h"
#include "Info.h"
#include "files.h"
#include "math.h"

//#define PROC_DUMP

enum MASKS
{
  MASK_IMM   = 0x40, // Immediate (4)
  MASK_REG   = 0x20, // To register (ax)
  MASK_I2RAM = 0xC0, // Immediate to RAM ([4])
  MASK_R2RAM = 0xA0, // Register [ax] to RAM ([ax])
  MASK_S2RAM = 0xE0, // Summ to RAM ([ax+4])
};

const char SetColor[8][10] =
  {
    "\033[0;37m",
    "\033[0;30m",
    "\033[0;31m",
    "\033[0;32m",
    "\033[0;33m",
    "\033[0;34m",
    "\033[0;35m",
    "\033[0;36m",
  };

#define GET_NEXT_CHAR                                                           \
  addr = *(proc->code + proc->ip) - 'a';                                        \
  if (addr < 0 || addr > REG_NUM)                                               \
  {                                                                             \
    printf("RE: INVALID ARGUMENT; command = %d; arg = %ld\n",                   \
    cmd, addr);                                                                 \
    return NULL;                                                                \
  }

#define GET_NEXT_TYPE_T                                                         \
  addr = *(type_t *)(proc->code + proc->ip) / 1000;                             \
  if (addr < 0 || addr > RAM_MEM)                                               \
  {                                                                             \
    printf("RE: INVALID ARGUMENT; command = %d; arg = %ld\n",                   \
    cmd, addr);                                                                 \
    return NULL;                                                                \
  }

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
    type_t *arg = get_arg (proc, command);                                      \
    StackPush (proc->stk, *arg);                                                \
  }

#define in_code                                                                 \
  {                                                                             \
    double in_val = 0;                                                          \
    printf ("Waiting for input...\n");                                          \
    scanf("%lf", &in_val);                                                      \
    StackPush(proc->stk, (type_t) (in_val * 1000));                             \
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
    type_t *dest = get_arg (proc, command);                                     \
    *dest = pop_val;                                                            \
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
    StackPush (proc->stk, (type_t) (1000.0f / (float) b * (float) a));          \
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

#define call_action !StackPush (proc->stk, (type_t) (proc->ip + sizeof (type_t)))

#define sin_code                                                                \
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
    StackPush (proc->stk, (type_t) sin (((double)pop_val) / 1000) * 1000);          \
  }

#define cos_code                                                                \
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
    StackPush (proc->stk, (type_t) cos (((double)pop_val) / 1000) * 1000);      \
  }

#define sqrt_code                                                               \
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
    StackPush (proc->stk, (type_t) sqrt (((double)pop_val) / 1000) * 1000);     \
  }

#define drw_code                                                                \
  {                                                                             \
    long int len = RAM_MEM - (proc->video_mem - proc->RAM);                     \
    for (long Ypixel = 0; Ypixel < len; Ypixel += proc->xRes)                   \
    {                                                                           \
      for (long i = 0; i < proc->xRes; i++)                                     \
      {                                                                         \
        type_t pixel_val = *(proc->video_mem + Ypixel + i) / 1000;              \
        unsigned char pixel_sym = (unsigned char) pixel_val;                    \
        unsigned char color_val = (unsigned char) ((pixel_val >> 8) % 8);       \
        printf ("%s", SetColor[color_val]);                                     \
        printf ("%c ", pixel_sym ? pixel_sym : ' ');                            \
      }                                                                         \
      printf ("\n");                                                            \
    }                                                                           \
  }

#ifdef PROC_DUMP
  #define ret_code                                                              \
    {                                                                           \
      uint64_t pop_err = 0;                                                     \
      type_t val = StackPop (proc->stk, &pop_err);                              \
      printf("value to ret to = %lx\n", val);                                   \
      if (pop_err)                                                              \
      {                                                                         \
        printf("RUNTIME ERROR: in function: out; error: %02lX\n", pop_err);     \
        return pop_err;                                                         \
      }                                                                         \
      proc->ip = val;                                                           \
    }

  #define DEF_CMD(num, name, argc, code, hash)                                  \
    case CMD_##name:                                                            \
      printf("Command = " #name "\n");                                          \
      code;                                                                     \
      break;


  #define DEF_JMP_CMD(num, name, argc, action, hash)                            \
    case CMD_##name:                                                            \
    {                                                                           \
      printf("command = " #name "\n");                                          \
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
        printf("JUMP from %lx\n", proc->ip - sizeof (Header_t));                \
        val = *(type_t *)(proc->code + proc->ip);                               \
        proc->ip = val - sizeof (Header_t);                                     \
        printf ("Moved ip to %02lX\n", val - sizeof (Header_t));                \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        printf ("JUMP condition not worked: " #action " result = %lu\n", res);  \
        proc->ip += sizeof (type_t);                                            \
      }                                                                         \
    }                                                                           \
    break;
#else
  #define ret_code                                                              \
    {                                                                           \
      uint64_t pop_err = 0;                                                     \
      type_t ret_val = StackPop (proc->stk, &pop_err);                          \
      if (pop_err)                                                              \
      {                                                                         \
        printf("RUNTIME ERROR: in function: out; error: %02lX\n", pop_err);     \
        return pop_err;                                                         \
      }                                                                         \
      proc->ip = (unsigned long) ret_val;                                       \
    }

  #define DEF_CMD(num, name, argc, code, hash)                                  \
    case CMD_##name:                                                            \
      code;                                                                     \
      break;

  #define DEF_JMP_CMD(num, name, argc, action, hash)                            \
    case CMD_##name:                                                            \
    {                                                                           \
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
        val = *(type_t *)(proc->code + proc->ip);                               \
        proc->ip = (unsigned long) val - sizeof (Header_t);                     \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        proc->ip += sizeof (type_t);                                            \
      }                                                                         \
    }                                                                           \
    break;
#endif


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
  unsigned char *code;
  unsigned long bytes_num;
  long unsigned int ip;
  type_t *reg;
  type_t *RAM;
  type_t *video_mem;
  unsigned long xRes;
  unsigned long yRes;
};

int get_io_args (int argc, const char **argv, Config *curr_config);

int read_code (processor *proc, Config *io_config);

int get_header (processor *proc);

uint64_t run_binary (processor *proc);

uint64_t process_command (processor *proc);

type_t *get_arg (processor *proc, unsigned char cmd);

int dump_proc (processor *proc);
