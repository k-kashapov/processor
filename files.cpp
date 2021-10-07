/*****************************************************************//**
 * \file   files.cpp
 * \brief  В файле собраны все функции, связанные с работой с файловой системой
 *********************************************************************/
#include "files.h"

//  Убирает предупреждение о функциях библиотеки string.h в Visual Studio
#pragma warning(disable:4996)

int read_all_lines (file_info *info, const char* file_name)
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

    string **strings = (string **) calloc (BUFF_SIZE + 1, sizeof (string *)); //TODO arr of struct
    assert (strings);

    for (int i = 0; i < BUFF_SIZE + 1; i++)
    {
        strings [i] = (string *) calloc (1, sizeof (string));
        assert (strings [i]);
    }

    string **strings_ptr = strings;

    for (char *token = strtok (text_buff, "\n"); token; token = strtok (NULL, "\n"))
    {
        if (*token != '\n') 
        {           
            while (*token == ' ') token++;
            char *token_ptr = token;
            while (*token_ptr != '\n' && *token_ptr) token_ptr++;
            (*strings_ptr)->len = token_ptr - token;
            (*strings_ptr++)->text = token;
        } 
    } 
    
    info->text = text_buff;
    info->strs = strings;
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
    
    int length = get_len (source);

    char *text_buff = (char *) calloc ( length + 1, sizeof ( char ) );
    assert (text_buff);

    int sym_read = fread (text_buff, sizeof (*text_buff), length, source);
    
    if (sym_read < 0 || sym_read > length)
    {
         free (text_buff);
         printf ("ERROR: Reading text file failed");
         return (NULL);
    }

    // Останавливает дальнейшее чтение, т.к. дальше лежит мусор
    text_buff[sym_read] = '\0';
    
    return text_buff;
}

int get_len (FILE *file)
{
    assert (file);

    fseek (file, 0, SEEK_END);
    int length = ftell (file); 
    fseek (file, 0, SEEK_SET);

    return length;
}

int show_res (file_info *file_text, const char * output_file)
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

void free_info (file_info *info)                                                        
{
    assert (info && "Invalid pointer: file_info");

    free (info->text);
    free (info->strs);
}

void get_params (int argc, const char **argv, config *current)
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
