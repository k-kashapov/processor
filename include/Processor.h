#include "Stack.h"

#define push(value) StackPush (proc_stk, value)

#define in                           \
  double val = 0;                    \
  printf ("Waiting for input...\n"); \
  scanf("%lf", &val);                \
  push((uint64_t) (val * 1000))

#define pop StackPop (proc_stk, NULL)

#define add                                  \
  {                                          \
    uint64_t b = StackPop (proc_stk, NULL);  \
    uint64_t a = StackPop (proc_stk, NULL);  \
    push (a + b);                            \
  }

#define sub                                  \
  {                                          \
    uint64_t b = StackPop (proc_stk, NULL);  \
    uint64_t a = StackPop (proc_stk, NULL);  \
    push (a - b);                            \
  }

#define mul push (pop * pop / 1000)

#define div push (1000.0f / pop * pop)

#define out printf ("%.3lf\n", ((double)StackPop(proc_stk, NULL)) / 1000)

#define hlt StackDtor (proc_stk)

#define bgn              \
  stack_t proc_stk = {}; \
  StackInit(proc_stk)

enum Exit_codes
{
  INVALID_FILE = -1,
  INVALID_CODE = -2,
};

#define PROC_DUMP

#define process_command(command, current_byte, ip) process_command_(&proc_stk, command, current_byte, ip)

int run_binary (void *bytes_ptr, int bytes_num);

int process_command_ (stack_t *proc_stk_ptr, unsigned char command, unsigned char *current_byte, int *ip);

int dump_proc (void *bytes, int bytes_num, int ip, int next_len);
