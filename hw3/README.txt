README - CS176A HW3
Bharat Kathi

## Overview
This assignment implements a UDP-based ping client in C that communicates with a Python-based UDP ping server.
The client sends 10 ping messages to the server, waits for responses, and calculates round-trip times (RTTs).
The program also computes packet loss statistics, including minimum, average, and maximum RTTs.

## Files Included
- `PingClient.c` : UDP Ping client implementation.
- `Makefile` : Makefile to compile the client program.
- `README.txt` : This documentation file.

## Compilation Instructions
To compile all programs, run:
```
make
```

## Running the Programs

1. Start the ping server:
```
python3 UDPPingerServer.py
```

2. Start the ping client:
```
./PingClient <server_ip> <port>
```
Example:
```
./PingClient 127.0.0.1 12000
```