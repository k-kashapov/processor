#include "Processor.h"
#include "Info.h"

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
