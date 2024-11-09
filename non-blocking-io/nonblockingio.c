#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

int sockfd = -1; // Global socket file descriptor

// Set non-blocking mode for the socket
int set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl(F_SETFL)");
        return -1;
    }

    return 0;
}

// Signal handler for SIGTERM
void handle_sigterm(int signum) {
    printf("\nReceived SIGTERM, closing socket and exiting...\n");
    if (sockfd != -1) {
        close(sockfd);
    }
    _exit(0);  
}

int readNonBlockingIO() {
    char applicationBuffer[1024];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Set up the SIGTERM handler
    signal(SIGTERM, handle_sigterm);

    // Create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // Set the socket to non-blocking mode
    if (set_non_blocking(sockfd) == -1) {
        close(sockfd);
        return 1;
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8081);

    // Bind the socket to the specified port and IP address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(sockfd);
        return 1;
    }

    printf("Non-blocking UDP server is running on port %d...\n", 8081);

    // Event loop to repeatedly receive data from the socket
    while (1) {
        int received_byte = recvfrom(sockfd, applicationBuffer, sizeof(applicationBuffer), 0, (struct sockaddr *)&client_addr, &client_len);
        if (received_byte > 0) {
            applicationBuffer[received_byte] = '\0';  // Null-terminate the string
            printf("Received message: %s\n", applicationBuffer);
    
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            printf("Message from client: %s:%d\n", client_ip, ntohs(client_addr.sin_port));
            printf("Reading message done: %s\n", client_ip);
            break;
        } else if (received_byte == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available yet. Come back later...
                usleep(500);  // Sleep for 500ms and retry
            } else {
                perror("recvfrom failed");
                break;
            }
        }
    }
    printf("This block will not be hit until the message become avaiable to read from and the read process has done");
    close(sockfd);  // Close the socket after breaking out of the loop
    return 0;
}
