package main

/*
#include <stdio.h>
#include <errno.h>

// Define the hello function in C
void hello() {
    printf("Hello from C!\n");
}
*/
import "C"

func main() {
	// Call the hello function defined above
	C.hello()
}
