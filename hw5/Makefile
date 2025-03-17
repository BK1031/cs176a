CC = gcc
CFLAGS = -Wall -Wextra -pthread
SERVER = hangman_server
CLIENT = hangman_client

SERVER_SRC = hangman_server.c
CLIENT_SRC = hangman_client.c

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

clean:
	rm -f $(SERVER) $(CLIENT)