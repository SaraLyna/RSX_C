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
 08 BB :identifiant 
 81 80 : parametre FLAGS
 00 01 : 1 question
 00 02 : 2 réponses
 00 00 : 0 authorités
 00 00 : 0 additionnels
 03 77 77 77 :	. w w w
 04 6C 69 66 : . l i f
 6C 02 66 72 : l . f r
 00 
 00 01 :type A
 00 01 : class IN
 C0 0C : position de l'offset, donc là on a une réponse
 00 05 : Type CNAME
 00 01 : Class IN
 00 00 52 0F : TTL
 00 0D : RDLENGTH
 0A 70 72 6F 78 79 2D 69 6E 73 74 C0 10 : proxy-inst +"........"domain_name
 C0 29 : offset, réponse 2
 00 01 : TYPE A
 00 01 :  class A
 00 00 4A A0 : TTL
 00 04 : RDLENGTH
 C1 30 BA 7D : '......'@IP


- l'adresse IPv4 isolée est `C1 30 BA 7D`, qui est la représentation hexadécimale de l'adresse IPv4 `193.48.186.125`.

- Pour pouvoir analyser le datagramme DNS de retour, j'exécute le programme analyseDNS en lui donnant comme argument le datagramme de retour sans les espaces.
