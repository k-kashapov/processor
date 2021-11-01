#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

/// <summary>
/// Максимальное количество строк, которые можно прочиать из файла
/// </summary>
const int BUFF_SIZE = 500;

enum FilesExitCodes
{
  READ_TEXT_FAILED =    -1,
  WRITING_TEXT_FAILED = -2,
  OPEN_FILE_FAILED =    -3
};

/**
 * \brief Структура, содержащая строку и её длину
 */
struct String
{
    char *text;
    long int len;
};

/**
 * \brief Структура, содержащая конфигурацию программы
 *
 * \param input_file  Имя файла, откуда производится чтение
 * \param output_file Имя файла для записи
 * \param mode        Режим сортировки строк
 * \param reverse     bool Сортировать строки по их окончанию
 */
struct Config
{
    const char *input_file  = "source.txt";
    const char *output_file = "code.asm";
};

/**
 * \brief Структура, содержащая в себе полный текст файла и информацию о нём
 *
 * \param text      Полный текст файла
 * \param str_ptrs  Массив указателей на строки в файле
 * \param lines_num Количество строк
 */
struct File_info
{
    char *text;
    String **strs;
    long int  lines_num;
    String *strs_buff;
};

/**
 * \brief Читает все строки из файла
 *
 * \param  info         Указатель в который будет записана информация о файле
 * \param  file_name    Имя файла, который будет прочитан
 * \return              Количество прочитанных строк
 */
long int read_all_lines (File_info *info, const char *file_name);

/**
 * \brief Пробует открыть файл.
 */
int open_file (FILE **ptr, const char* file_name, const char* mode);

/**
 * \brief Читает файл, помещает все символы в буфер
 *
 * \param  source Файл, который нужно прочитать
 * \return        Указатель на буфер
 */
char* read_to_end (FILE *source);

/**
 * \brief Возвращает количество символов в файле
 *
 * \param  file Указатель на файл
 * \return      Длина файла
 */
long unsigned int get_len (FILE *file);

/**
 * \brief Записывает все строки из структуры source в файл output_file
 *
 * \param source      Структура, откуда будут напечатаны строки
 * \param output_file Название файла, в который необходимо напечатать строки
 */
int show_res (File_info *source, const char * output_file);

/**
 * \brief Очищает строки, содержащиеся в структуре info
 *
 * \param info Структура, память которой будет очищена
 */
void free_info (File_info *info);

/**
 * \brief Получает список аргументов программы. Аргументы задают имена входного и выходного файлов
 *
 * \param argc    Количество аргументов программы
 * \param argv    Массив аргументов
 * \param current Структура, хранящая имена файлов
 */
void get_params (int argc, const char **argv, Config *current);
