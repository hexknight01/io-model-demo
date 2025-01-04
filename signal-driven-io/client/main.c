#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int client_socket;
     int message_count = 100;  // Number of messages to send
    int SERVER_PORT =  8081;
    char SERVER_IP[] = "127.0.0.1";
    int BUFFER_SIZE = 1024;
    int PORT_RANGE = 5;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
   

    // Create a UDP socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Send multiple messages to the server
    for (int i = 0; i < message_count; i++) {
        int random_port = SERVER_PORT + (rand() % PORT_RANGE); // Random port between 8081 and 8085
        server_addr.sin_port = htons(random_port);
        snprintf(buffer, BUFFER_SIZE, "Message %d from client", i + 1);

        ssize_t sent_bytes = sendto(client_socket, buffer, strlen(buffer), 0,
                                    (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (sent_bytes < 0) {
            perror("sendto failed");
        } else {
            printf("Sent: %s\n", buffer);
        }
    }

    printf("All messages sent.\n");

    // Close the socket
    close(client_socket);

    return 0;
}
