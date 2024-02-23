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

#ifdef GETADDRINFO

#define DESTNAME "dns1.univ-lille.fr" /* "ns1.univ-lille.fr" */
#define DESTPORT "53"

#else

#define DESTIPV4 "10.140.6.45"      /* "194.254.129.102" */
#define DESTPORT 53

#endif


#define NS_ANSWER_MAXLEN 512


unsigned char answer[NS_ANSWER_MAXLEN];

int create(char * domaine) {
    
    int NS_QUERY_LEN= 12 + 5 + strlen(domaine)+1;

    unsigned char query[NS_QUERY_LEN];
    query[0] = 0x08;
    query[1] = 0xbb;
    query[2] = 0x01;
    query[3] = 0x00;
    query[4] = 0x00;
    query[5] = 0x01;
    query[6] = 0x00;
    query[7] = 0x00;
    query[8] = 0x00;
    query[9] = 0x00;
    query[10] = 0x00;
    query[11] = 0x00;
  

    int offset = 13;
    int count = 0;
   
    
    while (*domaine) {
        if (*domaine == '.') {
            query[offset - count - 1] = count;
            offset++;
            count=0;
        } else {
            query[offset++] = *domaine;
            count++;
        }
        domaine++;
    }
    query[offset - count - 1] = count; // Stocke la longueur du dernier label  
    query[offset++] = 0x00; // Terminaison de la chaîne


    query[offset++] = 0x00;  // TYPE (high byte)
    query[offset++] = 0x01;  // TYPE (low byte)
    query[offset++] = 0x00;  // CLASS (high byte)
    query[offset++] = 0x01;  // CLASS (low byte)
    
 


printf(" -------------------------------- \n") ;

for (int i = 0; i < NS_QUERY_LEN; i++) {

      fprintf(stdout," %.2X", query[i] & 0xff);

      if (((i+1)%16 == 0) || (i+1 == NS_QUERY_LEN)) {

        /* ceci pour afficher les caracteres ascii apres l'hexa */
        /* >>> */
        for (int j = i+1 ; j < ((i+16) & ~15); j++) {
          fprintf(stdout,"   ");
        }
        fprintf(stdout,"\t");
        for (int j = i & ~15; j <= i; j++)
          fprintf(stdout,"%c",query[j] > 31 && query[j] < 128 ? (char)query[j] : '.');
        /* <<< */
        fprintf(stdout,"\n");
      }
    }

printf(" -------------------------------- \n") ;


#ifdef GETADDRINFO

    struct addrinfo af_hints, *af_result = NULL;
    memset(&af_hints, 0, sizeof(struct addrinfo));
    af_hints.ai_family   = AF_INET;    /* IPv4 */
    af_hints.ai_socktype = SOCK_DGRAM; /* UDP */

    fprintf(stderr, " recherche de l'@IPv4, pour le nom \"%s\" ... ", DESTNAME);
    int err = getaddrinfo(DESTNAME, DESTPORT, &af_hints, &af_result);
    if (err) {
      fprintf(stderr,"[erreur] - getaddinfo(\"%s\") -> \"%s\"\n", DESTNAME, gai_strerror(err));
      return EXIT_FAILURE;
    }
    fprintf(stderr, "[ok]\n");

    int sock = 0;
    fprintf(stderr, " creation du socket en mode DGRAM (UDP) ... ");
    sock = socket(af_result->ai_family /* ou PF_INET */, af_result->ai_socktype /* ou SOCK_DGRAM */, af_result->ai_protocol /* ou 0 */);
    if (sock < 0) {
      perror("[erreur] - socket ");
      return EXIT_FAILURE;
    }
    fprintf(stderr, "[ok]\n");


#else

    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(struct sockaddr_in));
    remote_addr.sin_family      = AF_INET; /* IPv4 */
    remote_addr.sin_port        = htons(DESTPORT); /* numero de port dest */
    remote_addr.sin_addr.s_addr = inet_addr(DESTIPV4); /* @IPv4 dest */
    fprintf(stderr, " conversion de l'@IPv4, pour la chaine \"%s\" ... ", DESTIPV4);
    if (remote_addr.sin_addr.s_addr == INADDR_NONE) {
      fprintf(stderr,"[erreur] - inet_addr(\"%s\") -> Mauvais formatage\n", DESTIPV4);
      return EXIT_FAILURE;
    }
    fprintf(stderr, "[ok]\n");

    int sock = 0;
    fprintf(stderr, " creation du socket en mode DGRAM (UDP) ... ");
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
      perror("[erreur] - socket ");
      return EXIT_FAILURE;
    }
    fprintf(stderr, "[ok]\n");

#endif

    fprintf(stderr, " envoi du message ... ");
    ssize_t len;
    if ( (len = sendto(sock, query, NS_QUERY_LEN, 0,
#ifdef GETADDRINFO
                       af_result->ai_addr
#else
                       (struct sockaddr*) &remote_addr
#endif
                       , sizeof(struct sockaddr_in))) < 0) {
      perror("[erreur] - sendto ");
      return EXIT_FAILURE;
    }
    fprintf(stderr, "[ok]\n longueur du message envoye : %lu\n", len);

    struct sockaddr_in addrRemoteFromRecv;
    socklen_t addrRemoteFromRecvlen = sizeof(struct sockaddr_in);

    fprintf(stderr, " reception du message ... ");
    if ( (len = recvfrom(sock, answer, NS_ANSWER_MAXLEN, 0, (struct sockaddr *) &addrRemoteFromRecv, &addrRemoteFromRecvlen)) < 0) {
      perror("[erreur] - recvfrom ");
      return EXIT_FAILURE;
    }
    fprintf(stderr, "[ok]\n longueur du message recu : %lu\n", len);
    fprintf(stderr," - port  distant  : %hu\n", ntohs(addrRemoteFromRecv.sin_port));
    fprintf(stderr," - @IPv4 distante : %s\n", inet_ntoa(addrRemoteFromRecv.sin_addr));

    close(sock);

#ifdef GETADDRINFO
    freeaddrinfo(af_result);
#endif

printf(" -------------------------------- \n") ;

    for (int i = 0; i < len; i++) {

      fprintf(stdout," %.2X", answer[i] & 0xff);

      if (((i+1)%16 == 0) || (i+1 == len)) {

   
        for (int j = i+1 ; j < ((i+16) & ~15); j++) {       
          fprintf(stdout," ");        
        }
        fprintf(stdout,"\t");
        
        for (int j = i & ~15; j <= i; j++)
          fprintf(stdout,"%c",answer[j] > 31 && answer[j] < 128 ? (char)answer[j] : '.');
        /* <<< */
        fprintf(stdout,"\n");
      }
    }
printf(" -------------------------------- \n") ;

    return EXIT_SUCCESS;
}



int main (int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    create(argv[1] ) ;
    return 0; 
}
