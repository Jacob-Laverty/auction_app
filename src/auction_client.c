#include "auction_client.h"

void read_opts(int argc, char **argv) {
  int opt;
  int opterr = 0;
  port = 0;
  host = NULL;

  while((opt = getopt(argc, argv, "p:h:sb")) != -1) {
    switch(opt) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'h':
        host = optarg;
        break;
      case 's':
        cli_type = SELLER;
        break;
      case 'b':
        cli_type = BUYER;
        break;
      default:
        printf("Arg not recognized aborting\n");
        exit(0);
    }
  }
}

void create_socket() {
  socket_fd = socket(SERVER_FAMILY, SERVER_TYPE, 0); 
}

void *server_listen(int *server_fd_listener) {
  printf("Client listener thread initialized\n");
  char server_resp[1024];
  while(1) {
    memset(server_resp, 0, 1024);
    read(*server_fd_listener, server_resp, 1024);
    if(server_resp[0] != 0) {
      printf("%s\n", server_resp);
    }
    sleep(1);
  }
}

int main(int argc, char** argv) {
  read_opts(argc, argv);

  create_socket();

  memset(&sockAddr, 0, sizeof(sockAddr));

  sockAddr.sin_family = SERVER_FAMILY;
  sockAddr.sin_port = htons(port);
  int cli = inet_pton(SERVER_FAMILY, host, &sockAddr.sin_addr);
  connect(socket_fd, (struct sockaddr *)&sockAddr, sizeof(sockAddr));

  pthread_create(&server_listener, NULL, server_listen, &socket_fd);
  printf("Client initialized\n");
  while(1) {
    memset(input_buffer, 0, BUFFER_SIZE);
    fgets(input_buffer, BUFFER_SIZE, stdin);
    if(input_buffer != NULL) {
      write(socket_fd, input_buffer, strlen(input_buffer));
    }
  }

  printf("Client sent\n");
}
