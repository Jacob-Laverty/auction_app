#ifndef AUCTION_SERVER_H
#define AUCTION_SERVER_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_PORT 7227
#define SERVER_FAMILY AF_INET 
#define SERVER_HOST INADDR_ANY

int port;
void read_opts(int argc, char** argv);
void create_socket();

struct sockaddr_in sockAddr;
int socket_fd;
#endif
