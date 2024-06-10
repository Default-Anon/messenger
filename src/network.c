#include "../headers/network.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

Sock_Setup *init_server(const char *port) {
  int sock;
  struct addrinfo addr, *addr_ptr;
  memset(&addr, '\0', sizeof(addr));
  addr.ai_socktype = SOCK_STREAM;
  addr.ai_family = AF_INET;
  addr.ai_flags = AI_PASSIVE;

  sock = socket(addr.ai_family, addr.ai_socktype, IPPROTO_TCP);
  if (getaddrinfo(0, port, (struct addrinfo *)&addr, &addr_ptr)) {
    perror("getaddrinfo: ");
    exit(GETADDRINFO_ERROR_CODE);
  }
  if (bind(sock, addr_ptr->ai_addr, addr_ptr->ai_addrlen)) {
    perror("bind: ");
    exit(BIND_ERROR_CODE);
  }
  freeaddrinfo(addr_ptr);

  if (listen(sock, MAX_DEVICES_IN_IPv4)) {
    perror("listen: ");
    exit(LISTEN_ERROR_CODE);
  }
  Sock_Setup *init = (Sock_Setup *)malloc(sizeof(*init));
  init->sock = sock;
  return init;
}

Sock_Setup *init_client(const char *ip, const char *port) {
  int sock;
  struct addrinfo addr, *addr_ptr;
  addr.ai_socktype = SOCK_STREAM;
  addr.ai_family = AF_INET;
  addr.ai_protocol = IPPROTO_TCP;

  sock = socket(addr.ai_family, addr.ai_socktype, addr.ai_protocol);

  if (getaddrinfo(ip, port, &addr, &addr_ptr)) {
    perror("addrinfo :");
    exit(GETADDRINFO_ERROR_CODE);
  }
  if (connect(sock, addr_ptr->ai_addr, addr_ptr->ai_addrlen)) {
    perror("connect: ");
    exit(CONNECT_ERROR_CODE);
  }
  freeaddrinfo(addr_ptr);
  Sock_Setup *setup = (Sock_Setup *)malloc(sizeof(*setup));
  setup->sock = sock;
  return setup;
}
void handle_new_connection(int main_sock, fd_set *fd_desk, int *max_fd) {

  struct sockaddr_storage sockaddr;
  socklen_t sockaddr_len = sizeof(sockaddr);
  int connect_sock =
      accept(main_sock, (struct sockaddr *)&sockaddr, &sockaddr_len);
  if (connect_sock == -1) {
    perror("accept: ");
    exit(ACCEPT_ERROR_CODE);
  }
  greetings_new_user(connect_sock);

  FD_SET(connect_sock, &(*fd_desk));
  *max_fd = *max_fd > connect_sock ? *max_fd : connect_sock;
}
void handle_new_message(int sock_fd, fd_set *fd_desk) {
  char *recv_buffer = (char *)malloc(4096);
  memset(recv_buffer, '\0', 4096);
  int recv_status = recv(sock_fd, recv_buffer, 4096, 0);
  if (recv_status == 0) {
    free(recv_buffer);
    printf("client shutdown\n");
    FD_CLR(sock_fd, &(*fd_desk));
  } else if (recv_status == -1) {
    free(recv_buffer);
    perror("recv: ");
    exit(RECV_ERROR_CODE);
  } else {
    printf("[Message From User]\n");
    printf("%s\n", recv_buffer);
    free(recv_buffer);
  }
}
int event_loop(int sock) {
  fd_set fd_desk;
  FD_ZERO(&fd_desk);
  FD_SET(sock, &fd_desk);
  int max_fd = sock;

  while (666) {
    fd_set internal_tmp;
    internal_tmp = fd_desk;
    if (select(max_fd + 1, &internal_tmp, 0, 0, 0) < 0) {
      perror("select: ");
      return -4;
    }
    for (int i = 0; i <= max_fd; i++) {
      if (FD_ISSET(i, &internal_tmp)) {
        if (i == sock) {
          handle_new_connection(i, &fd_desk, &max_fd);
        } else {
          handle_new_message(i, &fd_desk);
        }
      }
    }
  }
}
int event_loop_client(int sock) {
  while (666) {
    fd_set internal_tmp;
    FD_ZERO(&internal_tmp);
    FD_SET(sock, &internal_tmp);
    FD_SET(STDOUT_FILENO, &internal_tmp);
    int max_fd = sock;
    struct timeval timee;
    timee.tv_sec = 1;
    if (select(max_fd + 1, &internal_tmp, 0, 0, &timee) < 0) {
      perror("select");
      exit(SELECT_ERROR_CODE);
    }
    if (FD_ISSET(sock, &internal_tmp)) {
      char recv_buffer[4096];
      int receive_bytes = recv(sock, recv_buffer, 4096, 0);

      printf("receive_bytes is %d\n", receive_bytes);
      if (receive_bytes < 1) {
        printf("Access to server forbidden or he inactive now\n");
        FD_CLR(sock, &internal_tmp);
        return 0;
      }
      printf("from server: %s\n", recv_buffer);
    }
    if (FD_ISSET(STDIN_FILENO, &internal_tmp)) {
      char sent_buffer[4096];
      memset(sent_buffer, 0, sizeof(sent_buffer));
      if (!fgets(sent_buffer, sizeof(sent_buffer), STDIN_FILENO)) {
        perror("fgets");
        return FGETS_ERROR_CODE;
      }
      send(sock, sent_buffer, sizeof(sent_buffer), 0);
    }
  }
  return 0;
}

void greetings_new_user(int connect_sock) {
  char buf[4096];
  memset(buf, '\0', sizeof(buf));
  strcpy(buf, "Hello, user\n");
  printf("hmmm\n");
  int send_status = send(connect_sock, buf, sizeof(buf), 0);
  if (send_status == -1) {
    perror("send");
    exit(SEND_ERROR_CODE);
  }
}
