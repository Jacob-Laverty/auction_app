#ifndef AUCTION_CLIENT_H
#define AUCTION_CLIENT_H
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define DEFAULT_PORT 7227
#define DEFAULT_HOST '127.0.0.1'
#define SERVER_FAMILY AF_INET
#define SERVER_TYPE SOCK_STREAM
#define BUFFER_SIZE 1024

enum client_type {SELLER, BUYER} cli_type;

int port;
int socket_fd;
char *host;
char input_buffer[BUFFER_SIZE];
pthread_t server_listener;
struct sockaddr_in sockAddr;

void read_opts(int argc, char** argv);
void create_socket();
void *server_listen(int*);

int main();

#endif
