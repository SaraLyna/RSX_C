#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define DNS_SERVER "194.254.129.102" 

struct DNS_Query {
    unsigned short id;
    unsigned short flags;
    unsigned short questions;
    unsigned short answers;
    unsigned short authority;
    unsigned short additional;
    char *qname;
    unsigned short qtype;
    unsigned short qclass;
};



unsigned char* createDNSQuery(struct DNS_Query *query) {
    unsigned char *dns_query;
    int dns_query_size = 12 + strlen(query->qname) + 5; // Taille totale de la requête DNS

    dns_query = (unsigned char*)malloc(dns_query_size);
    if (dns_query == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    // Initialisation de la requête DNS
    memset(dns_query, 0, dns_query_size);

    // Remplissage des champs de l'en-tête DNS
    *(unsigned short *)(dns_query) = htons(query->identifier);
    *(unsigned short *)(dns_query + 2) = htons(query->flags);
    *(unsigned short *)(dns_query + 4) = htons(query->questions);
    *(unsigned short *)(dns_query + 6) = htons(query->answers);
    *(unsigned short *)(dns_query + 8) = htons(query->authority);
    *(unsigned short *)(dns_query + 10) = htons(query->additional);

    // Conversion du nom de domaine en format DNS (Q-Name)
    char *token = strtok(query->qname, ".");
    int index = 12;
    while (token != NULL) {
        dns_query[index++] = strlen(token);
        memcpy(dns_query + index, token, strlen(token));
        index += strlen(token);
        token = strtok(NULL, ".");
    }
    dns_query[index++] = 0; // Octet de fin de nom de domaine

    // Remplissage des champs Q-Type et Q-Class
    *(unsigned short *)(dns_query + index) = htons(query->qtype);
    index += 2;
    *(unsigned short *)(dns_query + index) = htons(query->qclass);

    return dns_query;
}



int main() {
    struct DNS_Query query;
    query.id = 0x08bb; // Identifiant
    query.flags = 0x0100; // Paramètres
    query.questions = 0x0001; // Nombre de questions
    query.answers = 0x0000; // Nombre de réponses
    query.authority = 0x0000; // Nombre d'autorités
    query.additional = 0x0000; // Nombre d'informations additionnelles
    query.qname = "www.lifl.fr"; // Nom de domaine
    query.qtype = 0x0001; // Type de requête (A record)
    query.qclass = 0x0001; // Classe de requête (IN for Internet)

    
    unsigned char *dns_query = createDNSQuery(&query);

  
    int sockfd;
    struct sockaddr_in dest_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(53); // Port DNS
    dest_addr.sin_addr.s_addr = inet_addr(DNS_SERVER);

    if (sendto(sockfd, dns_query, strlen(dns_query), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
        perror("Erreur lors de l'envoi de la requête DNS");
        exit(EXIT_FAILURE);
    }

    printf("Requête DNS envoyée.\n");

    free(dns_query);
    close(sockfd);

    return 0;
}
