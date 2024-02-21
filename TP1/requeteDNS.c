#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#define DNS_SERVER "10.140.6.45" // Exemple d'adresse donnée
#define MAX_QUERY_SIZE 512
#define MAX_RESPONSE_SIZE 65536

struct DNS_Query {
    unsigned short id;
    unsigned short flags;
    unsigned short questions;
    unsigned short answers;
    unsigned short authorities;
    unsigned short additional;
    unsigned char *qname;
    unsigned short qtype;
    unsigned short qclass;
};

// Fonction pour coder le nom de domaine dans le format DNS
unsigned char* createDNSQuery(const char *qname, unsigned short qtype, unsigned short qclass, int *dns_query_size) {
    int qname_len = strlen(qname);

    *dns_query_size = 12 + qname_len + 5;
    unsigned char *dns_query = (unsigned char*)malloc(*dns_query_size);

    // Vérification de l'allocation mémoire
    if (dns_query == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    memset(dns_query, 0, *dns_query_size);
    int index = 0;
    *(unsigned short*)(dns_query + index) = htons(0x08bb);
    index += 2;
    *(unsigned short*)(dns_query + index) = htons(0x0100);
    index += 2;
    *(unsigned short*)(dns_query + index) = htons(0x0001);
    index += 2;
    *(unsigned short*)(dns_query + index) = htons(0x0000);
    index += 2;
    *(unsigned short*)(dns_query + index) = htons(0x0000);
    index += 2;
    *(unsigned short*)(dns_query + index) = htons(0x0000);
    index += 2;

    // Conversion du nom de domaine en format DNS (Q-Name)
    const char *token = strtok((char *)qname, ".");
    while (token != NULL) {
        dns_query[index++] = strlen(token);
        memcpy(dns_query + index, token, strlen(token));
        index += strlen(token);
        token = strtok(NULL, ".");
    }
    dns_query[index++] = 0;

    // Remplissage des champs Q-Type et Q-Class
    *(unsigned short*)(dns_query + index) = htons(qtype);
    index += 2;
    *(unsigned short*)(dns_query + index) = htons(qclass);

    // for(int i=0; i< dns_query_size; i++){
    //   printf("%02x", dns_query[i]);
    // }


    return dns_query;

}


int sendDNSQuery(const char *domain_name) {
    int sockfd;
    struct sockaddr_in dest_addr;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(53);
    dest_addr.sin_addr.s_addr = inet_addr(DNS_SERVER);


    struct DNS_Query query;
    query.id = 0x08bb;
    query.flags = 0x0100;
    query.questions = 0x0001;
    query.answers = 0x0000;
    query.authorities = 0x0000;
    query.additional = 0x0000;
    query.qname = strdup(domain_name);
    query.qtype = 0x0001;
    query.qclass = 0x0001;

    int dns_query_size;
    unsigned char *dns_query = createDNSQuery(query.qname, query.qtype, query.qclass, &dns_query_size);

    int mes = sendto(sockfd, dns_query, dns_query_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (mes == -1) {
        perror("Erreur lors de l'envoi du message");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Message envoyé avec succès.\n");

    unsigned char response[MAX_RESPONSE_SIZE];
    int response_size = recvfrom(sockfd, response, MAX_RESPONSE_SIZE, 0, NULL, NULL);

    if (response_size == -1) {
        perror("Erreur lors de la réception du datagramme de retour");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Contenu du datagramme envoyé en hexadécimal:\n");
    for (int i = 0; i < dns_query_size; i++) {
        printf("%.2X ", dns_query[i] & 0xFF);
    }
    printf("\n");

    printf("Datagramme de retour reçu avec succès. Taille : %d octets\n", response_size);

    printf("Contenu du datagramme de retour en hexadécimal:\n");
    for (int i = 0; i < response_size; i++) {
        printf("%.2X ", response[i] & 0xFF);
    }
    printf("\n");

    close(sockfd);
    free(dns_query);
    return response_size;

}


int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int response_size = sendDNSQuery(argv[1]);
    if (response_size < 12) {
        perror("Taille du datagramme de réponse DNS incorrecte");
        exit(EXIT_FAILURE);
    }

    return 0;
}
