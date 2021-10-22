#include "SyntaxProcessing.h"
#include "check_unique.h"

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

  read_all_lines (&source, io_config.input_file);

  FILE *output = NULL;
  open_file (&output, "code.dead", "wb");
  if (!output)
    return OPEN_FILE_FAILED;

  Assemble (&source, output);

  fclose (output);

  return 0;
}
