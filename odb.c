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
enum Etat {
    reel,
    virtuelle,
    none
};

struct request{
	long id; 
	int lentgh;
};
struct Buff_vir
{
	char ip[15];
	long id;
	int offset;
	int lentgh;
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
     char* buffer = malloc(BUFFER_LEN*sizeof(char));
    if(type_preload(buf)=='R'){
        *buffer ='R';
        *(buffer+1) = count ;
        *(buffer+sizeof(size_t)/sizeof(char)) = buff;
        enm Etat socket_ETat = reel ;
        return original_write(fd,buffer, count+1);
    }
    else{
        *buffer ='V';
        *(buffer+1) = count ;
        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);

        if (getpeername(sockfd, (struct sockaddr*)&addr, &addrLen) == -1) {
            perror("Erreur lors de l'extraction de l'adresse et du port");
            exit(1);
        }
        char ip[15];
        inet_ntop(AF_INET, &(addr.sin_addr), ip, 15);
        int port = ntohs(addr.sin_port);
        struct Buff_vir* buffer_virtuel = malloc(sizeof(struct Buff_vir));
	    bzero(buffer_virtuel, sizeof(struct Buff_vir));
	    //Initialisation de l'offset du buffer_share
	    buffer_virtuel->offset = 0; 
	    buffer_virtuel->lentgh = strlen(buf);
        buffer_virtuel->ip =ip ;
        buffer_virtuel->port = port ;
        buffer_virtuel->id = 0 ;   
        identifiant =+ buffer_virtuel->lentgh ;
        *(buffer+sizeof(size_t)/sizeof(char))=*(buffer_virtuel)
        strcpy(buffer_share,buff);
        enm Etat socket_ETat = virtuelle ;
        return original_write(fd,buffer,sizeof(struct Buff_vir)+sizeof(char)+sizeof(ssize_t));
    }
   
    
	
 }   
 ssize_t read(int fd, void *buf, size_t count){
    original_read_t original_read = (original_read_t)dlsym(RTLD_NEXT, "read");
    if(socket_ETat=reel){
      return original_read(fd, buff,count);
    }
    else{
      buffer_share[identifiant];
      original_read(fd, buff,count);
      

    }
     
 }
