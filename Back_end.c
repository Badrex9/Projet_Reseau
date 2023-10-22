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

void initAdresse(struct sockaddr_in * adresse, char* port, char* ip);
int initSocket(struct sockaddr_in * adresse, char* port);
int manageClient(int clients, char* buffer);
char *traitement_get_str(char buffer[BUFFER_LEN]);
char* traitement_get(char buffer[BUFFER_LEN], int clientSocket);
char* ouverture_et_lecture_fichier(char path[BUFFER_LEN], int clientSocket);


struct Buff_vir
{
	char ip[15];
	int id;
	int offset;
	int lentgh;
};



int main(int argc, char* argv[]) {

	struct sockaddr_in adresse;
	initAdresse(&adresse, argv[1], argv[2]);
	int serverSocket = initSocket(&adresse, argv[1]);
	int socket = 0;
	char** buffer_share = malloc(sizeof(**buffer_share));
	struct Buff_vir* buffer_virtuel = malloc(sizeof(*buffer_virtuel));
	//Initialisation de l'offset du buffer_share
	buffer_virtuel->offset = 0;
	buffer_virtuel->lentgh = 0;
	//Initialisation de l'identifiant du buffer_share
	buffer_virtuel->id = 0;
	int* decalage = malloc(sizeof(*decalage));
	*decalage = 0;
	int i = 1;
	//Initialisation de @IP
	strcpy(buffer_virtuel->ip, argv[2]);
	printf("Longueur: %d\n", buffer_virtuel->lentgh);
	while(1){
        //On ajoute au buffer_share la valeur demandé en l'IS
		
        socket = manageClient(serverSocket, *buffer_share + buffer_virtuel->offset);
		printf("Longueur: %d\n", buffer_virtuel->lentgh);
		//On ajoute la longueur de la valeur précédente
		buffer_virtuel->lentgh = sizeof(*(buffer_share + buffer_virtuel->offset));
		//On envoie à l'IS 
		
		send(socket, buffer_virtuel, sizeof(buffer_virtuel),0);

		//On incrémente la valeur de l'offset en fonction de la précédente et la nouvelle longueur
		buffer_virtuel->offset += buffer_virtuel->lentgh;
		*(decalage + i) = buffer_virtuel->lentgh + *(decalage + i-1);
		buffer_virtuel->id ++;
		i+=1;
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
int manageClient(int clients, char* buffer_share) {
	
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
	fcntl(clientSocket, F_SETFL, O_NONBLOCK);

	strcpy(buffer_share, ouverture_et_lecture_fichier(buffer, clientSocket)); 

	return clientSocket;
}

char* ouverture_et_lecture_fichier(char path[BUFFER_LEN], int clientSocket){
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
	return envoie;
}