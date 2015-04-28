#include "auction_server.h"

void read_opts(int argc, char **argv) {
  int opt;
  int opterr = 0;
  port = 0;

  while((opt = getopt(argc, argv, "p:")) != -1) {
    switch(opt) {
      case 'p':
        port = atoi(optarg);
        break;
      default:
        printf("Arg not recognized aborting\n");
        exit(0);
    }
  }
}

void init_thread_list() {
  thread_list = calloc(MAX_CLIENTS, sizeof(thread_list));
}

void create_socket() {
  socket_fd = socket(SERVER_FAMILY, SOCK_STREAM, 0);
}

void *client_server_thread(int *client_fd) {
  char buf[1024];
  while(1) {
    memset(buf, 0, 1024);
    read(*client_fd, buf, 1024);
    if(buf[0] != 0) {
      printf("%s\n", buf);
    }
    sleep(1);
  }
}

int main(int argc, char **argv) {
  init_thread_list();
  read_opts(argc, argv);

  if(port == 0) {
    port = DEFAULT_PORT;
  }

  create_socket();
  
  memset(&sockAddr, 0, sizeof(sockAddr));
  sockAddr.sin_family = SERVER_FAMILY;
  sockAddr.sin_port = htons(port);
  sockAddr.sin_addr.s_addr = htonl(SERVER_HOST);

  bind(socket_fd, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
  listen(socket_fd, 10);

  while(1) {
    int connect_fd = accept(socket_fd, NULL, NULL);

    if(connect_fd != -1) {
      pthread_create(&(*thread_list), NULL, client_server_thread, &connect_fd); 
      thread_list++;
    }
  }
  return 0;
}

