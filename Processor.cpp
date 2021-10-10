#include "files.h"
#include "Processor.h"
#include "Info.h"

int main(int argc, const char** argv)
{
    FILE *code = fopen ("code.dead", "rb");
    assert (code);

    char *text = read_to_end (code);
    char *text_ptr = text;

    Header_t header = *((Header_t *)text);
    printf ("sign = %x\n", header.signature);
    printf ("ver = %x\n", header.version);
    printf ("chars = %d\n", header.char_num);

    text += sizeof (Header_t);

    bgn;

    printf ("now = %d; header_size = %d\n", text - text_ptr, sizeof (Header_t));

    for (int ip = 0; ip < header.char_num;)
    {
        char command = text[ip++];
        int val = 0;

        printf ("com = %d; val = %d\n", command, val);

        switch (command) {
          case PUSH_CODE:
            {
              val = *(int *)(text+ip);
              push (val);
              ip += sizeof (int);
            }
            break;
          case IN_CODE:
            {
              in;
            }
            break;
          case POP_CODE:
            pop;
            break;
          case ADD_CODE:
            add;
            break;
          case SUB_CODE:
            sub;
            break;
          case MUL_CODE:
            mul;
            break;
        	case DIV_CODE:
            div;
        	   break;
        	case OUT_CODE:
            out;
        	   break;
          case HLT_CODE:
            hlt;
            break;
          default:
            printf ("\nERROR: INVALID COMMAND CODE: %d at %d\n", command, ip - 1);
            break;
        }
        printf ("com = %d; val = %d\n", command, val);
    }

    return 0;
}
