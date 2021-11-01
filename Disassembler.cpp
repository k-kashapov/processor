#include "enum.h"
#include "Disassembler.h"
#include "check_unique.h"

// Анекдот
// На занятиях по артиллерийской стрельбе:
// — Пушка стреляет, и снаряд летит по параболе…
// — Товарищ майор, а если пушку на бок положить,
//   то и за угол стрелять можно будет?
// — Можно, но по Уставу не положено.

int main(int argc, const char** argv)
{
  Config io_config;
  io_config.input_file = "code.asm";
  io_config.output_file = "disamed.txt";
  get_io_args (argc, argv, &io_config);

  processor proc = {};

  read_code (&proc, &io_config);

  int header_err = get_header (&proc);
  if (header_err)
    return header_err;

  FILE *output = NULL;
  open_file (&output, io_config.output_file, "wt");
  if (!output)
    return OPEN_FILE_FAILED;

  uint64_t runtime_err = disassemble (&proc);
  if (runtime_err)
    return (int) runtime_err;

  fclose (output);

  return 0;
}
