#define GNU_SOURCE
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <stdio.h>

#define BUFFER_LEN 200
#define BUFFER_SHARE_LEN 2000
#define TAB_MAX_LENGHT 20000
struct arg_struct {
    char* arg1;
};

struct Buff_vir
{
    char type;
    size_t size;
	char ip[15];
    int port;
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



bool isInitialise = false;

original_write_t original_write = (original_write_t)dlsym(RTLD_NEXT, "write");
original_read_t original_read = (original_read_t)dlsym(RTLD_NEXT, "read");
original_connect_t original_connect = (original_connect_t)dlsym(RTLD_NEXT, "connect");
original_accept_t original_accept = (original_accept_t)dlsym(RTLD_NEXT, "accept");



int right_tab[TAB_MAX_LENGHT];
int left_tab[TAB_MAX_LENGHT];
int indice_right;
int indice_left;



void all_initialisation(){
    if (!isInitialise){ 
        bzero(right_tab, TAB_MAX_LENGHT*sizeof(int));
        bzero(left_tab, TAB_MAX_LENGHT*sizeof(int));
        indice_right = 0;
        indice_left = 0;
        isInitialise = true;
    }
}

bool exist_tab(int tab[], int fdsocket){
    int i;
    for(i =0; i<TAB_MAX_LENGHT; i++){
        if (tab[i]==fdsocket){
            return true;
        }
    }
    return false;
}

int accept(int sockfd, const struct sockadd *adresse, socklen_t* longueur){
    all_initialisation();
    int result;
    if (result = original_accept(sockfd, adresse, longueur) != -1){
        left_tab[indice_left] = sockfd;
        indice_left++;
    }
    return result;
}

int connect(int sockfd, const struct sockadd *serv_addr, socklen_t addrlen){
    all_initialisation();
    int result;
    if (result = original_connect(sockfd, serv_addr, addrlen) >= 0){
        right_tab[indice_right] = sockfd;
        indice_right++;
    }
    return result;
}   





ssize_t read(int fd, void *buf, size_t count){

    all_initialisation();    
    ssize_t result;

    if(exist_tab(right_tab, fd)){
        char etat;
        original_read(fd, etat,sizeof(char));
        printf("état : %c", etat);

        if(etat == 'R'){
            struct Buff_reel* buffer = malloc(sizeof(struct Buff_reel));
            result= original_read(fd, buffer,count);
            bzero(buf, sizeof(*buf));
            strcpy(buf,buffer->preload);
            return result;
        }
        else if(etat = 'V'){

            struct  Buff_vir* buffer = malloc(sizeof(struct  Buff_vir));
            //result= original_read(fd, &buffer_share[buf->identifiant],count);
            result= original_read(fd, buffer,sizeof(struct Buff_vir));

            struct sockaddr_in adresse;
            initAdresse(&adresse, buffer->ip);
            int serverSocket = initSocket_client(&adresse, buffer->ip);

            if ((status = original_connect(serverSocket, (struct sockaddr*)&adresse, sizeof(adresse)))< 0) {
                printf("\nConnection Failed \n");
            }

            
            struct request* resquest = malloc(sizeof(struct request));
            bzero(request, sizeof(struct request));
            request->id = buffer->id;
            request->lentgh = buffer->lentgh;
            inet_ntop(AF_INET, &(adresse.sin_addr), buffer->ip, INET_ADDRSTRLEN);

            //Envoie du buffer virtuel directement au backend
            original_write(serverSocket, request, sizeof(struct request), 0);

            char* buffer_final = malloc(BUFFER_LEN);
            //Retour du backend avec la page demandée
            valread = original_read(serverSocket, buffer_final, request->lentgh);
            printf("Le buffer vaut: %s\n", buf);

            bzero(buf, sizeof(*buf));
            strcpy(buf, buffer_final);
            close(serverSocket);
            return result;
        }
    }

    else {      
        return original_read(fd, buf, count);
    }

}

