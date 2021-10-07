#include "files.h"

int main(int argc, const char** argv)
{
    file_info file;

    config io_config;

    get_params (argc, argv, &io_config);

    read_all_lines (&file, io_config.input_file);

    FILE* out = fopen ("aaa.cpp", "wt");

    fprintf (out, "\x23include \"Processor.h\"\nint main (int argc, const char** argv)\n{\n    bgn();\n");

    for (int line = 0; line < file.lines_num; line++)
    {
        char *command;
        float val = 0;
        sscanf (file.strs[line]->text, "%s %f", command, &val);

        if (!strcmp (command, "push")) fprintf (out, "    push (%f);\n", val);
         
    }

    fprintf (out, "    return 0;\n}"); 

    fclose (out);
    return 0;
}
