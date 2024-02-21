#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>

#define GETADDRINFO

#define DESTNAME "dns1.univ-lille.fr" /* Serveur DNS à interroger */
#define DESTPORT "53"                  /* Port du serveur DNS */

#define NS_QUERY_LEN  29               /* Longueur de la requête DNS */
#define NS_ANSWER_MAXLEN 512           /* Longueur maximale de la réponse DNS */

unsigned char query[NS_QUERY_LEN] = {
    /* En-tête de la requête DNS */
    0x08,  0xbb,  0x01,  0x00, 0x00,  0x01,  0x00,  0x00, 0x00,  0x00,  0x00,  0x00,
    /* Question (QNAME, QTYPE, QCLASS) */
    0x03,  0x77,  0x77,  0x77, 0x04,  0x6c,  0x69,  0x66, 0x6c,  0x02,  0x66,  0x72,
    0x00,  0x00,  0x01, 0x00,  0x01
};

unsigned char answer[NS_ANSWER_MAXLEN];

int main(void) {
    struct addrinfo af_hints, *af_result = NULL;
    memset(&af_hints, 0, sizeof(struct addrinfo));
    af_hints.ai_family   = AF_INET;    /* Utilisation d'IPv4 */
    af_hints.ai_socktype = SOCK_DGRAM; /* Utilisation de datagrammes UDP */

    fprintf(stderr, "Recherche de l'adresse IPv4 pour le nom \"%s\" ... ", DESTNAME);
    int err = getaddrinfo(DESTNAME, DESTPORT, &af_hints, &af_result);
    if (err) {
        fprintf(stderr,"[Erreur] - getaddinfo(\"%s\") -> \"%s\"\n", DESTNAME, gai_strerror(err));
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

    // Saisie du nom de domaine spécifique
    char domain_name[256];
    printf("Entrez le nom de domaine spécifique : ");
    scanf("%s", domain_name);

    // Vérification de la longueur du nom de domaine spécifique
    if (strlen(domain_name) > 255) {
        fprintf(stderr, "Erreur : Le nom de domaine est trop long.\n");
        return EXIT_FAILURE;
    }

    // Copie du nom de domaine spécifique dans la requête DNS
    int i, j;
    for (i = 12, j = 0; domain_name[j] != '\0'; ++i, ++j) {
        query[i] = domain_name[j];
    }
    query[i++] = 0x00; // Terminateur de chaîne pour QNAME
    query[i++] = 0x00; // TYPE (high byte)
    query[i++] = 0x01; // TYPE (low byte)
    query[i++] = 0x00; // CLASS (high byte)
    query[i++] = 0x01; // CLASS (low byte)

    fprintf(stderr, "Envoi du message ... ");
    ssize_t len;
    if ((len = sendto(sock, query, NS_QUERY_LEN, 0, af_result->ai_addr, sizeof(struct sockaddr_in))) < 0) {
        perror("[Erreur] - sendto ");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "[OK]\nLongueur du message envoyé : %lu\n", len);

    struct sockaddr_in addrRemoteFromRecv;
    socklen_t addrRemoteFromRecvlen = sizeof(struct sockaddr_in);

    fprintf(stderr, "Réception du message ... ");
    if ((len = recvfrom(sock, answer, NS_ANSWER_MAXLEN, 0, (struct sockaddr *) &addrRemoteFromRecv, &addrRemoteFromRecvlen)) < 0) {
        perror("[Erreur] - recvfrom ");
        return EXIT_FAILURE;
    }
    fprintf(stderr, "[OK]\nLongueur du message reçu : %lu\n", len);

    fprintf(stderr,"Port distant : %hu\n", ntohs(addrRemoteFromRecv.sin_port));
    fprintf(stderr,"Adresse IPv4 distante : %s\n", inet_ntoa(addrRemoteFromRecv.sin_addr));

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
