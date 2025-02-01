#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 129 // 128 chars + null terminator

void process_string(int server_sock) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int num_bytes;
    
    while ((num_bytes = recvfrom(server_sock, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr *)&client_addr, &client_len)) > 0) {
        buffer[num_bytes] = '\0';
        
        // Calculate initial sum and validate input
        int sum = 0;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] < '0' || buffer[i] > '9') {
                sendto(server_sock, "Sorry, cannot compute!\n", 24, 0,
                       (struct sockaddr *)&client_addr, client_len);
                return;
            }
            sum += buffer[i] - '0';
        }
        
        // Keep computing digit sums until we get a single digit
        char response[12];
        while (sum >= 10) {
            snprintf(response, sizeof(response), "%d\n", sum);
            sendto(server_sock, response, strlen(response), 0,
                   (struct sockaddr *)&client_addr, client_len);
            int new_sum = 0;
            for (int i = 0; response[i] != '\n'; i++) {
                new_sum += response[i] - '0';
            }
            sum = new_sum;
        }
        snprintf(response, sizeof(response), "%d\n", sum);
        sendto(server_sock, response, strlen(response), 0,
               (struct sockaddr *)&client_addr, client_len);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    int server_sock;
    struct sockaddr_in server_addr;
    int port = atoi(argv[1]);

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    
    process_string(server_sock);
    
    close(server_sock);
    return 0;
}
