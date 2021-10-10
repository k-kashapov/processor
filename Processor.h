#include "Stack.h"

#define push(value) StackPush (&proc_stk, value)

#define in                           \
  double val = 0;                    \
  printf ("Waiting for input...\n"); \
  scanf("%lf", &val);                 \
  push((uint64_t) (val * 1000))

#define pop StackPop (&proc_stk, NULL)

#define add push (pop + pop)

#define sub                                  \
  {                                          \
    uint64_t b = StackPop (&proc_stk, NULL); \
    uint64_t a = StackPop (&proc_stk, NULL); \
    push (a - b);                            \
  }

#define mul push (pop * pop / 1000)

#define div push (1000.0f / pop * pop)

#define out printf ("%.3lf\n", ((float)StackPop(&proc_stk, NULL)) / 1000)

#define hlt StackDtor (&proc_stk)

#define bgn            \
stack_t proc_stk = {}; \
StackInit(proc_stk)
