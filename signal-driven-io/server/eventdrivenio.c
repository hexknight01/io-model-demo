#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

#define MAX_SOCKETS 5
#define BUFFER_SIZE 1024

int sockets[MAX_SOCKETS];
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);

// Signal handler for SIGIO
void handle_sigio(int signo) {
    char applicationBuffer[BUFFER_SIZE];
    for (int i = 0; i < MAX_SOCKETS; i++) {
        int received_byte = recvfrom(sockets[i], applicationBuffer, sizeof(applicationBuffer) - 1, 0,
                                     (struct sockaddr *)&client_addr, &client_len);
        if (received_byte > 0) {
            applicationBuffer[received_byte] = '\0'; // Null-terminate the string
            printf("Received message on socket %d: %s\n", i, applicationBuffer);

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
            printf("Message from client on socket %d: %s:%d\n", i, client_ip, ntohs(client_addr.sin_port));
        } else if (received_byte == -1 && errno != EWOULDBLOCK) {
            perror("recvfrom failed");
        }
    }
}

int read_event_driven_IO() {
    struct sockaddr_in server_addr;

    // Initialize sockets
    for (int i = 0; i < MAX_SOCKETS; i++) {
        sockets[i] = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockets[i] == -1) {
            perror("socket");
            return 1;
        }

        // Define server address for each socket
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(8081 + i);

        // Bind the socket to the specified port and IP address
        if (bind(sockets[i], (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("bind failed");
            close(sockets[i]);
            return 1;
        }

        printf("Socket %d is running on port %d...\n", i, 8081 + i);

        // Set socket to non-blocking mode
        int flags = fcntl(sockets[i], F_GETFL, 0);
        fcntl(sockets[i], F_SETFL, flags | O_NONBLOCK);

        // Set the process as the owner of the socket for signals
        fcntl(sockets[i], F_SETOWN, getpid());

        // Enable asynchronous I/O
        fcntl(sockets[i], F_SETFL, flags | O_ASYNC);
    }

    // Install signal handler for SIGIO
    struct sigaction sa;
    sa.sa_handler = handle_sigio;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    while (1) { 
        printf("Doing something else while waiting for data in sockets\n");
        sleep(1);
    }
    return 0;
}
