# How to run
## Complile to binary code and run
### Server
`pwd` at `go-io-model-demo/multiplexing-io/server`
```c
gcc -o program main.c multiplexingio.c & ./program
```

### Linux manual
https://man7.org/linux/man-pages/man2/poll.2.html

### Client

```c
gcc -o program main.c & ./program
```
The output should be like this
```bash
➜  non-blocking-io git:(master) ✗ gcc -o program main.c nonblockingio.c & ./program
[1] 30379
[1]  + 30379 done       gcc -o program main.c nonblockingio.c
Non-blocking UDP server is running on port 8081...
Received message: Hello, server!
Message from client: 127.0.0.1:50412
Reading message done: 127.0.0.1
This block will not be hit until the message become avaiable to read from and the read process has done%  
```

```bash
➜  client git:(master) ✗ gcc -o program main.c & ./program
[1] 30428
Sent message to server: Hello, server!
```