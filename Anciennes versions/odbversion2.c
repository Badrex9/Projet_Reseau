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
#define BUFFER_LEN 200
#define BUFFER_SHARE_LEN 2000
enum Etat {
    reel,
    virtuelle,
    none
};

struct Buff_vir
{
    char type;
    size_t size;
	char ip[15];
	long id;
	int offset;
	int lentgh;
};
struct Buff_reel
{
    char type;
    size_t size;
    char preload[BUFFER_LEN];
};
enm Etat socket_ETat =  none;
char buffer_share[BUFFER_SHARE_LEN];
bzero(buffer_share, BUFFER_SHARE_LEN*sizeof(char));
long identifiant = 0;

char type_preload(const void *buffer){
    size_t taille =4096;
    if(sizeof(*buffer)< taille){
        socket_ETat = reel ;
       return 'R';
    }
    else {
        socket_ETat=virtuelle;
        return 'V';
    }
}

 ssize_t write(int fd, const void *buf, size_t count){
    original_write_t original_write = (original_write_t)dlsym(RTLD_NEXT, "write");
    if(type_preload(buf)=='R'){
        char* buffer = malloc(sizeof(struct Buff_reel));
        buffer->type ='R';
        buffer->size = count ;
        buffer->preload= *buff;
        enm Etat socket_ETat = reel ;
        return original_write(fd,buffer, count+1);
    }
    else{
        char* buffer = malloc(sizeof(struct  Buff_vir));
        buffer->type ='v'
        *(buffer+1) = count ;
        buffer->size = count ;
        if (getpeername(sockfd, (struct sockaddr*)&addr, &addrLen) == -1) {
            perror("Erreur lors de l'extraction de l'adresse et du port");
            exit(1);
        }
        char ip[15];
        inet_ntop(AF_INET, &(addr.sin_addr), ip, 15);
        int port = ntohs(addr.sin_port);;
	    //Initialisation de l'offset du buffer_share
	    buffer->offset = 0; 
        buffer->ip =ip ;
        buffer->port = port ;
        buffer->id = identifiant ;   
        identifiant =+ buffer_virtuel->lentgh ;
        strcpy(buffer_share,buff);
        enm Etat socket_ETat = virtuelle ;
        return original_write(fd,buffer,sizeof(struct Buff_vir));
    }
   
    
	
 }   
 ssize_t read(int fd, void *buf, size_t count){
    original_read_t original_read = (original_read_t)dlsym(RTLD_NEXT, "read");
    ssize_t result;
    if(socket_ETat==reel){
      result= original_read(fd, buf->preload,count);
    }
    else if(socket_ETat==virtuelle){
      result= original_read(fd, &buffer_share[buf->identifiant],count);
    }
    else {
        return;
    }
    socket_ETat=none ;
    return result ;
 }
