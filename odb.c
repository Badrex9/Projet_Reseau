#define GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>


ssize_t send(int socket, const void *buffer, size_t size, int flags){
    printf("On a fait un appel à send qu'on va faire échouer\n");
    getchar();
    return 0;
}


