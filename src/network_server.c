#include "../headers/network.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

Sock_Setup *
init_server (const char *port)
{
  int sock;
  struct addrinfo addr, *addr_ptr;
  memset (&addr, '\0', sizeof (addr));
  addr.ai_socktype = SOCK_STREAM;
  addr.ai_family = AF_INET;
  addr.ai_flags = AI_PASSIVE;

  sock = socket (addr.ai_family, addr.ai_socktype, IPPROTO_TCP);
  if (getaddrinfo (0, port, (struct addrinfo *)&addr, &addr_ptr))
    {
      perror ("getaddrinfo: ");
      exit (GETADDRINFO_ERROR_CODE);
    }
  if (bind (sock, addr_ptr->ai_addr, addr_ptr->ai_addrlen))
    {
      perror ("bind: ");
      exit (BIND_ERROR_CODE);
    }
  freeaddrinfo (addr_ptr);

  if (listen (sock, MAX_DEVICES_IN_IPv4))
    {
      perror ("listen: ");
      exit (LISTEN_ERROR_CODE);
    }
  Sock_Setup *init = (Sock_Setup *)malloc (sizeof (*init));
  init->sock = sock;
  return init;
}
int
event_loop (int sock)
{
  fd_set fd_desk;
  FD_ZERO (&fd_desk);
  FD_SET (sock, &fd_desk);
  int max_fd = sock;

  while (666)
    {
      fd_set internal_tmp;
      internal_tmp = fd_desk;
      if (select (max_fd + 1, &internal_tmp, 0, 0, 0) < 0)
        {
          perror ("select: ");
          return -4;
        }
      for (int i = 0; i <= max_fd; i++)
        {
          if (FD_ISSET (i, &internal_tmp))
            {
              if (i == sock)
                handle_new_connection (i, &fd_desk, &max_fd);
              else
                handle_new_message (i, &fd_desk);
            }
        }
    }
}

void
handle_new_connection (int main_sock, fd_set *fd_desk, int *max_fd)
{

  struct sockaddr_storage sockaddr;
  socklen_t sockaddr_len = sizeof (sockaddr);
  int connect_sock
      = accept (main_sock, (struct sockaddr *)&sockaddr, &sockaddr_len);
  if (connect_sock == -1)
    {
      perror ("accept: ");
      exit (ACCEPT_ERROR_CODE);
    }
  greetings_new_user (connect_sock);

  FD_SET (connect_sock, &(*fd_desk));
  *max_fd = *max_fd > connect_sock ? *max_fd : connect_sock;
}
void
handle_new_message (int sock_fd, fd_set *fd_desk)
{
  unsigned char *recv_buffer
      = (unsigned char *)malloc (4096 + sizeof (File_Msg));
  memset (recv_buffer, '\0', 4096 + sizeof (File_Msg));
  int recv_status = recv (sock_fd, recv_buffer, 4096 + sizeof (File_Msg), 0);
  if (recv_status == 0)
    {
      free (recv_buffer);
      printf ("client shutdown\n");
      FD_CLR (sock_fd, &(*fd_desk));
    }
  else if (recv_status == -1)
    {
      free (recv_buffer);
      perror ("recv: ");
      exit (RECV_ERROR_CODE);
    }
  else
    {
      File_Msg *msg = (File_Msg *)recv_buffer;
      msg->type = ntohl (msg->type);
      unsigned long long *raw_sz = &msg->raw_size;
      htonll (&raw_sz);
      msg->begin = ntohl (msg->begin);
      msg->end = ntohl (msg->end);

      printf ("\n\n");
      printf ("[Message From User]\n");
      printf ("%s\n", recv_buffer);
      int body_recv_length = recv_status - sizeof (File_Msg) + 4096;
      for (int i = 0; i < body_recv_length; i++)
        {
          printf ("%c ", msg->body[i]);
        }
      printf ("\n\n");
      free (recv_buffer);
    }
}

void
greetings_new_user (int connect_sock)
{
  char buf[4096];
  memset (buf, '\0', sizeof (buf));
  strcpy (buf, "Hello, user\n");
  printf ("hmmm\n");
  int send_status = send (connect_sock, buf, sizeof (buf), 0);
  if (send_status == -1)
    {
      perror ("send");
      exit (SEND_ERROR_CODE);
    }
}
