/*****************************************************************//**
 * \file   files.cpp
 * \brief  В файле собраны все функции, связанные с работой с файловой системой
 *********************************************************************/
#include "files.h"

long int read_all_lines (File_info *info, const char* file_name)
{
    assert (info);
    assert (file_name);

    FILE *source = NULL;
    if (open_file (&source, file_name, "rt"))  // rt rt rt!!!
    {
        return OPEN_FILE_FAILED;
    }

    char *text_buff = read_to_end (source);

    if (text_buff == NULL)
    {
        return READ_TEXT_FAILED;
    }

    fclose (source);

    String **strings = (String **) calloc (BUFF_SIZE + 1, sizeof (String *));
    assert (strings);

    String *strings_buff = (String *) calloc (BUFF_SIZE + 1, sizeof (String));
    assert (strings_buff);

    for (int i = 0; i < BUFF_SIZE + 1; i++)
    {
        strings [i] = strings_buff + i;
        assert (strings [i]);
    }

    String **strings_ptr = strings;

    for (char *token = strtok (text_buff, "\n\r"); token; token = strtok (NULL, "\n\r"))
    {
      while (*token == ' ') token++;
      char *token_ptr = token;
      while (*token_ptr != '\n' && *token_ptr) token_ptr++;
      (*strings_ptr)->len = token_ptr - token;
      (*strings_ptr++)->text = token;
    }

    info->text = text_buff;
    info->strs = strings;
    info->strs_buff = strings_buff;
    info->lines_num = strings_ptr - strings;

    return info->lines_num;
}

int open_file (FILE **ptr, const char* file_name, const char* mode)
{
    *ptr = fopen (file_name, mode);
    if (!ptr)
    {
        printf ("ERROR: Couldn't open file \"%s\"\n", file_name);
        return (OPEN_FILE_FAILED);
    }

    return 0;
}

char* read_to_end (FILE *source)
{
    assert (source);

    long unsigned int length = get_len (source);

    char *text_buff = (char *) calloc ( length + 1, sizeof ( char ) );
    assert (text_buff);

    long unsigned int sym_read = fread (text_buff, sizeof (*text_buff), length, source);

    if (sym_read > length)
    {
         free (text_buff);
         printf ("ERROR: Reading text file failed");
         return (NULL);
    }

    // Останавливает дальнейшее чтение, т.к. дальше лежит мусор
    text_buff[sym_read] = '\0';

    return text_buff;
}

long unsigned int get_len (FILE *file)
{
    assert (file);

    fseek (file, 0, SEEK_END);
    long int length = ftell (file);
    fseek (file, 0, SEEK_SET);
    if (length < 0) return 0;
    return (unsigned long int)length;
}

int show_res (File_info *file_text, const char * output_file)
{
    assert (file_text);

    FILE *destination = NULL;
    open_file (&destination, output_file, "wt");

    for (int i = 0; i < file_text->lines_num; i++)
    {
        fputs ((file_text->strs [i])->text, destination);

        fputs ("\n", destination);
        if (feof (destination))
        {
            printf ("ERROR: Writing to file failed!");
            free_info (file_text);
            return (WRITING_TEXT_FAILED);
        }
    }
    fclose (destination);
    return 0;
}

void free_info (File_info *info)
{
    assert (info && "Invalid pointer: File_info");

    free (info->text);
    free (info->strs);
    free (info->strs_buff);
}

void get_params (int argc, const char **argv, Config *current)
{
    assert (argv);
    assert (current);

    while (--argc && 0) // TODO arguments
    {
        const char* arg = *++argv;
        if (!strncmp (arg, "<", 1))
        {
            current->input_file = *++argv;
            argc--;
        }

    }
}
