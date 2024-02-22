#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define DESTPORT "53"                  /* Port du serveur DNS */
#define NS_ANSWER_MAXLEN 512           /* Longueur maximale de la réponse DNS */
#define NS_QUERY_HEADER_LEN 12         /* Longueur de l'entête de la requête DNS */

int createDnsQuery(char* domain_name) {
    unsigned char answer[NS_ANSWER_MAXLEN];
    unsigned char query[NS_QUERY_HEADER_LEN + strlen(domain_name) + 2]; // 2 octets supplémentaires pour le TYPE et le CLASS
    int offset = NS_QUERY_HEADER_LEN;
    int count = 0;
    int size = strlen(domain_name);

    // Construction de l'entête de la requête DNS
    memset(query, 0, NS_QUERY_HEADER_LEN);
    query[1] = 0x01;  // Type de la requête (standard query)
    query[5] = 0x01;  // Nombre de questions (1)

    // Construction de la partie QNAME de la requête DNS
    while (*domain_name) {
        if (*domain_name == '.') {
            query[offset - count - 1] = count;
            count = 0;
        } else {
            query[offset++] = *domain_name;
            count++;
        }
        domain_name++;
    }
    query[offset - count - 1] = count; // Terminaison de la chaîne

    // Ajout du TYPE (A) et du CLASS (IN) à la requête DNS
    query[offset++] = 0x00;  // TYPE (high byte)
    query[offset++] = 0x01;  // TYPE (low byte)
    query[offset++] = 0x00;  // CLASS (high byte)
    query[offset++] = 0x01;  // CLASS (low byte)

    struct addrinfo af_hints, *af_result = NULL;
    memset(&af_hints, 0, sizeof(struct addrinfo));
    af_hints.ai_family = AF_INET;       /* Utilisation d'IPv4 */
    af_hints.ai_socktype = SOCK_DGRAM;  /* Utilisation de datagrammes UDP */

    fprintf(stderr, "Recherche de l'adresse IPv4 pour le nom \"%s\" ... ", domain_name);
    int err = getaddrinfo(domain_name, DESTPORT, &af_hints, &af_result);
    if (err) {
        fprintf(stderr, "[Erreur] - getaddrinfo(\"%s\") -> \"%s\"\n", domain_name, gai_strerror(err));
        return EXIT_FAILURE;
    }
    fprintf(stderr, "[OK]\n");

    int sock = 0;
    fprintf(stderr, "Création du socket en mode DGRAM (UDP) ... ");
    sock = socket(af_result->ai_family, af_result->ai_socktype, af_result->ai_protocol);
    if (sock < 0) {
        perror("[Erreur] - socket ");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "[OK]\n");

    fprintf(stderr, "Envoi du message ... ");
    ssize_t len;
    if ((len = sendto(sock, query, offset, 0, af_result->ai_addr, sizeof(struct sockaddr))) < 0) {
        perror("[Erreur] - sendto ");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "[OK]\nLongueur du message envoyé : %ld\n", len);

    struct sockaddr_in addrRemoteFromRecv;
    socklen_t addrRemoteFromRecvlen = sizeof(struct sockaddr_in);

    fprintf(stderr, "Réception du message ... ");
    if ((len = recvfrom(sock, answer, NS_ANSWER_MAXLEN, 0, (struct sockaddr *) &addrRemoteFromRecv, &addrRemoteFromRecvlen)) < 0) {
        perror("[Erreur] - recvfrom ");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "[OK]\nLongueur du message reçu : %ld\n", len);

    fprintf(stderr, "Port distant : %hu\n", ntohs(addrRemoteFromRecv.sin_port));
    fprintf(stderr, "Adresse IPv4 distante : %s\n", inet_ntoa(addrRemoteFromRecv.sin_addr));

    close(sock);
    freeaddrinfo(af_result);

    fprintf(stdout, "Réponse DNS :\n");
    for (int i = 0; i < len; i++) {
        fprintf(stdout, "%.2X ", answer[i] & 0xff);
        if (((i + 1) % 16 == 0) || (i + 1 == len)) {
            for (int j = i + 1; j < ((i + 16) & ~15); j++) {
                fprintf(stdout, "   ");
            }
            fprintf(stdout, "\t");
            for (int j = i & ~15; j <= i; j++)
                fprintf(stdout, "%c", answer[j] > 31 && answer[j] < 128 ? (char) answer[j] : '.');
            fprintf(stdout, "\n");
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    createDnsQuery(argv[1]);
}

