#ifndef __NETWORK_H__
#define __NETWORK_H__

#define __DEBUG_

#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_IP "192.168.0.126"
#define DEFAULT_PORT "9090"
#define MAX_DEVICES_IN_IPv4 0xFF

typedef struct Sock_Setup {
  int sock;
} Sock_Setup;

typedef enum Error_Code {
  GETADDRINFO_ERROR_CODE = -1,
  BIND_ERROR_CODE = -2,
  LISTEN_ERROR_CODE = -3,
  SELECT_ERROR_CODE = -4,
  ACCEPT_ERROR_CODE = -5,
  SEND_ERROR_CODE = -6,
  RECV_ERROR_CODE = -7,
  CONNECT_ERROR_CODE = -8,
  FGETS_ERROR_CODE = -9
} Error_Code;

int event_loop(int sock);
int event_loop_client(int sock);
Sock_Setup *init_server(const char *port);
Sock_Setup *init_client(const char *ip, const char *port);
void greetings_new_user(int connect_sock);

#endif
