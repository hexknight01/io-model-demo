# How to run
## Complile to binary code
```c
gcc -o program main.c blockingio.c -pthread & ./program
```
## Run the binary
```c
./program
```
The output should be like this
```bash
Read 38 bytes: hello from the other side of the world
Other thread is working...
Other thread is working...
Other thread is working...
Other thread is working...
Other thread is working...
```