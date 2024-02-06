# Makefile for Project Réseau

all: BE FE SI

# Variables
PORT_CONNEXION_BE = 6000
PORT_ECOUTE_BE = 7000
PORT_ECOUTE_SI = 8000
PORT_ECOUTE_FE = 9000

# Compilation
BE: odb_be.c Back_end.c
	gcc -shared -o odb_be.so -lpthread -pthread -fPIC odb_be.c -ldl -DPORT_BE=$(PORT_CONNEXION_BE)
	gcc Back_end.c -o BE

FE: odb_fe.c Front_end.c
	gcc -shared -o odb_fe.so -fPIC odb_fe.c -ldl
	gcc Front_end.c -o FE

SI: Server_intermediaire.c
	gcc Server_intermediaire.c -o SI

# Lancement serveurs
run_BE: BE
	LD_PRELOAD=./odb_be.so ./BE $(PORT_ECOUTE_BE)

run_SI: SI
	./SI $(PORT_ECOUTE_SI) $(PORT_ECOUTE_BE)

run_FE: FE
	LD_PRELOAD=./odb_fe.so ./FE $(PORT_ECOUTE_FE) $(PORT_ECOUTE_SI)

# Lancement Tests
# Replace PORT_FE and nom_du_test with the actual values
test1:
	wget http://localhost:$(PORT_ECOUTE_FE)/Test.html
	
test2:
	wget http://localhost:$(PORT_ECOUTE_FE)/Image.html

test3:
	wget http://localhost:$(PORT_ECOUTE_FE)/Wiki.html
# Nettoyage
clean:
	rm -f BE FE SI odb_be.so odb_fe.so