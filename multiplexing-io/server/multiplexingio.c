#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <string.h>



int read_multiplexing_IO() {
    int MAX_EVENTS = 10;
    int BUFFER_SIZE = 1024;
    int MAX_SOCKETS = 5;
    int sockets[MAX_SOCKETS];
    char applicationBuffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Initialize multiple sockets and bind them to the same port using SO_REUSEPORT
    for (int i = 0; i < MAX_SOCKETS; i++) {
        sockets[i] = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockets[i] == -1) {
            perror("socket");
            return 1;
        }

        // Set the SO_REUSEPORT option to allow multiple sockets to bind to the same port
        int optval = 1;
        if (setsockopt(sockets[i], SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
            perror("setsockopt(SO_REUSEPORT) failed");
            return 1;
        }

        memset(&server_addr, 0, sizeof(server_addr)); // Zero out the struct
        server_addr.sin_family = AF_INET;              // IPv4 family
        server_addr.sin_addr.s_addr = INADDR_ANY;      // Bind to all available interfaces
        server_addr.sin_port = htons(8081);            // Bind all sockets to port 8081

        if (bind(sockets[i], (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("bind");
            return 1;
        }

        printf("Socket %d is running on port %d...\n", i, 8081);
    }

    // Set up the poll structure
    /*
        struct pollfd {
            int   fd;          file descriptor 
            short events;      requested events 
            short revents;     returned events 
        };
    */
   // https://man7.org/linux/man-pages/man2/poll.2.html
    struct pollfd fds[MAX_SOCKETS];
    for (int i = 0; i < MAX_SOCKETS; i++) {
        fds[i].fd = sockets[i];
        fds[i].events = POLLIN; // POLLIN signal rerpresents data is avaialble to read.
    }
    while (1) {
        printf("Waiting for client");
        int poll_count = poll(fds, MAX_SOCKETS, -1); // Wait indefinitely for events
        if (poll_count == -1) {
            perror("poll failed");
            break;
        }

        for (int i = 0; i < MAX_SOCKETS; i++) {
            if (fds[i].revents & POLLIN) {
                // Data is ready to be read on socket i
                int received_byte = recvfrom(sockets[i], applicationBuffer, sizeof(applicationBuffer) - 1, 0,
                                             (struct sockaddr *)&client_addr, &client_len);
                if (received_byte > 0) {
                    applicationBuffer[received_byte] = '\0';  // Null-terminate the string
                    printf("Received message on socket %d: %s\n", i, applicationBuffer);

                    char client_ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
                    printf("Message from client on socket %d: %s:%d\n", i, client_ip, ntohs(client_addr.sin_port));
                } else if (received_byte == -1) {
                    perror("recvfrom failed");
                }
            }
        }
    }
    return 0;
}