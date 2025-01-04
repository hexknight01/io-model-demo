# How to run
## Complile to binary code and run
### Server
`pwd` at `go-io-model-demo/multiplexing-io/server`
```c
gcc -o program main.c eventdrivenio.c & ./program
```

### Linux manual
https://man7.org/linux/man-pages/man2/poll.2.html

### Client

```c
gcc -o program main.c & ./program
```