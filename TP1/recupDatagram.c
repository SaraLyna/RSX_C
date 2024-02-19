#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_RESPONSE_SIZE 65536

int recupDatagram(int sockfd, unsigned char *response) {
    int response_size = recvfrom(sockfd, response, MAX_RESPONSE_SIZE, 0, NULL, NULL);
    if (response_size == -1) {
        perror("Erreur lors de la réception du datagramme de retour");
        return -1;
    }
    return response_size;
}
