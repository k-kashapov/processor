#include "Processor.h"
#include "Info.h"
#include "enum.h"

int run_binary (void *bytes_ptr, int bytes_num)
{
  stack_t proc_stk = {};
  StackInit (proc_stk);

  unsigned char *bytes = (unsigned char *)bytes_ptr;

  for (int ip = 0; ip < bytes_num;)
  {
      unsigned char command = bytes[ip++];

      #ifdef PROC_DUMP
        dump_proc (bytes, bytes_num, ip - 1, 1);
      #endif

      int error = process_command (command, bytes + ip, &ip);

      if (error == HLT_CMD)
      {
        return 0;
      }

      if (error)
      {
        printf ("\nFATAL: command = %d at %d\n", command, ip - 1);
        return error;
      }
  }

  return 0;
}

int process_command_ (stack_t *proc_stk, unsigned char command, unsigned char *current_byte, int *ip)
{
  type_t val = 0;

  switch (command) {
    case PUSH_CMD:
      {
        val = *(type_t *)current_byte;
        push (val);

        *ip += sizeof (uint64_t);
      }
      break;
    case IN_CMD:
      {
        in;
      }
      break;
    case POP_CMD:
      pop;
      break;
    case ADD_CMD:
      add;
      break;
    case SUB_CMD:
      sub;
      break;
    case MUL_CMD:
      mul;
      break;
    case DIV_CMD:
      div;
      break;
    case OUT_CMD:
      out;
      break;
    case HLT_CMD:
      return HLT_CMD;
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
