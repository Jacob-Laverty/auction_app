#ifndef AUCTION_SERVER_H
#define AUCTION_SERVER_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_BUYERS 10
#define MAX_SELLERS 1
#define DEFAULT_BUYER_PORT 8888
#define DEFAULT_SELLER_PORT 7777
#define SERVER_FAMILY AF_INET 
#define SERVER_HOST INADDR_ANY

//Command list
#define LOGIN "login"
#define LIST "list"
#define BID "bid"
#define ADD "add"
#define SELL "sell"
//End command list

typedef struct BuyerNodeStruct {
  char buyer_name[32];
  int bid;
  struct BuyerNodeStruct *next;
} BuyerNode;

typedef struct ItemNodeStruct {
  char item_name[32];
  int item_uid;
  struct ItemNodeStruct *next;
  BuyerNode *buyers_head;
} ItemNode;

int seller_port = DEFAULT_SELLER_PORT;
int buyer_port = DEFAULT_BUYER_PORT;
pthread_t* thread_list;

void read_opts(int argc, char** argv);
void init_thread_list();
void create_sockets();
void *client_server_thread(int*);

struct sockaddr_in buyer_sock_addr;
struct sockaddr_in seller_sock_addr;

//Head of the list of currently selling items
ItemNode *head;

int socket_buyer_fd;
int socket_seller_fd;
#endif
