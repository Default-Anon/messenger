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
  puts("use ./server <port>");
  exit(1);
}

int main(int argc, char **argv) {

  signal(SIGINT, sig_int_handler);
  int sock;
  if (argc < 2)
    setup = init_server(DEFAULT_PORT);
  else {
    setup = init_server(argv[1]);
  }

  event_loop(setup->sock);

  close(setup->sock);
  free(setup);

  return 0;
}
