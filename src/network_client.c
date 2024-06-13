#include "../headers/network.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

Sock_Setup *
init_client (const char *ip, const char *port)
{
  int sock;
  struct addrinfo addr, *addr_ptr;
  addr.ai_socktype = SOCK_STREAM;
  addr.ai_family = AF_INET;
  addr.ai_protocol = IPPROTO_TCP;

  sock = socket (addr.ai_family, addr.ai_socktype, addr.ai_protocol);

  if (getaddrinfo (ip, port, &addr, &addr_ptr))
    {
      perror ("addrinfo :");
      exit (GETADDRINFO_ERROR_CODE);
    }
  if (connect (sock, addr_ptr->ai_addr, addr_ptr->ai_addrlen))
    {
      perror ("connect: ");
      exit (CONNECT_ERROR_CODE);
    }
  freeaddrinfo (addr_ptr);
  Sock_Setup *setup = (Sock_Setup *)malloc (sizeof (*setup));
  setup->sock = sock;
  return setup;
}
int
event_loop_client (int sock)
{
  F_A_Q ();
  while (666)
    {
      fd_set internal_tmp;
      FD_ZERO (&internal_tmp);
      FD_SET (sock, &internal_tmp);
      FD_SET (STDIN_FILENO, &internal_tmp);
      int max_fd = sock;
      struct timeval timee;
      timee.tv_sec = 0;
      if (select (max_fd + 1, &internal_tmp, 0, 0, &timee) < 0)
        {
          perror ("select");
          exit (SELECT_ERROR_CODE);
        }
      if (FD_ISSET (sock, &internal_tmp))
        {
          char recv_buffer[4096];
          int receive_bytes = recv (sock, recv_buffer, 4096, 0);

          printf ("receive_bytes is %d\n", receive_bytes);
          if (receive_bytes < 1)
            {
              printf ("Access to server forbidden or he inactive now\n");
              FD_CLR (sock, &internal_tmp);
              return 0;
            }
          printf ("from server: %s\n", recv_buffer);
        }
      if (FD_ISSET (STDIN_FILENO, &internal_tmp))
        {
#define SENT_BUFFER_SIZE 4096
          unsigned char *sent_buffer
              = (unsigned char *)malloc (SENT_BUFFER_SIZE);
          memset (sent_buffer, 0, SENT_BUFFER_SIZE);
          if (!fgets (sent_buffer, SENT_BUFFER_SIZE, stdin))
            {
              perror ("fgets");
              return FGETS_ERROR_CODE;
            }
          enum Message_Type type = get_message_type (sent_buffer);
          switch (type)
            {
            case MSG:
              {
                printf ("type is MSG\n");
                Message *msg = (Message *)malloc (sizeof (Message));
                strcpy (msg->buf, sent_buffer + 1);
                handle_message_sent (sock, msg);
                break;
              }
            case FILE_MSG:
              {
                printf ("type is FILE_MSG\n");
                File_Msg *msg = (File_Msg *)malloc (sizeof (File_Msg));
                printf ("%s\n", sent_buffer);
                char *enter_symbol = strchr (sent_buffer, '\n');
                *enter_symbol = '\0';
                msg->type = FILE_MSG;
                msg->path_to_file = sent_buffer + 5; /* 5 because "FILE:" */
                handle_file_sent (sock, msg);
                break;
              }
            default:
              {
                printf ("type is INVALID\n");
                int sent_bytes
                    = send (sock, sent_buffer, strlen (sent_buffer), 0);

#ifdef __DEBUG_
                printf ("bytes sended %d\n", sent_bytes);
#endif
                break;
              }
            }
          free (sent_buffer);
        }
    }
  return 0;
}

void
handle_message_sent (int sock, Message *msg)
{
  int sent_bytes = send (sock, msg->buf, strlen (msg->buf), 0);
#ifdef __DEBUG_
  printf ("sent_bytes is %d\n", sent_bytes);
#endif
}
void
handle_file_sent (int sock, File_Msg *fle)
{
  printf ("We are in handle_file_sent\n");
  printf ("path is %s\n", fle->path_to_file);
  FILE *file = fopen (fle->path_to_file, "rb");
  if (!file)
    {
      perror ("fopen");
    }
  else
    {
      fseek (file, 0, SEEK_END);
      fle->raw_size = ftell (file);
      rewind (file);
#ifdef __DEBUG_
      printf ("File %s size is %llu\n", fle->path_to_file, fle->raw_size);
#endif
      fle->begin = 1;
      int raw_size = fle->raw_size;
      while (raw_size)
        {
          printf ("raw_size is %d\n", raw_size);
          memset (fle->body, 0, BODY_LENGTH);
          int read_bytes = fread (fle->body, sizeof (*fle->body),
                                  sizeof (fle->body), file);

          size_t file_msg_sz = sizeof (*fle);
          unsigned char buffer[file_msg_sz];
          File_Msg big_endian_convert = *fle;
          unsigned long long *raw_sz = &big_endian_convert.raw_size;
          htonll (&raw_sz);
          big_endian_convert.type = htonl (big_endian_convert.type);
          big_endian_convert.begin = htonl (big_endian_convert.begin);
          big_endian_convert.end = htonl (big_endian_convert.end);
          memcpy (buffer, &big_endian_convert, file_msg_sz);

          if (raw_size <= BODY_LENGTH)
            {
              fle->end = 1;
              printf ("read_bytes is %d\n", read_bytes);
              printf ("file_msg_sz is %zu\n", file_msg_sz);
              int sent_bytes = send (sock, buffer, file_msg_sz, 0);
              printf ("bytes whether sent is equal %d\n", sent_bytes);
              fle->raw_size = raw_size = 0;
            }
          else
            {
              fle->end = 0;
              printf ("read_bytes is %d\n", read_bytes);
              printf ("file_msg_sz is %zu\n", file_msg_sz);
              int sent_bytes = send (sock, buffer, file_msg_sz, 0);
              printf ("bytes whether sent is equal %d\n", sent_bytes);

              usleep (DELAY_AFTER_SEND_FILE_CHUNK);

              fle->raw_size = raw_size -= read_bytes;
            }
        }

      fclose (file);
    }
}
void
F_A_Q ()
{
  printf ("If you want to write simple message write #<your message>\n");
  printf ("If you want to get file from server write FILE:<path_to_file>\n");
  printf ("If you want to exit do whatever you want, sucker\n");
}
