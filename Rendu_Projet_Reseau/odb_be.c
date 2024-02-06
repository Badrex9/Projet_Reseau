#define _GNU_SOURCE 
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>

#define BUFFER_LEN 4096
#define BUFFER_SHARE_LEN 300000000
#define PORT 5000
#define IP INADDR_ANY
#define BACKLOG 3
#define TAB_MAX_LENGHT 100

//gcc -shared -o odb_be.so -lpthread -pthread -fPIC odb_be.c -ldl -DPORT_BE=6000 (compilation bibliothèque)
//gcc Back_end.c -o BE
//LD_PRELOAD=./odb_be.so ./BE 7000 
struct arg_struct {
    int arg1;
};

struct Buff_vir
{
    size_t size;
	char ip[15];
    int port;
	long id;
	int offset;
	int lentgh;
};
struct Buff_reel
{
    size_t size;
    char preload[BUFFER_LEN];
};

struct request{
	long id; 
	int lentgh;
};

char buffer_share[BUFFER_SHARE_LEN]; //mmap
bool isInitialise = false;
typedef ssize_t (*original_read_t)(int , void *, size_t );
typedef ssize_t (*original_write_t)(int , void *, size_t );
typedef int (*original_accept_t)(int,struct sockaddr *, socklen_t* );
typedef int (*original_connect_t)(int , const struct sockaddr *, socklen_t );

pthread_t a;

long identifiant = 0;

int right_tab[TAB_MAX_LENGHT];
int left_tab[TAB_MAX_LENGHT];
int indice_right;
int indice_left;

// Initialisation de la structure sockaddr_in
void initAdresse(struct sockaddr_in * adresse, int port) {
	(*adresse).sin_family = AF_INET;
	(*adresse).sin_addr.s_addr = IP;
	(*adresse).sin_port = htons( port);
}

// Démarrage de la socket serveur
int initSocket(struct sockaddr_in * adresse){
	// Descripteur de socket
	int fdsocket;

	// Création de la socket en TCP
	if ((fdsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Echéc de la création: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Création de la socket\n");

	// Attachement de la socket sur le port et l'adresse IP
	if (bind(fdsocket, (struct sockaddr *) adresse, sizeof(*adresse)) != 0) {
		printf("Echéc d'attachement: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	//printf("Attachement de la socket sur le port %s\n", port);

	// Passage en écoute de la socket
	if (listen(fdsocket, BACKLOG) != 0) {
		printf("Echéc de la mise en écoute: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Fin de l'initialisation\n");

    return fdsocket;
}

char type_preload(const void *buffer){
    size_t taille =4096;
    if(sizeof(*buffer)< taille){
       return 'R';
    }
    else {
        return 'V';
    }
}

void *traitement(void *arguments){
    original_accept_t original_accept = (original_accept_t)dlsym(RTLD_NEXT, "accept");
    original_read_t original_read = (original_read_t)dlsym(RTLD_NEXT, "read");
    original_write_t original_write = (original_write_t)dlsym(RTLD_NEXT, "write");
    
	//struct arg_struct *args = (struct arg_struct*)arguments;

	struct sockaddr_in adresse;
	initAdresse(&adresse, PORT_BE);
	
	int serverSocket = initSocket(&adresse);
	
	while(1){

	    // Descripteur de la socket du client
        struct request* request = malloc(sizeof(struct request));
        bzero(request, sizeof(struct request));

        // Structure contenant l'adresse du client
        struct sockaddr_in clientAdresse;
        unsigned int addrLen = sizeof(clientAdresse);
        int clientSocket = original_accept(serverSocket, (struct sockaddr *) &clientAdresse, &addrLen);
        
        if (clientSocket != -1) {
            // Convertion de l'IP en texte
        } 
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAdresse.sin_addr), ip, INET_ADDRSTRLEN);
        printf("Connexion du thread 2 de sur la machine%s:%i\n", ip, clientAdresse.sin_port);
        //fcntl(clients, F_SETFL, O_NONBLOCK);
        int len = original_read(clientSocket, request, sizeof(struct request));

        printf("Valeur de l'id: %lu\n", request->id);
        printf("Valeur de la longueur: %d\n", request->lentgh);

        // Passage en mode non bloquant
        fcntl(clientSocket, F_SETFL, O_NONBLOCK);
        char* fichier = malloc(request->lentgh*sizeof(char));
        bzero(fichier, request->lentgh*sizeof(char));

        

        strcpy(fichier, buffer_share + request->id);
        printf("Le fichier vaut: %s\n", fichier);
        printf("La longueur fichier vaut: %ld\n",strlen(fichier));
        int val = original_write(clientSocket, fichier, strlen(fichier));
        close(clientSocket);
        free(request);
        free(fichier);
    
	}
}



void all_initialisation(){
        if (!isInitialise){ 
        bzero(buffer_share, BUFFER_SHARE_LEN*sizeof(char));
        isInitialise = true;

        //initialisation des tableaux
        bzero(right_tab, TAB_MAX_LENGHT*sizeof(int));
        bzero(left_tab, TAB_MAX_LENGHT*sizeof(int));
        indice_right = 0;
        indice_left = 0;

    
        //Créé le thread et initialise la socket si la variable PORT est renseignée lors de la compilation
        #ifndef PORT_BE
        #define PORT_BE 4000
        #endif
        struct arg_struct *args = malloc(sizeof(struct arg_struct));
        //args -> arg1 = PORT_BE;
        pthread_create(&a, NULL, traitement, NULL /*(void *)args*/);
        pthread_detach(a);
    };
}

int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen){
    original_connect_t original_connect = (original_connect_t)dlsym(RTLD_NEXT, "connect");
    all_initialisation();
    int result;
    if (result = original_connect(sockfd, serv_addr, addrlen) >= 0){
        right_tab[indice_right] = sockfd;
        indice_right++;
    }
    return result;
}   


int accept(int sockfd,  struct sockaddr *adresse, socklen_t * longueur){
    original_accept_t original_accept = (original_accept_t)dlsym(RTLD_NEXT, "accept");
    all_initialisation();
    int result = original_accept(sockfd, adresse, longueur);
    if (result != -1){
        left_tab[indice_left] = sockfd;
        indice_left++;
    }
    return result;
}

ssize_t write(int fd, const void *buf, size_t count){
    original_write_t original_write = (original_write_t)dlsym(RTLD_NEXT, "write");
    all_initialisation();
    printf("On est là, la longueur est: %ld\n", count);

    if(count<4096){
        struct Buff_reel* buffer = malloc(sizeof(struct Buff_reel));
        buffer->size = count;
        strcpy(buffer->preload,buf);
        printf("Buffer Preload: %s\n", buffer->preload);
        printf("Taille fichier: %ld\n", count);
        char etat = 'R';
        original_write(fd,&etat, sizeof(char));
        return original_write(fd,buffer, sizeof(struct Buff_reel));
        free(buffer);
    }
    else {
        //on écrit la valeur dans buffer share
        strcpy(buffer_share + identifiant,buf);

        struct Buff_vir* buffer = malloc(sizeof(struct Buff_vir));
        buffer->lentgh = count;
        buffer->size=15*sizeof(char)+2*sizeof(int)+sizeof(long);

        struct sockaddr_in clientAdresse;
        unsigned int addrLen = sizeof(clientAdresse);

        char ip[15];
        bzero(ip, 15);
        int port = PORT_BE;
	    //Initialisation de l'offset du buffer_share
	    buffer->offset = 0; 
        strcpy(buffer->ip,ip);
        buffer->port = port;
        buffer->id = identifiant ;   
        identifiant =+ buffer->lentgh ;
        printf("Valeur ip: %s\n", buffer->ip);
        printf("Valeur port: %d\n", buffer->port);
        printf("Valeur id: %ld\n", buffer->id);
        printf("Valeur lentgh: %d\n", buffer->lentgh);
        printf("Valeur ofset: %d\n", buffer->offset);
        char etat = 'V';
        original_write(fd,&etat, sizeof(char));
        return original_write(fd,buffer,sizeof(struct Buff_vir));
        free(buffer);
    }
}   




