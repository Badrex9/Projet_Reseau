#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>


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

struct Buff_vir
{
	char ip[15];
	long id;
	int offset;
	int lentgh;
};


void initAdresse(struct sockaddr_in * adresse, char* port);
void initAdresse_ip(struct sockaddr_in * adresse, char* port, char* ip);
int initSocket_client(struct sockaddr_in * adresse, char* port);
int initSocket_server(struct sockaddr_in * adresse, char* port);
int initSocket(struct sockaddr_in * adresse, char* port);
void manageClient(int clients, char* port, struct sockaddr_in adresse2);

int main(int argc, char* argv[]) {

	struct sockaddr_in adresse;
	initAdresse(&adresse, argv[1]);
	int serverSocket = initSocket_server(&adresse, argv[1]);

    //Initialisation connexion au serveur intermédiaire
    struct sockaddr_in adresse2;
    initAdresse(&adresse2, argv[2]);
    int serverSocket2 = initSocket_client(&adresse2, argv[2]);

	//while(1){
		manageClient(serverSocket, argv[2], adresse2);
	//}
}
// Initialisation de la structure sockaddr_in
void initAdresse(struct sockaddr_in * adresse, char* port) {
	(*adresse).sin_family = AF_INET;
	(*adresse).sin_addr.s_addr = IP;
	(*adresse).sin_port = htons( atoi(port));
}

// Initialisation de la structure sockaddr_in
void initAdresse_ip(struct sockaddr_in * adresse, char* port, char* ip) {
	(*adresse).sin_family = AF_INET;
	(*adresse).sin_addr.s_addr = IP;
	(*adresse).sin_port = htons( atoi(port));
}

// Démarrage de la socket serveur
int initSocket_client(struct sockaddr_in * adresse, char* port){
	// Descripteur de socket
	int fdsocket;

	// Création de la socket en TCP
	if ((fdsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		printf("Echéc de la création: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Création de la socket\n");

	printf("Fin de l'initialisation\n");

    return fdsocket;
}

// Démarrage de la socket serveur
int initSocket_server(struct sockaddr_in * adresse, char* port){
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
void manageClient(int clients, char* port, struct sockaddr_in adresse2) {
	printf("Retour ici\n");
    int longueur = 1024;
    char buffer_fichier[longueur];
    struct Buff_vir* buffer = malloc(sizeof(*buffer));

    int clientSocket;
    // Structure contenant l'adresse du client
    struct sockaddr_in clientAdresse;
    unsigned int addrLen = sizeof(clientAdresse);
	int serverSocket2;

    while ((clientSocket = accept(clients, (struct sockaddr *) &clientAdresse, &addrLen)) != -1) {
        char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAdresse.sin_addr), ip, INET_ADDRSTRLEN);
		printf("Connexion de %s:%i\n", ip, clientAdresse.sin_port);
		//fcntl(clients, F_SETFL, O_NONBLOCK);
		int len = read(clientSocket, buffer_fichier, sizeof(buffer_fichier));

		

		serverSocket2 = initSocket_client(&adresse2, port);
		//Connexion au serveur intermédiaire
		int status;
		if ((status
				= connect(serverSocket2, (struct sockaddr*)&adresse2,
						sizeof(adresse2)))< 0) {
				printf("\nConnection Failed \n");
		}

		inet_ntop(AF_INET, &(adresse2.sin_addr), ip, INET_ADDRSTRLEN);
		//Envoie du chemin au serveur intermédiaire


		send(serverSocket2, buffer_fichier, sizeof(buffer_fichier), 0);
		int valread;
		bzero(buffer, sizeof(*buffer));

		struct Buff_vir* buffer_virtuel = malloc(sizeof(struct Buff_vir));
		bzero(buffer_virtuel, sizeof(struct Buff_vir));
		//Lecture du fichier
		valread = read(serverSocket2, buffer_virtuel, sizeof(struct Buff_vir));
		//printf("Valeur dans le buffer: %s\n", buffer);

		close(serverSocket2);

		printf("Valeur de l'id: %lu\n", buffer_virtuel->id);
		printf("Valeur de l'ip: %s\n", buffer_virtuel->ip);
		printf("Valeur de la longueur: %d\n", buffer_virtuel->lentgh);
		printf("Valeur de l'offset: %d\n", buffer_virtuel->offset);

		//Envoie de la réponse du server base de donnée au client
		send(clientSocket, buffer_virtuel, sizeof(struct Buff_vir), 0);
		close(clientSocket);
	}
        
    }
	
