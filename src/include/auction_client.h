#ifndef AUCTION_CLIENT_H
#define AUCTION_CLIENT_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define DEFAULT_PORT 7227
#define DEFAULT_HOST '127.0.0.1'
#define SERVER_FAMILY AF_INET
#define SERVER_TYPE SOCK_STREAM


int port;
char *host;
int socket_fd;
struct sockaddr_in sockAddr;

void read_opts(int argc, char** argv);
void create_socket();

int main();

#endif
