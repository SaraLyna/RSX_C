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

 void receiveUDP(struct ReceiveUDP *UDP) {
  int socket;
  struct sockaddr_in addrRemoteFromRecv;
  socklen_t addrRemoteFromRecvlen = sizeof(struct sockaddr_in);


  if (socket < 0) {
    perror("socket incorrect");
    exit(EXIT_FAILURE);
  }

  if((int len = recvfrom(socket, answer, NS_ANSWER_MAXLEN, 0, (struct sockaddr *) &addrRemoteFromRecv, &addrRemoteFromRecvlen)) < 0) {
    perror("[erreur] - recvfrom ");
    exit(EXIT_FAILURE);
  }




  close(socket);
}



int main(int argc, char *argv[]){
  if(argc != 4 ){
    exit(EXIT_FAILURE);
  }

  struct ReceiveUDP r;
  receiveUDP(&r);


  return 0;
}
