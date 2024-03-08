# Rapport pour le TP1

## Semaine 1 :
Mercredi 24 Janvier :
- Réponse à la question 1 du TP
- Codage des fonctions sendUDP() et receiveUDP()
- dépot de la version en Java du premier semestre également pour faire la comparaison
- netcat -u : client
- netcat -l: server

## Semaine 2 :
Mercredi 7 Février :
- Réception du message sur le port 4200 :
./receiveUDP 4200
Message reçu :hello
- Envoi du message :
./sendUDP 127.1.1.3 4200 "hello"
- Makefile
- make all : tout compiler
- make run : tout exécuter
- modification du code de requeteDNS.c
- création des fichiers recupDatagram.c et analyseDNS.c pour le prochain tp


## Semaine 3 :
Mercredi 14 Février :
- Suite de DNSRequete.c 
- Réorganisation des fichiers.

## Semaine 4 :
Mercredi 21 Février :
- Travail sur le code pour recuperer le datagramme envoyé par DNSRequete.c dans le meme fichier.
- Résolution des bugs.
-----------------------------------------------------------------------------------------------
##                                      Analyse manuelle :
- l'adresse IPv4 isolée est `C1 30 BA 7D`, qui est la représentation hexadécimale de l'adresse IPv4 `193.48.186.125`.

- Pour pouvoir analyser le datagramme DNS de retour, j'exécute le programme analyseDNS en lui donnant comme argument le datagramme de retour sans les espaces.
