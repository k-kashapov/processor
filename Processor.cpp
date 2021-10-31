#include "enum.h"
#include "check_unique.h"
#include "Processor.h"

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
  Config io_config;
  io_config.input_file = "code.asm";
  get_io_args (argc, argv, &io_config);

  processor proc = {};
  proc.reg = (type_t *) calloc (REG_NUM, sizeof (type_t));
  proc.RAM = (type_t *) calloc (RAM_MEM, sizeof (type_t));

  stack_t proc_stk = {};
  proc.stk = &proc_stk;

  read_code (&proc, &io_config);

  int header_err = get_header (&proc);
  if (header_err)
    return header_err;

  uint64_t runtime_err = run_binary (&proc);
  if (runtime_err)
    return (int) runtime_err;

  return 0;
}
