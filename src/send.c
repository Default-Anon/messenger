#include "../headers/send.h"

enum Message_Type
get_message_type (unsigned char *msg)
{
  switch (*msg)
    {
    case '#':
      return MSG;
    case 'F':
      {
        if (!find_pattern_from_buf (msg, "FILE:"))
          return FILE_MSG;
        else
          return INVALID;
      }
    default:
      return INVALID;
    }
}

int
find_pattern_from_buf (char *buf, char *pattern)
{
  int equal_k = 0;
  while (*buf != '\0')
    {
      if (*buf == *pattern)
        {
          char *buf_cpy = buf;
          char *pattern_cpy = pattern;
          for (int i = 0; i < strlen (pattern); i++)
            {
              if (*buf_cpy == *pattern_cpy)
                equal_k++;
              else
                {
                  equal_k = 0;
                  break;
                }
              buf_cpy++;
              pattern_cpy++;
            }
          if (equal_k == strlen (pattern))
            return 0;
        }
      buf++;
    }
  return 1;
}
void
htonll (unsigned long long **val)
{
  unsigned char buf[8];
  memcpy (buf, (unsigned char *)*val, 8);
  **val = 0;
  int k = 64 - 8; // 8byte type
  for (int i = sizeof (unsigned long long) - 1; i >= 0; i--)
    {
      **val += (buf[i] << k);
      k -= 8;
    }
}
