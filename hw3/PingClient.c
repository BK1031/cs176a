#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#define PING_COUNT 10
#define TIMEOUT_SEC 1
#define BUFFER_SIZE 1024

// Function to calculate time difference in milliseconds
double time_diff(struct timeval start, struct timeval end) {
    return ((end.tv_sec - start.tv_sec) * 1000.0) + ((end.tv_usec - start.tv_usec) / 1000.0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in server_addr;
    struct timeval timeout = {TIMEOUT_SEC, 0};
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set socket timeout
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Set timeout failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    int transmitted = 0, received = 0;
    double min_rtt = 1e6, max_rtt = 0, total_rtt = 0;

    for (int seq = 1; seq <= PING_COUNT; seq++) {
        struct timeval start, end;
        char send_buffer[BUFFER_SIZE];
        char recv_buffer[BUFFER_SIZE];

        // Get current time
        gettimeofday(&start, NULL);
        snprintf(send_buffer, sizeof(send_buffer), "PING %d %ld.%06ld", seq, start.tv_sec, start.tv_usec);

        // Send packet
        if (sendto(sockfd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("sendto failed");
            continue;
        }
        transmitted++;

        // Receive response
        socklen_t addr_len = sizeof(server_addr);
        ssize_t recv_len = recvfrom(sockfd, recv_buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);

        if (recv_len < 0) {
            printf("Request timeout for seq#=%d\n", seq);
        } else {
            gettimeofday(&end, NULL);
            double rtt = time_diff(start, end);

            printf("PING received from %s: seq#=%d time=%.2f ms\n", server_ip, seq, rtt);
            received++;

            // Update RTT stats
            if (rtt < min_rtt) min_rtt = rtt;
            if (rtt > max_rtt) max_rtt = rtt;
            total_rtt += rtt;
        }

        sleep(1);
    }

    double loss_percent = ((transmitted - received) / (double)transmitted) * 100.0;
    double avg_rtt = received ? (total_rtt / received) : 0.0;

    // Print final statistics
    printf("--- %s ping statistics ---\n", server_ip);
    printf("%d packets transmitted, %d received, %.0f%% packet loss", transmitted, received, loss_percent);
    if (received > 0) {
        // Round RTT values to 3 decimal places
        min_rtt = round(min_rtt * 1000) / 1000.0;
        avg_rtt = round(avg_rtt * 1000) / 1000.0;
        max_rtt = round(max_rtt * 1000) / 1000.0;
        printf(" rtt min/avg/max = %.3f %.3f %.3f ms", min_rtt, avg_rtt, max_rtt);
    }
    printf("\n");

    close(sockfd);
    return 0;
}
