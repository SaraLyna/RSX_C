#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>

struct sockaddr_in
{
short sin_family; /* famille d’adresse = AF_INET */
u_short sin_port; /* port UDP ou TCP `a utiliser */
struct in_addr sin_addr; /* 4 octets de l’adresse internet */
char sin_zero[8]; /* doit valoir 0 */
};


/* Création de la socket */
int socket(int domain, int type, int protocol);

/* Liaison de la socket `a une adresse et un port */
int bind(int socket,
const struct sockaddr *address, socklen_t address_len);

/* Envoi d’un message */
int sendto(int s, const void * msg, size_t len, int flags,
const struct sockaddr * dest_addr, socklen_t dest_addr_len);

/* Reception d’un message */
int recv(int s, void * buf, size_t len, int flags);

int recvfrom(int s, void * buf, size_t len, int flags,
struct sockaddr * from_addr, socklen_t * from_addr_len);



/* -------------------------------------------------------------- */
/* Question 1: */
struct SendUDP {
    char name[256];
    int port;
    char msg[256];
};


void sendUDP(struct SendUDP *sUDP) {
  int socket ;
  struct sockaddr_in server;
  socklen_t addr_len = sizeof(server);

  socket = socket(AF_INET, SOCK_DGRAM, 0);



  if (socket < 0) {
    perror("socket incorrect");
    exit(EXIT_FAILURE);
  }
  

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(sUDP->port);
  inet_aton(sUDP->name, &server.sin_addr);



  if (sendto(socket, sUDP->msg, strlen(sUDP->msg), 0, (struct sockaddr *)&server, addr_len) < 0) {
      perror("Error");
      close(socket);
      exit(EXIT_FAILURE);
  }

  close(socket);
}


int main(int argc, char *argv[]){
  if(argc != 4 ){
    exit(EXIT_FAILURE);
  }

  struct SendUDP s;
  strncpy(s.name, argv[1], sizeof(s.name));
  s.port = atoi(argv[2]);
  strncpy(s.msg, argv[3], sizeof(s.msg));

  sendUDP(&s);

  return 0;
}
