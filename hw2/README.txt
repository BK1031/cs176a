README - CS176A HW2
Bharat Kathi

## Overview
This assignment implements a client-server architecture using both TCP and UDP protocols in C. The programs allow a client to send a string to the server, which processes it and returns either:
1. A computed sum of digits until a single-digit result is achieved.
2. An error message if the string contains non-numeric characters.

## Files Included
- `server_c_tcp.c` : TCP server implementation.
- `client_c_tcp.c` : TCP client implementation.
- `server_c_udp.c` : UDP server implementation.
- `client_c_udp.c` : UDP client implementation.
- `Makefile` : Makefile to compile all programs.
- `README.txt` : This documentation file.

## Compilation Instructions
To compile all programs, run:
```
make
```

## Running the Programs

### TCP Server and Client:
1. Start the TCP server on a given port:
```
./server_c_tcp <port>
```
Example:
```
./server_c_tcp 32000
```
2. Start the TCP client and connect to the server:
```
./client_c_tcp <server_ip> <port>
```
Example:
```
./client_c_tcp 127.0.0.1 32000
```
3. Enter a numeric or non-numeric string when prompted.

### UDP Server and Client:
1. Start the UDP server on a given port:
```
./server_c_udp <port>
```
Example:
```
./server_c_udp 32000
```
2. Start the UDP client and connect to the server:
```
./client_c_udp <server_ip> <port>
```
Example:
```
./client_c_udp 127.0.0.1 32000
```
3. Enter a numeric or non-numeric string when prompted.
