#include "Stack.h"
#include "Info.h"
#include "files.h"
#include "math.h"

//#define PROC_DUMP

const int MULTIPLIER = 1000;

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
    printf ("RE: INVALID ARGUMENT; command = %d; arg = %ld\n",                  \
    cmd, addr);                                                                 \
    return NULL;                                                                \
  }

#define GET_NEXT_TYPE_T                                                         \
  addr = *(type_t *)(proc->code + proc->ip) / MULTIPLIER;                             \
  if (addr < 0 || addr > RAM_MEM)                                               \
  {                                                                             \
    printf ("RE: INVALID ARGUMENT; command = %d; arg = %ld\n",                  \
    cmd, addr);                                                                 \
    return NULL;                                                                \
  }

#define CHECK_MASK(val, msk) ((val & msk) == msk)

#define POP(cmd_name)                                                           \
  StackPop (proc->stk, &pop_err);                                               \
  if (pop_err)                                                                  \
  {                                                                             \
    printf ("RUNTIME ERROR: in function: " #cmd_name                            \
    "; error: %06lX\n", pop_err);                                               \
    return pop_err;                                                             \
  }

#define TOP(cmd_name)                                                           \
  StackTop (proc->stk, &pop_err);                                               \
  if (pop_err)                                                                  \
  {                                                                             \
    printf ("RUNTIME ERROR: in function: " #cmd_name                            \
    "; error: %06lX\n", pop_err);                                               \
    return pop_err;                                                             \
  }

#define PUSH(value) StackPush (proc->stk, value)

#define POP_AB(cmd_name)                                                        \
  uint64_t pop_err = 0;                                                         \
  type_t b = POP (cmd_name);                                                    \
  type_t a = POP (cmd_name);


#define PUSH_CODE                                                               \
  {                                                                             \
    type_t *arg = get_arg (proc, command);                                      \
    PUSH (*arg);                                                                \
  }

#define IN_CODE                                                                 \
  {                                                                             \
    double in_val = 0;                                                          \
    printf ("Waiting for input...\n");                                          \
    scanf("%lf", &in_val);                                                      \
    PUSH ((type_t) (in_val * MULTIPLIER));                                            \
  }

#define POP_CODE                                                                \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t pop_val = POP (pop);                                                 \
    type_t *dest = get_arg (proc, command);                                     \
    *dest = pop_val;                                                            \
  }

#define ADD_CODE                                                                \
  {                                                                             \
    POP_AB (add);                                                               \
    PUSH (a + b);                                                               \
  }

#define SUB_CODE                                                                \
  {                                                                             \
    POP_AB (sub);                                                               \
    PUSH (a - b);                                                               \
  }

#define MUL_CODE                                                                \
  {                                                                             \
    POP_AB (mul);                                                               \
    PUSH (a * b / MULTIPLIER);                                                  \
  }

#define DIV_CODE                                                                \
  {                                                                             \
    POP_AB (div);                                                               \
    if (b == 0)                                                                 \
    {                                                                           \
      printf("RUNTIME ERROR: division by zero\n");                              \
      return DIV_BY_ZERO;                                                       \
    }                                                                           \
    PUSH ((type_t) ((float)MULTIPLIER / (float) b * (float) a));                \
  }

#define OUT_CODE                                                                \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t num = TOP (&pop_err);                                                \
    printf (">> %.3lf\n", ((double)num) / MULTIPLIER);                          \
  }

#define HLT_CODE return CMD_hlt;

#define CALL_ACTION !PUSH ((type_t) (proc->ip + sizeof (type_t)))

#define SIN_CODE                                                                \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t pop_val = POP (sin);                                                 \
    PUSH ((type_t) sin (((double)pop_val) / MULTIPLIER) * MULTIPLIER);                      \
  }

#define COS_CODE                                                                \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t pop_val = POP (cos);                                                 \
    PUSH ((type_t) cos (((double)pop_val) / MULTIPLIER) * MULTIPLIER);                      \
  }

#define SQRT_CODE                                                               \
  {                                                                             \
    uint64_t pop_err = 0;                                                       \
    type_t pop_val = POP (sqrt);                                                \
    PUSH ((type_t) sqrt (((double)pop_val) / MULTIPLIER) * MULTIPLIER);                     \
  }

#define DRW_CODE                                                                \
  {                                                                             \
    system ("clear");                                                           \
    long int len = RAM_MEM - (proc->video_mem - proc->RAM);                     \
    for (long Ypixel = 0; Ypixel < len; Ypixel += proc->xRes)                   \
    {                                                                           \
      for (long xPixel = 0; xPixel < proc->xRes; xPixel++)                      \
      {                                                                         \
        type_t pixel_val = *(proc->video_mem + Ypixel + xPixel) / MULTIPLIER;         \
        unsigned char pixel_sym = (unsigned char) pixel_val;                    \
        unsigned char color_val = (unsigned char) ((pixel_val >> 8) % 8);       \
        printf ("%s", SetColor[color_val]);                                     \
        printf ("%c ", pixel_sym ? pixel_sym : ' ');                            \
      }                                                                         \
      printf ("\n");                                                            \
    }                                                                           \
  }

#ifdef PROC_DUMP
  #define RET_CODE                                                              \
    {                                                                           \
      uint64_t pop_err = 0;                                                     \
      type_t val = POP (ret);                                                   \
      printf ("value to ret to = %lx\n", val);                                  \
      proc->ip = val;                                                           \
    }

  #define DEF_CMD(num, name, argc, code, hash)                                  \
    case CMD_##name:                                                            \
      printf ("Command = " #name "\n");                                         \
      code;                                                                     \
      break;

  #define JUMP_WITH_0_ARGS(name, action)                                        \
    {                                                                           \
      printf("command = " #name "\n");                                          \
      if (action)                                                               \
      {                                                                         \
        printf("JUMP from %lx\n", proc->ip - sizeof (Header_t));                \
        val = *(type_t *)(proc->code + proc->ip);                               \
        proc->ip = (unsigned long) val - sizeof (Header_t);                     \
        printf ("Moved ip to %02lX\n", val - sizeof (Header_t));                \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        printf ("JUMP condition not worked: " #action " result = %lu\n", res);  \
        proc->ip += sizeof (type_t);                                            \
      }                                                                         \
    }

  #define JUMP_WITH_2_ARGS(name, action)                                        \
    {                                                                           \
      POP_AB ()                                                                 \
      printf("command = " name "\n");                                           \
      if (action)                                                               \
      {                                                                         \
        printf("JUMP from %lx\n", proc->ip - sizeof (Header_t));                \
        val = *(type_t *)(proc->code + proc->ip);                               \
        proc->ip = (unsigned long) val - sizeof (Header_t);                     \
        printf ("Moved ip to %02lX\n", val - sizeof (Header_t));                \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        printf ("JUMP condition not worked: " #action " result = %lu\n", res);  \
        proc->ip += sizeof (type_t);                                            \
      }                                                                         \
    }
#else
  #define RET_CODE                                                              \
    {                                                                           \
      uint64_t pop_err = 0;                                                     \
      type_t ret_val = POP (ret);                                               \
      proc->ip = (unsigned long) ret_val;                                       \
    }

  #define DEF_CMD(num, name, argc, code, hash)                                  \
    case CMD_##name:                                                            \
      code;                                                                     \
      break;

  #define JUMP_WITH_0_ARGS(name, action)                                        \
    {                                                                           \
      if (action)                                                               \
      {                                                                         \
        val = *(type_t *)(proc->code + proc->ip);                               \
        proc->ip = (unsigned long) val - sizeof (Header_t);                     \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        proc->ip += sizeof (type_t);                                            \
      }                                                                         \
    }

  #define JUMP_WITH_2_ARGS(name, action)                                        \
    {                                                                           \
      POP_AB ()                                                                 \
      if (action)                                                               \
      {                                                                         \
        val = *(type_t *)(proc->code + proc->ip);                               \
        proc->ip = (unsigned long) val - sizeof (Header_t);                     \
      }                                                                         \
      else                                                                      \
      {                                                                         \
        proc->ip += sizeof (type_t);                                            \
      }                                                                         \
    }
#endif

#define DEF_JMP_CMD(num, name, argc, action, hash)                              \
  case CMD_##name:                                                              \
    JUMP_WITH_##argc##_ARGS (#name, action);                                    \
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
  unsigned char *code;
  unsigned long bytes_num;
  long unsigned int ip;
  type_t *reg;
  type_t *RAM;
  type_t *video_mem;
  long xRes;
  long yRes;
};

int get_io_args (int argc, const char **argv, Config *curr_config);

int read_code (processor *proc, Config *io_config);

int get_header (processor *proc);

uint64_t run_binary (processor *proc);

uint64_t process_command (processor *proc);

type_t *get_arg (processor *proc, unsigned char cmd);

int dump_proc (processor *proc);
