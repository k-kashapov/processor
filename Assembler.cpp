#include "enum.h"
#include "check_unique.h"
#include "SyntaxProcessing.h"


// Анекдот
// Готовили американцы шпиона, чтобы послать на Украину с самого детства
// взращивали его, научили говорить по-украински
// лучше большинства украинцев и заслали его в село в закарпатье.
// За околицей стоит дед, мимо по дороге идёт американский
// шпион - висячие усы, вышиванка, чуб-оселец.
// - Здорово, диду.
// - Здорово-здорово мериканьский шпиён.
// - Та ты шо, диду, якый мериканьский шпиён, колы я
// гарный хлопэц з соседнёйи дерэвни?
// - Який же ты гарний хлопиц, коли ты нигра губастая?

int main (int argc, const char **argv)
{
  file_info source;
  config io_config;
  get_io_args (argc, argv, &io_config);

  read_all_lines (&source, io_config.input_file);

  FILE *output = NULL;
  open_file (&output, io_config.output_file, "wb");
  if (!output)
    return OPEN_FILE_FAILED;

  printf("output [%p], line = %ld\n", output, __LINE__);


  Assemble (&source, output);
  printf("output [%p], line = %ld\n", output, __LINE__);

  fclose (output);

  printf("output [%p], line = %ld\n", output, __LINE__);

  return 0;
}
