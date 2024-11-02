#include "blockingio.h"
#include <unistd.h>  
#include <fcntl.h>   
#include <stdio.h>   
#include <stdlib.h>  
#include <pthread.h>

// Function that performs non-blocking work
void* thread_callback(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("Other thread is working...\n");
        sleep(1);  // Simulate work with a 1-second delay
    }
    return NULL;
}

int readBlockingIO() {
    int fd;
    char buffer[100];
    pthread_t other_thread;
    ssize_t bytesRead;

    // Open a file in read-only mode
    fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    

    // This read() function blocks the current thread until it is done
    bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Failed to read file");
        close(fd);
        exit(1);
    }

    // Create a new thread after read() operation is completed
    if (pthread_create(&other_thread, NULL, thread_callback, NULL) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    // Null-terminate the buffer to treat it as a string
    buffer[bytesRead] = '\0';

    // Print the contents of the buffer
    printf("Read %ld bytes: %s\n", bytesRead, buffer);

    // Close the file descriptor
    close(fd);

    // Wait for the other thread to finish
    if (pthread_join(other_thread, NULL) != 0) {
        perror("Failed to join thread");
        return 1;
    }

    return 0;
}