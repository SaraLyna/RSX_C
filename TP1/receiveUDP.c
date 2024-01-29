#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>

// struct sockaddr_in
// {
// short sin_family; /* famille d’adresse = AF_INET */
// u_short sin_port; /* port UDP ou TCP `a utiliser */
// struct in_addr sin_addr; /* 4 octets de l’adresse internet */
// char sin_zero[8]; /* doit valoir 0 */
// };
//
//
// /* Création de la socket */
// int socket(int domain, int type, int protocol);
//
// /* Liaison de la socket `a une adresse et un port */
// int bind(int socket,
// const struct sockaddr *address, socklen_t address_len);
//
// /* Envoi d’un message */
// int sendto(int s, const void * msg, size_t len, int flags,
// const struct sockaddr * dest_addr, socklen_t dest_addr_len);
//
// /* Reception d’un message */
// int recv(int s, void * buf, size_t len, int flags);
// int recvfrom(int s, void * buf, size_t len, int flags,
// struct sockaddr * from_addr, socklen_t * from_addr_len);



/* -------------------------------------------------------------- */
/* Question 1: */
struct ReceiveUDP {
    int port;
    int socket;
    struct sockaddr_in server_addr;
    socklen_t addr_len;
    char buffer[512];
};

 void receiveUDP(struct ReceiveUDP *rUDP) {
  rUDP->socket = socket(AF_INET, SOCK_DGRAM, 0);

  if (rUDP->socket < 0) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
  }
  
  memset(&rUDP->server_addr, 0, sizeof(rUDP->server_addr));
  rUDP->server_addr.sin_family = AF_INET;
  rUDP->server_addr.sin_addr.s_addr = INADDR_ANY;
  rUDP->server_addr.sin_port = htons(rUDP->port);


  if (bind(rUDP->socket, (struct sockaddr *)&(rUDP->server_addr), sizeof(rUDP->server_addr)) < 0) {
        perror("Erreur lors de la liaison de la socket");
        exit(EXIT_FAILURE);
  }
  
  rUDP->addr_len = sizeof(struct sockaddr_in);
  int len = recvfrom(rUDP->socket, rUDP->buffer, sizeof(rUDP->buffer), 0, (struct sockaddr *)&(rUDP->server_addr), &(rUDP->addr_len));

  if (len < 0) {
        perror("Erreur lors de la réception des données");
        exit(EXIT_FAILURE);
  }




  close(rUDP->socket);
}



int main(int argc, char *argv[]){
  if(argc != 2 ){
    exit(EXIT_FAILURE);
  }

  struct ReceiveUDP r;
  r.port = atoi(argv[1]);
  receiveUDP(&r);


  return 0;
}
