#ifndef __SEND_H__
#define __SEND_H__

#include <string.h>

#define HEADER_LENGTH 10
#define BODY_LENGTH 1024
#define END_LENGTH 10

enum Message_Type
{
  INVALID,
  MSG,
  FILE_MSG
};

typedef struct SendBase
{
  enum Message_Type type;
} Base;

typedef struct SendMessage
{
  enum Message_Type type;
  unsigned char *buf;
} Message;

typedef struct SendFile
{
  enum Message_Type type;
  unsigned long long int raw_size;
  char *path_to_file;
  int begin;
  int end;
  unsigned char body[4096];
} File_Msg;

int find_pattern_from_buf (char *buf, char *pattern);
enum Message_Type get_message_type (unsigned char *msg);

#endif
