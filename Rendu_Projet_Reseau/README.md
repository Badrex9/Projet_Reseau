
# Project Réseau

Implémentation de ODB, projet réseau réalisé par Enzo Zamaï et Taha Aubouhan.  
Pour simplifier la compilation et l'éxécution, nous avons choisi de créer un MakeFile, les commandes correspondantes à ce fichier sont mensionnées ci-dessous. Les fichiers finaux se trouvent dans le dossier: Rendu_Projet_Reseau.

## Compilation

La compilation de chaque type de serveur (back-end, front-end, intermédiaire) avec sa biblothèque attribuée se fait automatiquement avec la commande: 

```bash
make all
```

La configuration initiale est donnée avec un serveur front-end écoutant sur le port 9000, un serveur intermédiaire écoutant sur le port 8000, un serveur back-end écoutant sur le port 7000 et dont les connections pour la version ODB se feront sur le port 6000.

`1. Pour la partie Back-end, la variable PORT_CONNEXION_BE peut être modifiée dans le Makefile, elle correspond au port de connection sur lequel le front-end ira si les fichiers sont de taille supérieure à 4096bits.  
La variable PORT_ECOUTE_BE est le port sur lequel le back-end attend les connections front-end ou serveurs intermédiaires pour le traitement du fichier demandé. Elle est modifiable de la même manière.`   
  
`2.Pour la partie serveur intermédiaire, la variable PORT_ECOUTE_SI peut être modifiée dans le MakeFile.`

`3.Pour la partie serveur intermédiaire, la variable PORT_ECOUTE_FE peut être modifiée dans le MakeFile.`


## Lancement des serveurs

Chaque serveur est lancable sur un terminal différent.  

`1.Le Back-end se lancera avec la commande:`
```bash
make run_BE
```
`2.Le(s) serveur(s) intermédiaire(s) se lancer(a, ont) avec la commande:`
```bash
make run_SI
```
`3.Le Front-end se lancera avec la commande:`
```bash
make run_FE
```

Par défaut, les ports sont ceux attribués dans les variables choisies lors de la compilation. Il est aussi possible de modifier ces ports directement lors de l'éxécution.

`1.Pour le Back-end:`
```bash
make run_BE PORT_ECOUTE_BE=AAAA
```
`2.Pour le(s) serveur(s) intermédiaire(s):`
```bash
make run_SI PORT_ECOUTE_SI=BBBB PORT_ECOUTE_BE=AAAA
```
`3.Pour le Front-end:`
```bash
make run_FE PORT_ECOUTE_FE=CCCC PORT_ECOUTE_SI=BBBB
```

## Tests

Des pages de différentes tailles sont disponibles pour vérifier la bonne exécution de ODB. Deux sont en dessous de 4096bits (Test.html, Image.html), l'autre est bien supérieure à 4096bits (copie du code html de la page d'accueil de Wikipédia, Wiki.html):

`1. Test.html: page de taille 567 bits`   
`2. Image.html: page de taille 682 bits`  
`3. Wiki.html: page de taille 342172 bits` 

Les commandes suivantes lancent les 3 tests dans l'odre précédent et si les configurations restent les mêmes que dans le Makefile.
```bash
make test1
```
```bash
make test2
```
```bash
make test3
```
Autrement, si vous avez changé le port du serveur front-end manuellement lors de l'execution, il faudra inscrire dans votre terminal de commande:
```bash
wget http://localhost:PORT_ECOUTE_FE/nom_du_test
```

On peut aussi lancer les tests depuis un navigateur à l'adresse:

[http://localhost:PORT_ECOUTE_FE/nom_du_test](http://localhost:PORT_ECOUTE_FE/nom_du_test)

La variable PORT_ECOUTE_FE est à remplacer par le port d'écoute choisit lors du lancement du serveur Front-End.
La variable nom_du_test est la page HTML à charger par le Back-end.

On peut aussi lancer les tests avec la commande suivante:
```bash
wget http://localhost:PORT_FE/nom_du_test
```

D'autres tests (pages html) peuvent être ajouter dans le dossier DB. Il faudra alors bien spécifier le nom_du_test en paramètre. 

La taille des buffers pour l'accueil des pages est élevé, nous avons choisi 3000000bits, le buffer qui stocke les différentes pages sur le back-end fait quand à lui 300000000bits.
## Authors

- [@Enzo Zamaï](https://www.github.com/Badrex9)

