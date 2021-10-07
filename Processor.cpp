#include "files.h"

int main(int argc, const char** argv)
{
    file_info file;

    config io_config;

    get_params (argc, argv, &io_config);

    read_all_lines (&file, io_config.input_file);

    printf ("\x23include \"Processor.h\"\nint main (int argc, const char** argv)\n{\n    bgn();\n");

    for (int line = 0; line < file.lines_num; line++)
    {
        char *command;
        float val = 0;
        sscanf (file.strs[line]->text, "%s %f", command, &val);

        if (!strcmp (command, "push")) printf ("    push (%f);\n", val);
         
    }

    printf ("    return 0;\n}"); 

    return 0;
}
