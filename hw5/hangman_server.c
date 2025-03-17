#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>

#define MAX_CLIENTS 3
#define MAX_WORDS 15
#define MAX_WORD_LENGTH 8
#define PORT 9090

typedef struct {
    int socket;
    char word[MAX_WORD_LENGTH + 1];
    char guessed[MAX_WORD_LENGTH + 1];
    char incorrect[7];  // Max 6 incorrect guesses
    int incorrect_count;
} GameSession;

char word_list[MAX_WORDS][MAX_WORD_LENGTH + 1];
int total_words = 0;
int active_clients = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void load_words() {
    FILE *file = fopen("hangman_words.txt", "r");
    if (!file) {
        perror("Error opening word list");
        exit(1);
    }
    while (fscanf(file, "%s", word_list[total_words]) != EOF && total_words < MAX_WORDS) {
        total_words++;
    }
    fclose(file);
}

char* get_random_word() {
    return word_list[rand() % total_words];
}

void* handle_client(void* arg) {
    GameSession *session = (GameSession*) arg;
    int client_socket = session->socket;
    char buffer[256];

    // Initialize game session
    memset(session->guessed, '_', strlen(session->word));
    session->guessed[strlen(session->word)] = '\0';
    memset(session->incorrect, 0, 7);  // Initialize incorrect guesses array
    session->incorrect_count = 0;

    while (session->incorrect_count < 6) {
        // Send game state - Add space after incorrect guesses
        snprintf(buffer, sizeof(buffer), "0 %ld %d %s %s ",
                 strlen(session->word), session->incorrect_count,
                 session->guessed, session->incorrect);
        send(client_socket, buffer, strlen(buffer), 0);

        // Receive guess
        int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';

        char guess = tolower(buffer[0]);
        int correct = 0;

        // Check if guess is in word
        for (int i = 0; i < strlen(session->word); i++) {
            if (session->word[i] == guess) {
                session->guessed[i] = guess;
                correct = 1;
            }
        }

        if (!correct) {
            // Make sure to null terminate after adding each incorrect guess
            session->incorrect[session->incorrect_count] = guess;
            session->incorrect[++session->incorrect_count] = '\0';
        }

        // Check for win/loss
        if (strcmp(session->word, session->guessed) == 0) {
            send(client_socket, "8 You Win!", 10, 0);
            break;
        } else if (session->incorrect_count >= 6) {
            send(client_socket, "8 You Lose :(", 12, 0);
            break;
        }
    }

    close(client_socket);
    free(session);
    pthread_mutex_lock(&lock);
    active_clients--;
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    load_words();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server running on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);

        pthread_mutex_lock(&lock);
        if (active_clients >= MAX_CLIENTS) {
            send(client_socket, "8 Server Overloaded", 19, 0);
            close(client_socket);
        } else {
            GameSession *session = malloc(sizeof(GameSession));
            session->socket = client_socket;
            strcpy(session->word, get_random_word());
            session->incorrect_count = 0;
            active_clients++;
            pthread_t thread;
            pthread_create(&thread, NULL, handle_client, (void*) session);
            pthread_detach(thread);
        }
        pthread_mutex_unlock(&lock);
    }
}
