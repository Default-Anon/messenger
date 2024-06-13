#ifndef __SEND_H__
#define __SEND_H__

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define BODY_LENGTH 4096

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
  unsigned char buf[BODY_LENGTH];
} Message;

typedef struct SendFile
{
  enum Message_Type type;
  unsigned long long int raw_size;
  char *path_to_file;
  int begin;
  int end;
  unsigned char body[BODY_LENGTH];
} File_Msg;

int find_pattern_from_buf (char *buf, char *pattern);
enum Message_Type get_message_type (unsigned char *msg);
int write_to_file (unsigned char *buf, size_t buf_sz);

#endif
