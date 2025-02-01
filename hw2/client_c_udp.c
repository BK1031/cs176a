#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 129 // 128 chars + null terminator

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }
    
    int client_sock;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE];
    
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(1);
    }
    
    // Timeout so that the client doesn't wait indefinitely for a response since the server can't close the connection
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if (setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error setting timeout");
        exit(1);
    }
    
    printf("Enter string: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    
    sendto(client_sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
    
    // Keep receiving messages until timeout or error
    int num_bytes;
    while ((num_bytes = recvfrom(client_sock, buffer, BUFFER_SIZE - 1, 0, 
           (struct sockaddr *)&server_addr, &addr_len)) > 0) {
        buffer[num_bytes] = '\0';
        char *line = strtok(buffer, "\n");
        while (line != NULL) {
            printf("From server: %s\n", line);
            line = strtok(NULL, "\n");
        }
    }
    
    close(client_sock);
    return 0;
}
