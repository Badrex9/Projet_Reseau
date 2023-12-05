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
    request,
    none
};

struct Buff_vir
{
    char type;
    size_t size;
	char ip[15];
    int port
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

struct request{
	long id; 
	int lentgh;
};

enm Etat socket_ETat =  none;
char buffer_share[BUFFER_SHARE_LEN]; //mmap
bool isInitialise = False;
original_write_t original_write = (original_write_t)dlsym(RTLD_NEXT, "write");
original_read_t original_read = (original_read_t)dlsym(RTLD_NEXT, "read");


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


    if (!isInitialise){ 
        bzero(buffer_share, BUFFER_SHARE_LEN*sizeof(char));
        isInitialise = True;
    };

    if(type_preload(buf)=='R'){
        char* buffer = malloc(sizeof(struct Buff_reel));
        buffer->type ='R';
        buffer->size = count ;
        buffer->preload= *buff;
        return original_write(fd,buffer, sizeof(struct Buff_reel));
    }
    else{
        //on écrit la valeur dans buffer share
        strcpy(buffer_share + identifiant,buf);


        char* buffer = malloc(sizeof(struct  Buff_vir));
        buffer->type ='v'
        buffer->size = count ;

        struct sockaddr_in clientAdresse;
        unsigned int addrLen = sizeof(clientAdresse);

        if (getpeername(fd, (struct sockaddr *) &clientAdresse, &addrLen) == -1) {
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
        identifiant =+ buffer->lentgh ;
        return original_write(fd,buffer,sizeof(struct Buff_vir));
    }
	
 }   
 ssize_t read(int fd, void *buf, size_t count){

    if (!isInitialise){ 
        bzero(buffer_share, BUFFER_SHARE_LEN*sizeof(char));
        isInitialise = True;
    };


    ssize_t result;
    if(socket_ETat==reel){
        char* buffer = malloc(sizeof(struct Buff_reel));
        result= original_read(fd, buffer,count);
        bzero(buf, sizeof(*buf));
        strcpy(buf,buffer->preload);
    }
    else if(socket_ETat==virtuelle){

        char* buffer = malloc(sizeof(struct  Buff_vir));
        //result= original_read(fd, &buffer_share[buf->identifiant],count);
        result= original_read(fd, buffer,sizeof(struct Buff_vir));
        
        struct sockaddr_in adresse;
        initAdresse(&adresse, buffer->ip);
        int serverSocket = initSocket_client(&adresse, buffer->ip);

        if ((status
                = original_connect(serverSocket, (struct sockaddr*)&adresse,
                        sizeof(adresse)))< 0) {
                printf("\nConnection Failed \n");
        }

        socket_ETat = request;
        struct request* resquest = malloc(sizeof(struct request));
        bzero(request, sizeof(struct request));
        request->id = buffer->id;
        request->lentgh = buffer->lentgh;
        inet_ntop(AF_INET, &(adresse2.sin_addr), buffer->ip, INET_ADDRSTRLEN);
        original_write(serverSocket, request, sizeof(struct request), 0);

        char* buffer_final = malloc(BUFFER_LEN);
        valread = original_read(serverSocket, buffer_final, request->lentgh);
        printf("Le buffer vaut: %s\n", buf);

        bzero(buf, sizeof(*buf));
        strcpy(buf, buffer_final);
        close(serverSocket);

    }
    else if(socket_ETat==request){
        // A ecire
        return;
    }
    socket_ETat=none ;
    return result ;
 }
