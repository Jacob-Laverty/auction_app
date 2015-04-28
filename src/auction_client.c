#include "auction_client.h"

void read_opts(int argc, char **argv) {
  int opt;
  int opterr = 0;
  port = 0;

  while((opt = getopt(argc, argv, "p:h:")) != -1) {
    switch(opt) {
      case 'p':
        port = atoi(optarg);
        break;
      case 'h':
        host = optarg;
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

int main(int argc, char** argv) {
  read_opts(argc, argv);

  create_socket();

  memset(&sockAddr, 0, sizeof(sockAddr));

  sockAddr.sin_family = SERVER_FAMILY;
  sockAddr.sin_port = htons(port);
  int cli = inet_pton(SERVER_FAMILY, host, &sockAddr.sin_addr);
  connect(socket_fd, (struct sockaddr *)&sockAddr, sizeof(sockAddr));

  char str[] = "hello!";
  int datalen = strlen(str);
  write(socket_fd, str, datalen);

  printf("Client sent\n");
}
