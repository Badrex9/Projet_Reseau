#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>



int main(int argc, char* argv[]) {
    #ifdef PORT
    printf("Le port vaut : %d", PORT); 
    #endif
}