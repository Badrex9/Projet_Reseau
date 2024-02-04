
# Project Réseau

Implémentation de ODB.




## Compilation

Compilation Back_end avec notre bibliothèque, la valeur PORT_BE peut être choisie par vos soins (par défaut 6000 dans l'exemple ci-dessous):

```bash
gcc -shared -o odb_be.so -lpthread -pthread -fPIC odb_be.c -ldl -DPORT_BE=6000
```
```bash
gcc Back_end.c -o BE
```

Compilation Front_end avec notre bibliothèque:

```bash
gcc -shared -o odb_fe.so -fPIC odb_fe.c -ldl
```
```bash
gcc Front_end.c -o FE
```
Compilation du serveur intermédiaire:

```bash
gcc Server_intermediaire.c -o SI
```


## Lancement des serveurs

Back-end, indiquer le port d'écoute de ce serveur (7000 ici): 

```bash
LD_PRELOAD=./odb_be.so ./BE 7000 
```

Front-end, indiquer le port d'écoute de ce serveur dans le premier argument (9000 ici) puis le port du serveur sur lequel il va se connecter (8000 ici): 

```bash
LD_PRELOAD=./odb_fe.so ./FE 9000 8000
```

Serveur-intermédiaire: Indiquer le port d'écoute (1er argument ici 8000) et de connection (2ième argument ici 7000). Il peut y avoir autant de serveur intermédiaire que désirés entre le front-end et le back-end.

```bash
./SI 8000 7000
```



## Tests

Des pages de différentes tailles sont disponibles pour vérifier la bonne exécution de ODB. 

On peut lancer les tests depuis un navigateur à l'adresse:

[http://localhost:PORT_FE/nom_du_test](http://localhost:PORT_FE/nom_du_test)

La variable PORT_FE est à remplacer par le port d'écoute choisit lors du lancement du serveur Front-End.
La variable nom_du_test est la page HTML à charger par le Back-end, nous en avons plusieures avec différentes tailles:

------------------ A MODIFIER -------------  
`1. Test.html: page de taille xxx`   
`2. Image.html: page de taille yyy`  
------------------ A MODIFIER -------------  

On peut aussi lancer les tests avec la commande suivante:
```bash
wget http://localhost:PORT_FE/nom_du_test
```