#include "../include/Processor.h"
#include "../include/Info.h"

int run_binary (void *bytes_ptr, int bytes_num)
{
  assert (bytes_ptr);
  assert (bytes_num >= 0);

  bgn;

  unsigned char *bytes = (unsigned char *)bytes_ptr;

  for (int ip = 0; ip < bytes_num;)
  {
      unsigned char command = bytes[ip++];

      #ifdef PROC_DUMP
        dump_proc (bytes, bytes_num, ip - 1, 1);
      #endif

      int error = process_command (command, bytes + ip, &ip);

      if (error)
      {
        printf ("\nERROR: INVALID COMMAND CODE: %d at %d\n", command, ip - 1);
        getchar ();
        return INVALID_CODE;
      }
  }

  return 0;
}

int process_command_ (stack_t *proc_stk, unsigned char command, unsigned char *current_byte, int *ip)
{
  type_t val = 0;

  switch (command) {
    case PUSH_CODE:
      {
        val = *(type_t *)current_byte;
        push (val);

        *ip += sizeof (uint64_t);
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
      return INVALID_CODE;
  }

  #ifdef PROC_DUMP
    printf ("com = %d; val = %.3lf\n", command, (double) val / 1000);
  #endif

  return 0;
}

int dump_proc (void *bytes, int bytes_num, int ip, int next_len)
{
  assert (bytes && "Bytes array for dump is empty!");
  FILE *log = fopen ("processor_log.html", "at");
  assert (log);

  fprintf(log, "<pre>");
  for (int printer = 0; printer < bytes_num; printer++)
  {
    fprintf (log, "%02X ", (unsigned char)printer);
  }
  fprintf(log, "\n");
  for (int printer = 0; printer < bytes_num; printer++)
  {
    fprintf (log, "%02X ", ((unsigned char*)bytes)[printer]);
  }
  fprintf(log, "\n%*s%*s", ip*3+1, "^-", next_len * 3 - 1, "-^");

  fprintf (log, "\n---------------------------------------------------\n</pre>");

  fclose (log);

  return 0;
}
