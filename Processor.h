#include "Stack.h"

#define bgn                \
    stack_t proc_stk = {}; \
    StackInit(proc_stk);

#define push (value) StackPush (&proc_stk, value)

#define pop StackPop (&proc_stk, NULL)

#define add push (pop + pop)

#define sub push (-pop + pop)

#define mul push (pop * pop)

#define div push (1.0 / pop * pop)

#define out printf ("%f", StackPop(&proc_stk, NULL))

#define hlt StackDtor (&proc_stk)