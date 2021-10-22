#include "Processor.h"
#include "check_unique.h"

// Анекдот
// Идёт грузин с бумерангом, весь в
// синяках и ссадинах. Hавстречу ему другой:
// - Вано, эта чито у тэбя в рукэ?
// - Hэ знаю.
// - А зачэм она тибэ?
// - Hэ знаю.
// - Так ти её викинь.
// - Hа, билят, сам викинь!

int main(int argc, const char** argv)
{
    processor proc = {};
    stack_t proc_stk = {};
    proc.stk = &proc_stk;

    read_code (&proc);

    int header_err = get_header (&proc);
    if (header_err)
      return header_err;

    int runtime_err = run_binary (&proc);
    if (runtime_err)
      return runtime_err;

    return 0;
}
