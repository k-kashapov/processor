#include "files.h"

int main(int argc, const char** argv)
{
    file_info file;

    config io_config;

    get_params (argc, argv, &io_config);

    read_all_lines (&file, io_config.input_file);

    FILE* out_blet = fopen ("aaa.cpp", "wt");

    assert (out_blet);

    const char * msg = "\x23include \"Processor.h\"\nint main (int argc, const char** argv)\n{\n    bgn();\n";

    printf ("%d", fwrite ((void *) msg, 1, 84, out_blet));

    for (int line = 0; line < file.lines_num; line++)
    {
        char *command;
        float val = 0;
        sscanf (file.strs[line]->text, "%s %f", command, &val);

        //if (!strcmp (command, "push")) fprintf (out, "push (%1.0f);\n", val);
         
    }

    //fprintf (out, "    return 0;\n}"); 

    fclose (out_blet);
    return 0;
}
