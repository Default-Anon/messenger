#include "../../headers/network.h"
#include <signal.h>

Sock_Setup *setup;

void sig_int_handler() {
  printf("exec interrupted\n");
  close(setup->sock);
  free(setup);
  exit(0);
}

void __attribute__((noreturn)) usage() {
  puts("use ./client <ip> <port>");
  exit(1);
}

int main(int argc, char **argv) {
  signal(SIGINT, sig_int_handler);
  if (argc < 2)
    setup = init_client(DEFAULT_IP, DEFAULT_PORT);
  else
    setup = init_client(argv[1], argv[2]);

  event_loop_client(setup->sock);

  close(setup->sock);
  free(setup);
  return 0;
}
