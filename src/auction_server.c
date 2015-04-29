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

void update(WinningNode* winning_list_head, int* client_fd, char* name) {
  WinningNode* curr_winning = winning_list_head;
  ItemNode* curr_item;

  while(curr_winning->next != NULL && curr_winning != NULL) {
    curr_item = head;
    while(curr_item -> next != NULL && curr_item->item_uid != curr_winning->next->item) {
      curr_item = curr_item->next;
    }

    WinningNode* win = curr_winning->next;
    if((curr_item->item_uid) == win->item) {
      if((strcmp(curr_item -> highest_bidder, name)) != 0) {
        char server_resp[1024];
        snprintf(&server_resp, 1024, "You have been outbid on item %d", win->item);
        write(*client_fd, server_resp, strlen(server_resp));

        curr_winning->next = win->next;
        free(win);
      }
    } else {
      char server_resp[1024];
      snprintf(&server_resp, 1024, "You have won item %d", win->item);
      write(*client_fd, server_resp, strlen(server_resp));

      curr_winning->next = win->next;
      free(win);
    }
    curr_winning = curr_winning->next;
  }
}

void *seller_thread(int *client_fd) {
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
        pthread_mutex_lock(&operation_mutex);
        if(client_name[0] != 0) {
          strncpy(server_resp, "Client is already logged in!", 1024);
          write(*client_fd, server_resp, strlen(server_resp));
        } else {
          sscanf(buf, "%s %s", command, client_name);
          snprintf(&server_resp, 1024, "Client logged in as %s", client_name);
          write(*client_fd, server_resp, strlen(server_resp));
        }
        pthread_mutex_unlock(&operation_mutex);
      }
      //LIST
      else if (strcmp(LIST, command) == 0 && client_name[0] != 0) {
        pthread_mutex_lock(&operation_mutex);

        ItemNode *curr = head->next;
        while(curr != NULL) {
          snprintf(server_resp, 1024, "Item: %d, %s\t Highest Bidder: %s, %d", curr->item_uid, curr->item_name, curr->highest_bidder, curr->highest_bid);
          curr = curr -> next;
          write(*client_fd, server_resp, strlen(server_resp));
          memset(server_resp, 0 , 1024);
        }

        pthread_mutex_unlock(&operation_mutex);
      }
      //SELL
      else if (strcmp(SELL, command) == 0 && client_name[0] != 0) {
        pthread_mutex_lock(&operation_mutex);

        int sell_item_id;

        sscanf(buf, "%s %d", command, &sell_item_id);

        ItemNode* curr = head->next;
        if(curr -> item_uid == sell_item_id) {
          head -> next = curr -> next;
          snprintf(&server_resp, 1024, "Item %d sold for %d monies!", sell_item_id, curr -> highest_bid);
          write(*client_fd, server_resp, strlen(server_resp));
          free(curr);
        } else {
          while(curr -> next != NULL && curr -> next -> item_uid != sell_item_id) {
            curr = curr -> next;
          }

          if(curr->next->item_uid != sell_item_id) {
            snprintf(&server_resp, 1024, "Item %d does not exist to be sold. Please sell an existing item.", sell_item_id);
            write(*client_fd, server_resp, strlen(server_resp));
          } else {
            ItemNode* sellable = curr->next;
            curr -> next = sellable -> next;
            snprintf(&server_resp, 1024, "Item %d sold for %d monies!", sell_item_id, sellable->highest_bid);
            write(*client_fd, server_resp, strlen(server_resp));
            free(sellable);
          }
        }
        pthread_mutex_unlock(&operation_mutex);
      }
      //ADD
      else if (strcmp(ADD, command) == 0 && client_name[0] != 0) {
        pthread_mutex_lock (&operation_mutex);

        char new_item_name[32];
        int new_item_id;

        sscanf(buf, "%s %s %d", command, new_item_name, &new_item_id);

        printf("Server is adding new item %s with id %d", new_item_name, new_item_id);

        int invalid = 0;
        ItemNode *curr = head;
        while(curr->next != NULL) {
          if((strcmp(curr->item_name, new_item_name)) == 0 || new_item_id == curr->item_uid) {
            strncpy(server_resp, "An item matching this description is already being sold", 1024);
            write(*client_fd, server_resp, strlen(server_resp));
            invalid = 1;
          }
          curr = curr -> next; 
        }

        if(invalid != 1) {
          curr->next = (ItemNode*)malloc(sizeof(ItemNode*));
          curr = curr -> next;

          memset(curr->highest_bidder, 0, 32);
          memset(curr->item_name, 0, 32);

          strncpy(curr->item_name, new_item_name, 32);

          curr -> item_uid = new_item_id;
          curr -> highest_bid = 0;
          curr -> next = NULL;

          snprintf(server_resp, 1024, "Added item %s %d", new_item_name, new_item_id);

          write(*client_fd, server_resp, strlen(server_resp));
        }

        pthread_mutex_unlock(&operation_mutex);
      }
      //Else
      else {
        snprintf(&server_resp, 1024, "Command %s not recognized. Please enter a valid command", command);
        write(*client_fd, server_resp, strlen(server_resp));
      }
    }
  }
}

void *buyer_thread(int *client_fd) {
  char client_name[32];
  memset(client_name, 0, 32);
  char buf[1024];
  char server_resp[1024];
  
  WinningNode *winning_list_head = (WinningNode*)malloc(sizeof(WinningNode)); 
  winning_list_head -> next = NULL;
  while(1) {
    memset(buf, 0, 1024);
    memset(server_resp, 0 ,1024);

    //pthread_mutex_lock(&operation_mutex);
    //update(winning_list_head, client_fd, client_name);
    //pthread_mutex_unlock(&operation_mutex);
    read(*client_fd, buf, 1024);
    if(buf[0] != 0) {
      char command[32]; 
      sscanf(buf, "%s", command);
      printf("Server Received %s\n", command);

      //LOGIN
      if((strcmp(LOGIN, command)) == 0) {
        pthread_mutex_lock(&operation_mutex);
        if(client_name[0] != 0) {
          strncpy(server_resp, "Client is already logged in!", 1024);
          write(*client_fd, server_resp, strlen(server_resp));
        } else {
          sscanf(buf, "%s %s", command, client_name);
          snprintf(&server_resp, 1024, "Client logged in as %s", client_name);
          write(*client_fd, server_resp, strlen(server_resp));
        }
        pthread_mutex_unlock(&operation_mutex);
      }
      //LIST
      else if (strcmp(LIST, command) == 0 && client_name[0] != 0) {
        pthread_mutex_lock(&operation_mutex);

        ItemNode *curr = head->next;
        while(curr != NULL) {
          snprintf(server_resp, 1024, "Item: %d, %s\t Highest Bidder: %s, %d", curr->item_uid, curr->item_name, curr->highest_bidder, curr->highest_bid);
          curr = curr -> next;
          write(*client_fd, server_resp, strlen(server_resp));
          memset(server_resp, 0 , 1024);
        }

        pthread_mutex_unlock(&operation_mutex);
      }
      //BID
      else if (strcmp(BID, command) == 0 && client_name[0] != 0) {
        //Only keep the highest
        pthread_mutex_lock(&operation_mutex); 

        int bid_amount;
        int bid_item;
        sscanf(buf, "%s %d %d", command, &bid_item, &bid_amount);

        printf("Attempting bid on item %d for amount %d\n", bid_item, bid_amount);
        ItemNode  *curr = head->next;
        while(curr -> next != NULL && curr->item_uid != bid_item) {
          curr = curr->next;
        }

        if(curr->item_uid != bid_item) {
          snprintf(&server_resp, 1024, "Item %d does not exist, place a bid on an existing item.", bid_item);
          write(*client_fd, server_resp, strlen(server_resp));
        } else {
          if(curr->highest_bid < bid_amount) {
            strncpy(curr->highest_bidder, client_name, 32);
            curr -> highest_bid = bid_amount;

            WinningNode* win_curr = winning_list_head; 
            while(win_curr -> next != NULL) {
              win_curr = win_curr -> next; 
            }

            win_curr -> next = (WinningNode*)malloc(sizeof(WinningNode));
            win_curr = win_curr -> next;
            win_curr -> next = NULL;
            win_curr -> item = bid_item;

            snprintf(&server_resp, 1024, "You are now the highest bidder on item %d", bid_item);
            write(*client_fd, server_resp, strlen(server_resp));
          } else {
            snprintf(&server_resp, 1024, "Bid is not high enough. Highest bid for item %d is %d", bid_item, curr->highest_bid);
            write(*client_fd, server_resp, strlen(server_resp));
          }
        }
        pthread_mutex_unlock(&operation_mutex);
      } 
      //Else
      else {
        snprintf(&server_resp, 1024, "Command %s not recognized. Please enter a valid command", command);
        write(*client_fd, server_resp, strlen(server_resp));
      }
    }
  }
}

int main(int argc, char **argv) {
  init_thread_list();

  head = (ItemNode*) malloc(sizeof(ItemNode));
  head -> next = 0;

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

  int seller_connection = 0;
  while(1) {
    if(seller_connection == 0) {
      seller_connection = accept(socket_seller_fd, NULL, NULL);
      if(seller_connection != -1) {
        char msg[32] = "Connected as the seller";
        write(seller_connection, msg, strlen(msg));
        pthread_create(&(*thread_list), NULL, seller_thread, &seller_connection); 
        thread_list++;
      }
    }

    int connect_fd = accept(socket_buyer_fd, NULL, NULL);
    if(connect_fd != -1) {
      fcntl(connect_fd, F_SETFL, O_NONBLOCK);
      char msg[32] = "Connected as a buyer";
      write(connect_fd, msg, strlen(msg));
      pthread_create(&(*thread_list), NULL, buyer_thread, &connect_fd); 
      thread_list++;
    }
  }
  return 0;
}

