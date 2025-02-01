# Makefile for TCP and UDP Client-Server Programs

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Executable names
TCP_SERVER = server_c_tcp
TCP_CLIENT = client_c_tcp
UDP_SERVER = server_c_udp
UDP_CLIENT = client_c_udp

# Source files
TCP_SERVER_SRC = server_c_tcp.c
TCP_CLIENT_SRC = client_c_tcp.c
UDP_SERVER_SRC = server_c_udp.c
UDP_CLIENT_SRC = client_c_udp.c

all: $(TCP_SERVER) $(TCP_CLIENT) $(UDP_SERVER) $(UDP_CLIENT)

$(TCP_SERVER): $(TCP_SERVER_SRC)
	$(CC) $(CFLAGS) -o $(TCP_SERVER) $(TCP_SERVER_SRC)

$(TCP_CLIENT): $(TCP_CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(TCP_CLIENT) $(TCP_CLIENT_SRC)

$(UDP_SERVER): $(UDP_SERVER_SRC)
	$(CC) $(CFLAGS) -o $(UDP_SERVER) $(UDP_SERVER_SRC)

$(UDP_CLIENT): $(UDP_CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(UDP_CLIENT) $(UDP_CLIENT_SRC)

clean:
	rm -f $(TCP_SERVER) $(TCP_CLIENT) $(UDP_SERVER) $(UDP_CLIENT)
