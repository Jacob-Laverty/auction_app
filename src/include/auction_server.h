#ifndef AUCTION_SERVER_H
#define AUCTION_SERVER_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define DEFAULT_PORT 7227
#define SERVER_FAMILY AF_INET 
#define SERVER_HOST INADDR_ANY

int port;
pthread_t* thread_list;

void read_opts(int argc, char** argv);
void init_thread_list();
void create_socket();
void *client_server_thread(int*);

struct sockaddr_in sockAddr;
int socket_fd;
#endif
