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
#define PORT 5000

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
char buffer_share[BUFFER_SHARE_LEN]; //mmap
bool isInitialise = False;
original_write_t original_write = (original_write_t)dlsym(RTLD_NEXT, "write");
original_read_t original_read = (original_read_t)dlsym(RTLD_NEXT, "read");
original_connect_t original_connect = (original_connect_t)dlsym(RTLD_NEXT, "connect");
original_accecpt_t original_connect = (original_connect_t)dlsym(RTLD_NEXT, "connect");
pthread_t a;

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

void all_initialisation(){
    if (!isInitialise){ 
        bzero(buffer_share, BUFFER_SHARE_LEN*sizeof(char));
        isInitialise = True;


        struct arg_struct *args = malloc(sizeof(struct arg_struct));
        args -> arg1 = PORT; //Initialisé le 2nd paramètre du démarrage backend. 

        pthread_create(&a, NULL, traitement, (void *)args);
        //pthread_join(a, NULL);
    };
}

ssize_t write(int fd, const void *buf, size_t count){

    all_initialisation();

    if(type_preload(buf)=='R'){
        char* buffer = malloc(sizeof(struct Buff_reel));
        buffer->type ='R';
        buffer->size = count ;
        buffer->preload= *buff;
        return original_write(fd,buffer, sizeof(struct Buff_reel));
    }
    elif(type_preload(buf)=='v'){
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
        int port = ntohs(addr.sin_port);
	    //Initialisation de l'offset du buffer_share
	    buffer->offset = 0; 
        buffer->ip =ip ;
        buffer->port = port ;
        buffer->id = identifiant ;   
        identifiant =+ buffer->lentgh ;
        return original_write(fd,buffer,sizeof(struct Buff_vir));
    }
}   



void *traitement(void *arguments){

	struct arg_struct *args = (struct arg_struct*)arguments;

	struct sockaddr_in adresse;
	initAdresse(&adresse, args->arg1, args->arg1);
	
	int serverSocket = initSocket(&adresse, args->arg1);
	
	while(1){

	    // Descripteur de la socket du client
        struct request* request = malloc(sizeof(struct request));
        bzero(request, sizeof(struct request));

        // Structure contenant l'adresse du client
        struct sockaddr_in clientAdresse;
        unsigned int addrLen = sizeof(clientAdresse);
        int clientSocket;

        if ((clientSocket = original_accept(serverSocket, (struct sockaddr *) &clientAdresse, &addrLen)) != -1) {
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

        original_write(clientSocket, fichier, strlen(fichier),0);
        close(clientSocket);
    
	}
}

// Démarrage de la socket serveur
int initSocket(struct sockaddr_in * adresse, char* port){
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
	printf("Attachement de la socket sur le port %s\n", port);

	// Passage en écoute de la socket
	if (listen(fdsocket, BACKLOG) != 0) {
		printf("Echéc de la mise en écoute: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	printf("Fin de l'initialisation\n");

    return fdsocket;
}

// Initialisation de la structure sockaddr_in
void initAdresse(struct sockaddr_in * adresse, char* port, char* ip) {
	(*adresse).sin_family = AF_INET;
	(*adresse).sin_addr.s_addr = IP;
	(*adresse).sin_port = htons( atoi(port));
}
