#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>

// Taille du tampon de lecture des messages
#define BUFFER_LEN 200
//Chemin vers le dossier db
#define PATH_DB "/Users/zamaien/Documents/Projet Réseau/DB/"
//Longueur des lignes dans le fichier
#define LENGTH_RAW 1024

char *traitement_get(char buffer[BUFFER_LEN]);
char *str_sub (const char *s, unsigned int start, unsigned int end);
void ouverture_et_lecture_fichier(char path[BUFFER_LEN]);

int main(int argc, char* argv[]) {

    char *file = malloc(BUFFER_LEN);
    char path[BUFFER_LEN] = "/Users/zamaien/Documents/Projet Réseau/DB/";

    //Traitement de la requete GET: extraction du nom du fichier à chercher + ajout du chemin correspondant
    file = traitement_get(argv[1]);
    strcat(path, file);
    ouverture_et_lecture_fichier(path);
    //Traitement de la requete GET: ouverture du fichier précédent pour l'envoyer au client

    printf("Le chemin est: %s\n", path);
	return EXIT_SUCCESS;
}


char *traitement_get(char buffer[BUFFER_LEN]){
	if (buffer[0]=='G' && buffer[1]=='E' && buffer[2]=='T' && buffer[3]==' '){
        int i=4;
        int place_max = 4;
		// Test de chemin .../..../..../index.html
        while(buffer[i]!='\0'){
			if (buffer[i]=='/'){
				place_max = i;
			}
            i+=1;
		}

        char* nom_de_fichier = malloc(BUFFER_LEN);
        i = 0;

        if (buffer[place_max]=='/') place_max+=1;
        while(buffer[i+place_max]!='\0'){
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

void ouverture_et_lecture_fichier(char path[BUFFER_LEN]){
	FILE* fichier = NULL;
    char chaine[LENGTH_RAW] = "";
 
    fichier = fopen(path, "r");
 
    if (fichier != NULL)
    {
        while (fgets(chaine, LENGTH_RAW, fichier) != NULL) 
        {
            printf("%s", chaine); 
        }
 
        fclose(fichier);
    }
 
}