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

  char binary_arr[MAX_BINARY_LEN] = {};

  int char_num = Assemble (&source, binary_arr);
  if (char_num < 1)
  {
    printf("INVALID SYNTAX\n");
    return INVALID_SYNTAX;
  }

  printf ("char num = %d\n", char_num);

  FILE *output = fopen (io_config.output_file, "wb");


  fwrite (binary_arr, 1, char_num, output);

  fclose (output);

  return 0;
}
