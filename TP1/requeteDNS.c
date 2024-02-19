#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#define DNS_SERVER "194.254.129.102" // Exemple d'adresse donnée
#define MAX_QUERY_SIZE 512
#define MAX_RESPONSE_SIZE 65536

struct DNS_Query {
    unsigned short id;
    unsigned short flags;
    unsigned short questions;
    unsigned short answers;
    unsigned short authorities;
    unsigned short additional;
    char *qname;
    unsigned short qtype;
    unsigned short qclass;
};

// Fonction pour coder le nom de domaine dans le format DNS
unsigned char* createDNSQuery(struct DNS_Query *query, int *dns_query_size) {
    *dns_query_size = 12 + strlen(query->qname) + 5;
    unsigned char *dns_query = (unsigned char*)malloc(*dns_query_size);

    // Vérification de l'allocation mémoire
    if (dns_query == NULL) {
        perror("Erreur lors de l'allocation de mémoire");
        exit(EXIT_FAILURE);
    }

    memset(dns_query, 0, *dns_query_size);

    // Remplissage des champs de la requête DNS
    *(unsigned short*)(dns_query) = htons(query->id);
    *(unsigned short*)(dns_query + 2) = htons(query->flags);
    *(unsigned short*)(dns_query + 4) = htons(query->questions);
    *(unsigned short*)(dns_query + 6) = htons(query->answers);
    *(unsigned short*)(dns_query + 8) = htons(query->authorities);
    *(unsigned short*)(dns_query + 10) = htons(query->additional);

    // Conversion du nom de domaine en format DNS (Q-Name)
    char *token = strtok(query->qname, ".");
    int index = 12;
    while (token != NULL) {
        dns_query[index++] = strlen(token);
        memcpy(dns_query + index, token, strlen(token));
        index += strlen(token);
        token = strtok(NULL, ".");
    }
    dns_query[index++] = 0;

    // Remplissage des champs Q-Type et Q-Class
    *(unsigned short*)(dns_query + index) = htons(query->qtype);
    index += 2;
    *(unsigned short*)(dns_query + index) = htons(query->qclass);

    return dns_query;
}


int recupDatagram(int sockfd, unsigned char *response) {
    int response_size = recvfrom(sockfd, response, MAX_RESPONSE_SIZE, 0, NULL, NULL);
    if (response_size == -1) {
        perror("Erreur lors de la réception du datagramme de retour");
        return -1;
    }
    return response_size;
}


int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct DNS_Query query;
    query.id = 0x08bb;
    query.flags = 0x0100;
    query.questions = 0x0001;
    query.answers = 0x0000;
    query.authorities = 0x0000;
    query.additional = 0x0000;
    query.qname = argv[1];
    query.qtype = 0x0001; 
    query.qclass = 0x0001; 

    int dns_query_size;
    unsigned char *dns_query = createDNSQuery(&query, &dns_query_size);

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

    int mes = sendto(sockfd, dns_query, dns_query_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (mes == -1) {
        perror("Erreur lors de l'envoi du message");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Message envoyé avec succès.\n");
    
    unsigned char response[MAX_RESPONSE_SIZE];
    int response_size = recupDatagram(sockfd, response);

    if (response_size == -1) {
        perror("Erreur lors de la réception du datagramme de retour");
        close(sockfd);
        exit(EXIT_FAILURE);
    } else {	  
		if (response_size < 12) {
		    perror("Taille du datagramme de réponse DNS incorrecte");
		    close(sockfd);
		    exit(EXIT_FAILURE);
		}
		unsigned short num_answers = ntohs(*(unsigned short*)(response + 6));

		unsigned char *ptr = response + 12;

		for (int i = 0; i < num_answers; i++) {
		 
		    if (*(unsigned short*)(ptr + 2) == htons(0x0001)) { 
			if (*(unsigned short*)(ptr + 10) == htons(4)) { 
			    printf("Adresse IPv4 : %d.%d.%d.%d\n", *(ptr + 12), *(ptr + 13), *(ptr + 14), *(ptr + 15));
			} else {
			    printf("Réponse DNS de type A invalide\n");
			}
		    }
		
		    ptr += 16; 
		}

    	printf("Datagramme de retour reçu avec succès.\n");
    
    }

    free(dns_query);
    close(sockfd);

    return 0;
}

