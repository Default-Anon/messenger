#include "../headers/send.h"

enum Message_Type get_message_type(Base *msg) {
  switch (msg->type) {
  case MSG:
    return MSG;
  case FILE_MSG:
    return FILE_MSG;
  default:
    return INVALID;
  }
}
