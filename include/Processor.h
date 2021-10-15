#include "Stack.h"

#define pop_ab(cmd_name)                                                  \
  uint64_t pop_err = 0;                                                   \
  type_t b = StackPop (proc_stk, &pop_err);                               \
  type_t a = StackPop (proc_stk, &pop_err);                               \
  if (pop_err)                                                            \
  {                                                                       \
    printf("RUNTIME ERROR: in function: " #cmd_name                       \
    "; error: %06lX\n", pop_err);                                         \
    return pop_err;                                                       \
  }

#define push(value) StackPush (proc_stk, value)

#define in                                                                \
  double val = 0;                                                         \
  printf ("Waiting for input...\n");                                      \
  scanf("%lf", &val);                                                     \
  push((type_t) (val * 1000))

#define pop StackPop (proc_stk, NULL)

#define add                                                               \
  {                                                                       \
    pop_ab (add);                                                         \
    push (a + b);                                                         \
  }

#define sub                                                               \
  {                                                                       \
    pop_ab (sub);                                                         \
    push (a - b);                                                         \
  }

#define mul                                                               \
  {                                                                       \
    pop_ab (mul);                                                         \
    push (a * b / 1000);                                                  \
  }

#define div                                                               \
  {                                                                       \
    pop_ab (div);                                                         \
    if (b == 0)                                                           \
    {                                                                     \
      printf("RUNTIME ERROR: division by zero\n");                        \
      return DIV_BY_ZERO;                                                 \
    }                                                                     \
    push (1000.0f / a * b);                                               \
  }

#define out                                                               \
  {                                                                       \
    uint64_t pop_err = 0;                                                 \
    type_t num = StackPop (proc_stk, &pop_err);                           \
    if (pop_err)                                                          \
    {                                                                     \
      printf("RUNTIME ERROR: in function: out; error: %02lX\n", pop_err); \
      return pop_err;                                                     \
    }                                                                     \
    printf ("%.3lf\n", ((double)num) / 1000);                             \
  }

enum ExitCodes
{
  INVALID_FILE = -1,
  INVALID_CODE = -2,
  DIV_BY_ZERO  = -3,
};

#define PROC_DUMP

#define process_command(command, current_byte, ip) process_command_(&proc_stk, command, current_byte, ip)

int run_binary (void *bytes_ptr, int bytes_num);

int process_command_ (stack_t *proc_stk_ptr, unsigned char command, unsigned char *current_byte, int *ip);

int dump_proc (void *bytes, int bytes_num, int ip, int next_len);
