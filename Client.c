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

void initAdresse(struct sockaddr_in * adresse, char* port);
int initSocket(struct sockaddr_in * adresse, char* port);

int main(int argc, char* argv[]) {

	struct sockaddr_in adresse;
    int valread;
    char buffer[1024]; 
	initAdresse(&adresse, argv[1]);
	int serverSocket = initSocket(&adresse, argv[1]);
    int status;
    if ((status
         = connect(serverSocket, (struct sockaddr*)&adresse,
                   sizeof(adresse)))< 0) {
        printf("\nConnection Failed \n");
        return -1;
        }
    char *entree = (char*)malloc(BUFFER_LEN);
    printf("Entrez le mot à envoyer au serveur: \n");
    gets(entree);
    printf("Saisie : %s\n", entree);
    send(serverSocket, entree, BUFFER_LEN, 0);

    valread = read(serverSocket, buffer, 1024);

    printf("%s\n", buffer);

    free(entree);
	return EXIT_SUCCESS;
}
// Initialisation de la structure sockaddr_in
void initAdresse(struct sockaddr_in * adresse, char* port) {
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

	printf("Fin de l'initialisation\n");

    return fdsocket;
}


