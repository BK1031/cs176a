#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define BUFFER_SIZE 256

void error_exit(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <port>\n", argv[0]);
        exit(1);
    }

    // Extract server IP and port
    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) error_exit("Socket creation failed");

    // Set up server address struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
        error_exit("Invalid address");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error_exit("Connection failed");

    // Ask user if they are ready
    char response;
    printf(">>> Ready to start game? (y/n): ");
    scanf(" %c", &response);
    if (tolower(response) != 'y') {
        printf("Goodbye!\n");
        close(sock);
        return 0;
    }

    // Send empty message to start game
    char start_msg[2] = {0};
    send(sock, start_msg, 1, 0);

    char buffer[BUFFER_SIZE];
    while (1) {
        // Receive game state update
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("Connection lost.\n");
            break;
        }
        buffer[bytes] = '\0';

        // Check if it's a message packet
        if (buffer[0] != '0') {
            printf(">>> %s\n", buffer + 2);  // Skip first byte (message flag)
            if (strncmp(buffer + 2, "Game Over!", 10) == 0) break;
            continue;
        }

        // Parse the game control packet
        int word_length, num_incorrect;
        char word[BUFFER_SIZE], incorrect_guesses[BUFFER_SIZE];

        sscanf(buffer, "0 %d %d %s %s", &word_length, &num_incorrect, word, incorrect_guesses);

        // Print game state
        printf(">>> %s\n", word);
        printf(">>> Incorrect Guesses: %s\n\n", num_incorrect > 0 ? incorrect_guesses : "");

        // Get user input
        char guess;
        while (1) {
            printf(">>> Letter to guess: ");
            scanf(" %c", &guess);

            if (!isalpha(guess)) {
                printf(">>> Error! Please guess one letter.\n");
                continue;
            }

            guess = tolower(guess);
            break;
        }

        // Send the guess to server
        char guess_packet[2] = {1, guess}; // msg_length = 1, followed by guessed letter
        send(sock, guess_packet, 2, 0);
    }

    close(sock);
    return 0;
}
