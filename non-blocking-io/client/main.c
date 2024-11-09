#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_PORT 8081
#define SERVER_IP "127.0.0.1" 
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char *message = "Hello, server!";
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }

    // Define the server address to send data to
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // Convert IP address to binary form and assign it
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sockfd);
        return 1;
    }

    // Send a message to the server
    int bytes_sent = sendto(sockfd, message, strlen(message), 0,
                            (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_sent == -1) {
        perror("sendto failed");
        close(sockfd);
        return 1;
    }
    printf("Sent message to server: %s\n", message);

    close(sockfd);
    return 0;
}
