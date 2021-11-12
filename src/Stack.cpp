#include "Stack.h"

const int BUFFER_INIT_SIZE = 1;

FILE* Log_file = NULL;

const char *_type_name = "double";

#ifdef CANARY_PROTECTION
    const uint64_t CANARY_VAL = 0xC0DEDEADC0DEDEAD;
#endif

const int POISON    = 0x42;
uint64_t  Stack_Err = 0;

#ifdef DEBUG_INFO
    #define DEBUG_ME_HONEY(...)  __VA_ARGS__
#else
    #define DEBUG_ME_HONEY(...)
#endif

uint64_t StackInit_ (stack_t *stk
                     DEBUG_ME_HONEY (, const char *file_name, const char *func_name, const int line, const char *name))
{
      stk->buffer = NULL;

      stk->capacity = 0;
      stk->size = 0;

      #ifdef DEBUG_INFO
        if (file_name && func_name && line > 0 && name)
        {
            stk->file = file_name;
            stk->func = func_name;
            stk->line = line;
            stk->name = name;

            Log_file = fopen ("log.html", "a");
        }
      #endif

      #ifdef CANARY_PROTECTION
          stk->canary_l = CANARY_VAL ^ (uint64_t) stk;
          stk->canary_r = CANARY_VAL ^ (uint64_t) stk;
      #endif

      #ifdef HASH_PROTECTION
          unsigned long len = sizeof (*stk) - sizeof (stk->struct_hash) - sizeof (stk->data_hash);
          stk->struct_hash = MurmurHash (stk, len);
          stk->data_hash = MurmurHash (stk->buffer, (unsigned long) stk->size * sizeof (type_t));
      #endif

      STACK_OK (stk);
      if (Stack_Err) return Stack_Err;
      return OK;
  }

uint64_t StackError (stack_t *stk)
{
    uint64_t StkErrors = 0;

    #ifdef CANARY_PROTECTION
        uint64_t canary_check = CANARY_VAL ^ (uint64_t) stk;
    #endif

    if (stk->size > stk->capacity)
    {
        StkErrors |= SIZE_OVER_CAPACITY;
    }

    if (stk->buffer == (type_t *)POISON)
    {
        StkErrors |= BAD_PTR;
        return StkErrors;
    }

    #ifdef HASH_PROTECTION
        long unsigned int stk_hash = 0;
        COUNT_STACK_HASH (stk, stk_hash)

        if (stk_hash != stk->struct_hash)
        {
            StkErrors |= STACK_HASH_INVALID;
        }

        long unsigned int data_hash = 0;
        COUNT_DATA_HASH (stk, data_hash)

        if (data_hash != stk->data_hash)
        {
            StkErrors |= DATA_HASH_INVALID;
        }
    #endif

    #ifdef CANARY_PROTECTION
        if (stk->canary_l != canary_check)
        {
            StkErrors |= LEFT_CANARY_DEAD;
        }
        if (stk->canary_r != canary_check)
        {
            StkErrors |= RIGHT_CANARY_DEAD;
        }

        if (stk->buffer)
        {
            if (*((uint64_t *)stk->buffer - 1) != canary_check)
            {
                StkErrors |= LEFT_DATA_CANARY_DEAD;
            }
            if (*((uint64_t *)(stk->buffer + stk->capacity)) != canary_check)
            {
                StkErrors |= RIGHT_DATA_CANARY_DEAD;
            }
        }
    #endif

    #ifdef CHECK_POISON
        for (long pois_iter = stk->size; pois_iter > 0 && pois_iter < stk->capacity; pois_iter++)
        {
            if (stk->buffer [pois_iter] != POISON)
            {
                StkErrors |= NOT_POISONED_BEYOND_SIZE;
                break;
            }
        }
    #endif

    return StkErrors;
}

uint64_t StackDump (stack_t *stk, uint64_t err, const char *called_from, const int line_called_from)
{
  if (!Log_file) return LOG_NOT_OPENED;
  #ifdef DEBUG_INFO
    const char *err_string = err ? "<em style = \"color : red\">ERROR</em>" : "<em style = \"color : #00FA9A\">ok</em>";
    fprintf (Log_file, "<pre>[%s] [%s] Stack &#60%s&#62 [&%p] \"%s\" %s at %s at %s (%d); called from %s (%d)\n</pre>",\
                       __DATE__, __TIME__, _type_name, stk, stk->name, err_string, stk->func, stk->file, stk->line, called_from, line_called_from);
  #else
    const char *err_string = err ? "<em style = \"color : red\">ERROR</em>" : "<em style = \"color : #00FA9A\">ok</em>";
    fprintf (Log_file, "<pre>[%s] [%s] Stack &#60%s&#62 [&%p] %s; called from %s (%d)\n</pre>",\
                       __DATE__, __TIME__, _type_name, stk, err_string, called_from, line_called_from);
  #endif

  if (err)
  {
      PRINT_ERR (err, BAD_PTR);
      PRINT_ERR (err, ZERO_ELEM_POP);
      PRINT_ERR (err, CAPACITY_UNDER_ZERO);
      PRINT_ERR (err, SIZE_UNDER_ZERO);
      PRINT_ERR (err, SIZE_OVER_CAPACITY);
      PRINT_ERR (err, LEFT_CANARY_DEAD);
      PRINT_ERR (err, RIGHT_CANARY_DEAD);
      PRINT_ERR (err, LEFT_DATA_CANARY_DEAD);
      PRINT_ERR (err, RIGHT_DATA_CANARY_DEAD);
      PRINT_ERR (err, NOT_POISONED_BEYOND_SIZE);
      PRINT_ERR (err, STACK_HASH_INVALID);
      PRINT_ERR (err, DATA_HASH_INVALID);
  }

  #ifdef MAX_INFO
      if (1)
  #else
      if (err)
  #endif
  {
      fprintf (Log_file, "<pre>{\n\tcapacity = %ld;\n\tsize = %ld;",\
               stk->capacity, stk->size);

      #ifdef CANARY_PROTECTION
          fprintf (Log_file, "\n\tcanary_l = %lu;\n\tcanary_r = %lu;", stk->canary_l ^ (uint64_t)stk, stk->canary_r ^ (uint64_t)stk);
      #endif

      #ifdef HASH_PROTECTION
          long unsigned int stk_hash = 0;
          COUNT_STACK_HASH (stk, stk_hash);

          long unsigned int data_hash = 0;
          COUNT_DATA_HASH (stk, data_hash);
          fprintf (Log_file, "\n\tstruct_hash = %lu;\n\texpected struct_hash = %lu;\n\tdata_hash = %lu;\n\texpected data_hash = %lu;", stk_hash, stk->struct_hash, data_hash, stk->data_hash);
      #endif

      fprintf (Log_file, "\n\tbuffer &#60%s&#62 [&%p]\n\t{\n", _type_name, stk->buffer);

      if ((err & BAD_PTR) || !stk->buffer)
      {
          fprintf (Log_file, "\n\t}\n}\n </pre>");
          return err;
      }

      #ifdef CANARY_PROTECTION
          fprintf (Log_file, "\t\t left_canary = %lu\n", *((uint64_t *)stk->buffer - 1) ^ (uint64_t) stk);
      #endif

      for (int buff_iter = 0; buff_iter < stk->capacity; buff_iter++)
      {
          const char *poison = stk->buffer[buff_iter] == POISON ? "POISON" : "";
          if (buff_iter < stk->size)
          {
              fprintf (Log_file, "\t\t*[%d] = %ld\n", buff_iter, stk->buffer[buff_iter]);
          }
          else
          {
              fprintf (Log_file, "\t\t [%d] = %ld %s\n", buff_iter, stk->buffer[buff_iter], poison);
          }
      }

      fprintf (Log_file, "\t\t right_canary = %lu\n\t}\n}</pre>", *((uint64_t *)(stk->buffer + stk->capacity)) ^ (uint64_t) stk);
  }

    return 0;
}

uint64_t StackResize (stack_t *stk, long new_capacity)
{
    STACK_OK (stk);
    if (Stack_Err) return Stack_Err;

    if (new_capacity <= 0) new_capacity = BUFFER_INIT_SIZE;

    #ifdef CANARY_PROTECTION
        unsigned long buff_len = (unsigned long) new_capacity * sizeof (type_t) + 2 * sizeof (CANARY_VAL);
        uint64_t canary = CANARY_VAL ^ (uint64_t) stk;

        if (stk->capacity == 0)
        {
            REALLOC (stk->buffer, stk->capacity, buff_len, type_t);

            *(uint64_t *)stk->buffer = canary;
            stk->buffer = (type_t *)((uint64_t *)stk->buffer + 1);
        }
        else
        {
            uint64_t *tmp_buff = (uint64_t *) ((char *) stk->buffer - sizeof (CANARY_VAL));
            REALLOC (tmp_buff, stk->capacity, buff_len, uint64_t);
            stk->buffer = (type_t *)(tmp_buff + 1);
        }

        *(uint64_t *)(stk->buffer + new_capacity) = canary;
    #else
        int buff_len = new_capacity * sizeof (type_t);

        REALLOC (stk->buffer, stk->capacity, buff_len, type_t);
    #endif

    stk->capacity = new_capacity;

    for (long pois_iter = stk->size; pois_iter < stk->capacity; pois_iter++)
    {
        stk->buffer [pois_iter] = POISON;
    }

    #ifdef HASH_PROTECTION
        COUNT_STACK_HASH (stk, stk->struct_hash);
        COUNT_DATA_HASH  (stk, stk->data_hash);
    #endif

    STACK_OK (stk);
    if (Stack_Err) return Stack_Err;
    return OK;
}

uint64_t StackPush (stack_t* stk, type_t value)
{
    STACK_OK (stk);
    if (Stack_Err) return Stack_Err;

    if (stk->size >= stk->capacity)
    {
        uint64_t resize_error = StackResize (stk, stk->size * 2);
        if (resize_error) return resize_error;
    }

    assert (stk->buffer);
    stk->buffer[stk->size++] = value;

    #ifdef HASH_PROTECTION
        COUNT_STACK_HASH (stk, stk->struct_hash);
        COUNT_DATA_HASH (stk, stk->data_hash);
    #endif

    STACK_OK (stk);
    if (Stack_Err) return Stack_Err;
    return OK;
}

type_t StackPop (stack_t* stk, uint64_t *err_ptr)
{
    STACK_OK (stk);
    if (Stack_Err)
    {
        *err_ptr = Stack_Err;
        return 0;
    }

    if (stk->size < 1)
    {
        if (Log_file) fprintf (Log_file, "<p style = \"color : red\">ERROR: zero elements pop</p>");
        if (err_ptr) *err_ptr = ZERO_ELEM_POP;
        return ZERO_ELEM_POP;
    }

    type_t copy = *(stk->buffer + stk->size - 1);

    stk->buffer[--stk->size] = POISON;

    #ifdef HASH_PROTECTION
        COUNT_STACK_HASH (stk, stk->struct_hash);
        COUNT_DATA_HASH (stk, stk->data_hash);
    #endif

    if (stk->size <= stk->capacity / 4)
    {
        uint64_t resize_error = StackResize (stk, stk->capacity / 2);
        if (resize_error)
        {
            if (err_ptr) *err_ptr = resize_error;
            return -1;
        }
    }

    STACK_OK (stk);
    if (Stack_Err)
    {
        *err_ptr = Stack_Err;
        return 0;
    }
    return copy;
}

type_t StackTop (stack_t *stk, uint64_t *err_ptr)
{
    STACK_OK (stk);
    if (Stack_Err)
    {
        *err_ptr = Stack_Err;
        return 0;
    }

    if (err_ptr)
    {
        if (stk->size < 1)
        {
            if (Log_file) fprintf (Log_file, "<p style = \"color : red\">ERROR: zero elements top</p>");
            *err_ptr = ZERO_ELEM_POP;
            return ZERO_ELEM_POP;
        }
    }

    return stk->buffer[stk->size - 1];
}

unsigned long MurmurHash (const void *stk, unsigned long len)
{
  if (!stk) return 0;

  unsigned int ilen = (unsigned int) len;

  const unsigned int m = 0x5bd1e995;
  const unsigned int seed = 0;
  const int r = 24;

  unsigned int h = seed ^ ilen;

  const unsigned char * data = (const unsigned char *)stk;
  unsigned int k = 0;

  while (ilen >= 4)
  {
    k  = (unsigned int)  data[0];
    k |= (unsigned int) (data[1] << 8);
    k |= (unsigned int) (data[2] << 16);
    k |= (unsigned int) (data[3] << 24);

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    ilen -= 4;
  }

  switch (len)
  {
    case 3:
      h ^= (unsigned int) (data[2] << 16);
      [[fallthrough]];
    case 2:
      h ^= (unsigned int) data[1] << 8;
      [[fallthrough]];
    case 1:
      h ^= data[0];
      h *= m;
      [[fallthrough]];
    default: break;
  };

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

uint64_t StackDtor (stack_t *stk)
{
    STACK_OK (stk);
    if (Stack_Err) return Stack_Err;

    #ifdef CANARY_PROTECTION
        free ((uint64_t *)stk->buffer - 1);
    #else
        free (stk->buffer);
    #endif

    stk->buffer = (type_t *)POISON;

    stk->capacity = 0;
    stk->size = 1;

    #ifdef DEBUG_INFO
        if (Log_file)
        {
            fprintf (Log_file, "<em style = \"color : #00FA9A\">Stack Destructed</em>");
            fclose (Log_file);
        }
    #endif
    return OK;
}
