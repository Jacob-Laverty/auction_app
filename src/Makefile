CC = gcc
CFLAGS = -Wall -g

IDIR = ./include
INCLUDES = -I$(IDIR)

LANGUAGE = $(IDIR)/lang.l

SERVER_SRCS = auction_server.c 
CLIENT_SRCS = auction_client.c

OBJDIR=./obj
SERVER_OBJECTS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJECTS = $(CLIENT_SRCS:.c=.o)

MAIN_SERVER = server
MAIN_CLIENT = client

server: $(MAIN_SERVER)
	@mv *.o $(OBJDIR)
	@echo Auction application compiled

client: $(MAIN_CLIENT)
	@mv *.o $(OBJDIR)
	@echo Auction application compiled

$(MAIN_SERVER): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN_SERVER) $(SERVER_OBJECTS)

$(MAIN_CLIENT): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN_CLIENT) $(CLIENT_OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) ./*.o $(OBJDIR)/*.o *~ $(MAIN_SERVER) $(MAIN_CLIENT)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
