#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>


// Port d'écoute de la socket
#define PORT 8080
// Adresse d'écoute (toutes les adresses)
#define IP INADDR_ANY
// Taille de la file d'attente
#define BACKLOG 3
// Nombre de connexions clients
#define NB_CLIENTS 2
// Taille du tampon de lecture des messages
#define BUFFER_LEN 200
// Commande pour arrêter le serveur
#define EXIT_WORD "exit"
//Longueur des lignes dans le fichier
#define LENGTH_RAW 1024
// Longueur buffer_share
#define BUFFER_SHARE_LEN 10000


struct Buff_vir
{
	char ip[15];
	long id;
	int offset;
	int lentgh;
};

struct arg_struct {
    char* arg1;
    char* arg2;
	char* buffer_share;
};

struct request{
	long id; 
	int lentgh;
};

void *envoie(void *arguments);
void initAdresse(struct sockaddr_in * adresse, char* port, char* ip);
int initSocket(struct sockaddr_in * adresse, char* port);
int manageClient(int clients, char* buffer, struct Buff_vir *buff_vir);
char *traitement_get_str(char buffer[BUFFER_LEN]);
char* traitement_get(char buffer[BUFFER_LEN], int clientSocket);
void ouverture_et_lecture_fichier(char path[BUFFER_LEN], int clientSocket, char* buffer_share, struct Buff_vir *buff_vir);
void *traitement(void *arguments);
int RequestFE(int clients, char* buffer_share);

int main(int argc, char* argv[]) {
    pthread_t a;
    pthread_t b;
	char buffer_share[BUFFER_SHARE_LEN];
	bzero(buffer_share, BUFFER_SHARE_LEN*sizeof(char));

	struct arg_struct *args = malloc(sizeof(struct arg_struct));
	
	args -> arg1 = argv[1];
	args -> arg2 = argv[2];
	args -> buffer_share = buffer_share;
	

	printf("Arg 1: %s\n", args -> arg1);
	printf("Arg 2: %s\n", args -> arg2);

    pthread_create(&a, NULL,envoie, (void *)args);
    pthread_create(&b, NULL, traitement, (void *)args);
    pthread_join(a, NULL);
    pthread_join(b, NULL);
	
}


//traitement request

void *traitement(void *arguments){
	struct arg_struct *args = (struct arg_struct*)arguments;

	struct sockaddr_in adresse;
	initAdresse(&adresse, args->arg2, args->arg2);
	
	int serverSocket = initSocket(&adresse, args->arg2);
	
	while(1){
		RequestFE(serverSocket, args->buffer_share);
	}

}


void *envoie(void *arguments){
	struct arg_struct *args = (struct arg_struct*)arguments;
	//args = malloc(sizeof(struct arg_struct));
	printf("Arg 1: %s\n", args -> arg1);
	struct sockaddr_in adresse;
	initAdresse(&adresse, args->arg1, args->arg2);
	
	int serverSocket = initSocket(&adresse, args->arg1);
	int socket = 0;
	struct Buff_vir* buffer_virtuel = malloc(sizeof(struct Buff_vir));
	bzero(buffer_virtuel, sizeof(struct Buff_vir));
	//Initialisation de l'offset du buffer_share
	buffer_virtuel->offset = 0;
	buffer_virtuel->lentgh = 0;
	//Initialisation de l'identifiant du buffer_share
	buffer_virtuel->id = 0;
	long sum = 0;
	//Initialisation de @IP
	strcpy(buffer_virtuel->ip, args->arg2);
	printf("Valeur de l'ip: %s\n", buffer_virtuel->ip);
	printf("Longueur: %d\n", buffer_virtuel->lentgh);
	while(1){
        //On ajoute au buffer_share la valeur demandé en l'IS
		
        socket = manageClient(serverSocket, args->buffer_share + buffer_virtuel->id, buffer_virtuel);
		printf("Longueur: %d\n", buffer_virtuel->lentgh);
		//On ajoute la longueur de la valeur précédente

		//On envoie à l'IS 
		printf("Valeur de l'id: %lu\n", buffer_virtuel->id);
    	printf("Valeur de l'ip: %s\n", buffer_virtuel->ip);
    	printf("Valeur de la longueur: %d\n", buffer_virtuel->lentgh);
   		printf("Valeur de l'offset: %d\n", buffer_virtuel->offset);
		int retour = send(socket, buffer_virtuel, sizeof(struct Buff_vir),0);
		if (retour < sizeof(struct Buff_vir)){
			printf("Pas envoyé\n");
			getchar();
		}

	
		sum += buffer_virtuel->lentgh;
		buffer_virtuel->id = sum;
		printf("Buffer share: %s\n", args->buffer_share );
		close(socket);
    }

	return EXIT_SUCCESS;
}
// Initialisation de la structure sockaddr_in
void initAdresse(struct sockaddr_in * adresse, char* port, char* ip) {
	(*adresse).sin_family = AF_INET;
	(*adresse).sin_addr.s_addr = IP;
	(*adresse).sin_port = htons( atoi(port));
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

// On traite l'input des clients
int RequestFE(int clients, char* buffer_share) {
	
    // Descripteur de la socket du client
    struct request* request = malloc(sizeof(struct request));
    bzero(request, sizeof(struct request));
    int clientSocket;
    // Structure contenant l'adresse du client
    struct sockaddr_in clientAdresse;
    unsigned int addrLen = sizeof(clientAdresse);


    if ((clientSocket = accept(clients, (struct sockaddr *) &clientAdresse, &addrLen)) != -1) {
        // Convertion de l'IP en texte
    } 
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAdresse.sin_addr), ip, INET_ADDRSTRLEN);
	printf("Connexion de sur la machine%s:%i\n", ip, clientAdresse.sin_port);
	//fcntl(clients, F_SETFL, O_NONBLOCK);
	int len = read(clientSocket, request, sizeof(struct request));

	printf("Valeur de l'id: %lu\n", request->id);
	printf("Valeur de la longueur: %d\n", request->lentgh);


	// Passage en mode non bloquant
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);
	char* fichier = malloc(request->lentgh*sizeof(char));
	bzero(fichier, request->lentgh*sizeof(char));

	strcpy(fichier, buffer_share + request->id);	

	printf("Le fichier vaut: %s\n", fichier);



	send(clientSocket, fichier, strlen(fichier),0);
	close(clientSocket);
	return clientSocket;
}


// On traite l'input des clients
int manageClient(int clients, char* buffer_share, struct Buff_vir *buff_vir) {
	
    // Descripteur de la socket du client
    char* buffer = malloc(BUFFER_LEN*sizeof(char));
    bzero(buffer, BUFFER_LEN);
    int clientSocket;
    // Structure contenant l'adresse du client
    struct sockaddr_in clientAdresse;
    unsigned int addrLen = sizeof(clientAdresse);


    if ((clientSocket = accept(clients, (struct sockaddr *) &clientAdresse, &addrLen)) != -1) {
        // Convertion de l'IP en texte
    }    
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAdresse.sin_addr), ip, INET_ADDRSTRLEN);
	printf("Connexion de %s:%i\n", ip, clientAdresse.sin_port);
	//fcntl(clients, F_SETFL, O_NONBLOCK);
	int len = read(clientSocket, buffer, BUFFER_LEN);

	printf("Valeur dans le buffer: %s\n", buffer);

	// Passage en mode non bloquant
	//fcntl(clientSocket, F_SETFL, O_NONBLOCK);

	ouverture_et_lecture_fichier(buffer, clientSocket, buffer_share, buff_vir);
	return clientSocket;
}

void ouverture_et_lecture_fichier(char path[BUFFER_LEN], int clientSocket, char* buffer_share, struct Buff_vir *buff_vir){
	FILE* fichier = NULL;
    char chaine[LENGTH_RAW] = "";
	
    fichier = fopen(path, "r");
	
	char* envoie = malloc(BUFFER_LEN*sizeof(char));
	bzero(envoie, BUFFER_LEN);

    if (fichier != NULL)
    {
		// Ajout de l'entête HTTP
		strcat(envoie, "HTTP/1.1 200 OK\n\n");
        while (fgets(chaine, LENGTH_RAW, fichier) != NULL) 
        {
			strcat(envoie, chaine);
        }
		printf("%s\n", envoie);
        fclose(fichier);
		//send(clientSocket, envoie, strlen(envoie), MSG_DONTWAIT);
    }
	buff_vir->lentgh = strlen(envoie);
	strcpy(buffer_share,envoie);
	
	//return envoie;
}