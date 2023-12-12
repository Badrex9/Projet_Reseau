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

struct arg_struct {
    char* arg1;
}

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


bool isInitialise = False;
original_write_t original_write = (original_write_t)dlsym(RTLD_NEXT, "write");
original_read_t original_read = (original_read_t)dlsym(RTLD_NEXT, "read");
original_connect_t original_connect = (original_connect_t)dlsym(RTLD_NEXT, "connect");
original_accecpt_t original_connect = (original_connect_t)dlsym(RTLD_NEXT, "connect");


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

ssize_t read(int fd, void *buf, size_t count){

    isInitialise = True;

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

        if ((status = original_connect(serverSocket, (struct sockaddr*)&adresse, sizeof(adresse)))< 0) {
            printf("\nConnection Failed \n");
        }

        socket_ETat = request;
        struct request* resquest = malloc(sizeof(struct request));
        bzero(request, sizeof(struct request));
        request->id = buffer->id;
        request->lentgh = buffer->lentgh;
        inet_ntop(AF_INET, &(adresse2.sin_addr), buffer->ip, INET_ADDRSTRLEN);

        //Envoie du buffer virtuel directement au backend
        original_write(serverSocket, request, sizeof(struct request), 0);

        char* buffer_final = malloc(BUFFER_LEN);
        //Retour du backend avec la page demandée
        valread = original_read(serverSocket, buffer_final, request->lentgh);
        printf("Le buffer vaut: %s\n", buf);

        bzero(buf, sizeof(*buf));
        strcpy(buf, buffer_final);
        close(serverSocket);

    }
    socket_ETat=none ;
    return result ;
}

