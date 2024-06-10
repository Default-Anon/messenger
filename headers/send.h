#ifndef __SEND_H__
#define __SEND_H__

#define HEADER_LENGTH 10
#define BODY_LENGTH 1024
#define END_LENGTH 10

enum Message_Type { INVALID, MSG, FILE_MSG };

typedef struct SendBase {
  enum Message_Type type;
  int raw_size;
} Base;

typedef struct SendMessage {
  enum Message_Type type;
  int raw_size;
  char *buf;
} Message;

typedef struct SendFile {
  enum Message_Type type;
  int raw_size;
  char header[HEADER_LENGTH];
  char body[BODY_LENGTH];
  char end[END_LENGTH];
} File_Msg;

#endif
