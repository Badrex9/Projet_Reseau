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

void initAdresse(struct sockaddr_in * adresse, char* port);
int initSocket(struct sockaddr_in * adresse, char* port);
void manageClient(int clients);
char *traitement_get_str(char buffer[BUFFER_LEN]);
void traitement_get(char buffer[BUFFER_LEN], int clientSocket);
void ouverture_et_lecture_fichier(char path[BUFFER_LEN], int clientSocket);

int main(int argc, char* argv[]) {

	struct sockaddr_in adresse;
	initAdresse(&adresse, argv[1]);
	int serverSocket = initSocket(&adresse, argv[1]);
    while(1){
        manageClient(serverSocket);
    }
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
void manageClient(int clients) {
	

    // Descripteur de la socket du client
    char buffer[BUFFER_LEN];
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

	printf("Valeur dans le buffer: %s", buffer);


	traitement_get(buffer, clientSocket);
	//write(clients, "Coucou\n", strlen("Coucou\n"));
}

char *traitement_get_str(char buffer[BUFFER_LEN]){
	if (buffer[0]=='G' && buffer[1]=='E' && buffer[2]=='T' && buffer[3]==' '){
        int i=4;
        int place_max = 4;
		// Test de chemin .../..../..../index.html
        while(buffer[i]!='\0' && buffer[i]!=' '){
			if (buffer[i]=='/'){
				place_max = i;
			}
            i+=1;
		}

        char* nom_de_fichier = malloc(BUFFER_LEN);
        i = 0;

        if (buffer[place_max]=='/') place_max+=1;
        while(buffer[i+place_max]!='\0' && buffer[i+place_max]!=' '){
            nom_de_fichier[i] = buffer[i+place_max];
            i+=1;
        }

        printf("Le nom du fichier est: %s\n", nom_de_fichier);
        return nom_de_fichier;
    }
	else{
		printf("Pas de nom de fichier\n");
		return "NON";
	} 
}

void traitement_get(char buffer[BUFFER_LEN], int clientSocket){
	char *file = malloc(BUFFER_LEN);
    char path[BUFFER_LEN] = "/Users/zamaien/Documents/Projet Réseau/DB/";

    //Traitement de la requete GET: extraction du nom du fichier à chercher + ajout du chemin correspondant
    file = traitement_get_str(buffer);
    strcat(path, file);

	ouverture_et_lecture_fichier(path, clientSocket);

    printf("Le chemin est: %s\n", path);

    //Traitement de la requete GET: ouverture du fichier précédent pour l'envoyer au client

}

void ouverture_et_lecture_fichier(char path[BUFFER_LEN], int clientSocket){
	FILE* fichier = NULL;
    char chaine[LENGTH_RAW] = "";
	
    fichier = fopen(path, "r");
	
    if (fichier != NULL)
    {
        while (fgets(chaine, LENGTH_RAW, fichier) != NULL) 
        {
            write(clientSocket, chaine, strlen(chaine));
			printf("%s", chaine);
        }
 
        fclose(fichier);
    }
 
}