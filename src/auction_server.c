#include "auction_server.h"

void read_opts(int argc, char **argv) {
  int opt;
  int opterr = 0;

  while((opt = getopt(argc, argv, "s:b:")) != -1) {
    switch(opt) {
      case 's':
        seller_port = atoi(optarg);
        break;
      case 'b':
        buyer_port = atoi(optarg);
        break;
      default:
        printf("Arg not recognized aborting\n");
        exit(0);
    }
  }
}

void init_thread_list() {
  thread_list = calloc(MAX_BUYERS+MAX_SELLERS, sizeof(thread_list));
}

void create_sockets() {
  socket_seller_fd = socket(SERVER_FAMILY, SOCK_STREAM, 0);
  socket_buyer_fd = socket(SERVER_FAMILY, SOCK_STREAM, 0);
}

void *client_server_thread(int *client_fd) {
  char client_name[32];
  memset(client_name, 0, 32);
  char buf[1024];
  char server_resp[1024];
  while(1) {
    memset(buf, 0, 1024);
    memset(server_resp, 0 ,1024);
    read(*client_fd, buf, 1024);
    if(buf[0] != 0) {
      char command[32]; 
      sscanf(buf, "%s", command);
      printf("Server Received %s\n", command);

      //LOGIN
      if((strcmp(LOGIN, command)) == 0) {
        if(client_name[0] != 0) {
          strncpy(server_resp, "Client is already logged in!", 1024);
          write(*client_fd, server_resp, strlen(server_resp));
        } else {
          sscanf(buf, "%s %s", command, client_name);
          snprintf(&server_resp, 1024, "Client logged in as %s", client_name);
          write(*client_fd, server_resp, strlen(server_resp));
        }
      }
      //LIST
      else if (strcmp(LIST, command) == 0 && client_name[0] != 0) {
      }
      //SELL
      else if (strcmp(SELL, command) == 0 && client_name[0] != 0) {
      }
      //ADD
      else if (strcmp(ADD, command) == 0 && client_name[0] != 0) {
      }
      //BID
      else if (strcmp(BID, command) == 0 && client_name[0] != 0) {
      } 
      //Else
      else {
      }
    }
    sleep(1);
  }
}

int main(int argc, char **argv) {
  init_thread_list();
  read_opts(argc, argv);
  create_sockets();
  
  memset(&buyer_sock_addr, 0, sizeof(buyer_sock_addr));
  memset(&seller_sock_addr, 0, sizeof(seller_sock_addr));

  buyer_sock_addr.sin_family = SERVER_FAMILY;
  seller_sock_addr.sin_family = SERVER_FAMILY;

  buyer_sock_addr.sin_port = htons(buyer_port);
  seller_sock_addr.sin_port = htons(seller_port);

  buyer_sock_addr.sin_addr.s_addr = htonl(SERVER_HOST);
  seller_sock_addr.sin_addr.s_addr = htonl(SERVER_HOST);

  bind(socket_buyer_fd, (struct sockaddr *)&buyer_sock_addr, sizeof(buyer_sock_addr));
  bind(socket_seller_fd, (struct sockaddr *)&seller_sock_addr, sizeof(seller_sock_addr));

  listen(socket_buyer_fd, MAX_BUYERS);
  listen(socket_seller_fd, MAX_SELLERS);

  while(1) {
    int seller_connection = 0;
    if(seller_connection == 0) {
      seller_connection = accept(socket_seller_fd, NULL, NULL);
      if(seller_connection != -1) {
        pthread_create(&(*thread_list), NULL, client_server_thread, &seller_connection); 
        thread_list++;
      }
    }

    int connect_fd = accept(socket_buyer_fd, NULL, NULL);
    if(connect_fd != -1) {
      pthread_create(&(*thread_list), NULL, client_server_thread, &connect_fd); 
      thread_list++;
    }
  }
  return 0;
}

