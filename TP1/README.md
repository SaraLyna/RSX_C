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
 08 BB :identifiant de la trame dns
 81 80 : parametre FLAGS, ça indique le type de la requete dns
 la par exemple 81 indique que c'est une reponse, 80 indique qu'i=elle autoritative.
 00 01 : 1 question
 00 02 : 2 réponses
 00 00 : 0 authorités
 00 00 : 0 additionnels
 03 77 77 77 :	. w w w
 04 6C 69 66 : . l i f
 6C 02 66 72 : l . f r ------> ça indique le domaine demnadé dans la requete dns
 00 
 00 01 : reponse type A
 00 01 : class IN, pour internet
 C0 0C : position de l'offset, donc là on a une réponse
 00 05 : Type CNAME
 00 01 : Class IN
 00 00 52 0F : TTL,La durée de vie en secondes que le destinataire doit conserver les informations DNS,21 999 secondes --6 heures et 6 minutes
 00 0D : RDLENGTH, longueur des données de reponse en octets
 0A 70 72 6F 78 79 2D 69 6E 73 74 C0 10 : proxy-inst +"........"domain_name, les données reelles
 C0 29 : offset, réponse 2
 00 01 : TYPE A
 00 01 :  class A
 00 00 4A A0 : TTL, 19 200 secondes --5 heures et 20 minutes
 00 04 : RDLENGTH
 C1 30 BA 7D : '......'@IP, donnees reelles


- l'adresse IPv4 isolée est `C1 30 BA 7D`, qui est la représentation hexadécimale de l'adresse IPv4 `193.48.186.125`.

- Pour pouvoir analyser le datagramme DNS de retour,
j'ai d'abord créé une structure pour l'entete de dns.
pour la fonction analyze elle prend en entrée un pointeur vers un tableau d'octets representant la reponse dns brute et la taille du tableau,
je commence par initialiser struct dns pour stocker les differenst champs de l'entete, ensuite j'extrais les informations une par une, à l'aide du décalage de bits et d'operations de masquage car les donnees sont compactes et stockées sous forme binaire pour économiser de la bande passante réseau, alors moi j'utilise le decalage et le masquage pour ajouter des zeros a droite ou a gauche. ca sert a bien interpreter les octets.
et ensuite j'affiche ces informations en parcourant la reponse dns brute octet par octet,
la variable offset est utilisée pour suivre la position actuelle dans la réponse dns brute, 
et par exemple pour la compression je verifie les deux premiers bits de l'octet qui sont definis sur 11 pour la compression,


