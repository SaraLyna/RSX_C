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

struct DNS {
    unsigned short id;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

void analyze(unsigned char* dns_response, size_t length) {
    struct DNS dns;
    // En-tête
    dns.id = (dns_response[0] << 8) | dns_response[1];
    dns.flags = (dns_response[2] << 8) | dns_response[3];
    dns.qdcount = (dns_response[4] << 8) | dns_response[5];
    dns.ancount = (dns_response[6] << 8) | dns_response[7];
    dns.nscount = (dns_response[8] << 8) | dns_response[9];
    dns.arcount = (dns_response[10] << 8) | dns_response[11];

    printf("-- Entete --\n");
    printf("%04X : IDENTIFIANT\n", dns.id);
    printf("%04X : FLAGS\n", dns.flags);
    printf("Flags = %.4X : { QR:%d ; Opcode:%.4X ; AA:%d ; TC:%d ; RD:%d ; RA:%d ; Z:%.3X ; RCODE:%.4X }\n",
           dns.flags,
           (dns.flags & 0x8000) >> 15,  // QR (1 bit)
           (dns.flags & 0x7800) >> 11,  // Opcode (4 bits)
           (dns.flags & 0x0400) >> 10,  // AA (1 bit)
           (dns.flags & 0x0200) >> 9,   // TC (1 bit)
           (dns.flags & 0x0100) >> 8,   // RD (1 bit)
           (dns.flags & 0x0080) >> 7,   // RA (1 bit)
           (dns.flags & 0x0070) >> 4,   // Z (3 bits)
           (dns.flags & 0x000F));        // RCODE (4 bits)
    printf("%04X : QDCOUNT (Nombre de questions)\n", dns.qdcount);
    printf("%04X : ANCOUNT (Nombre de reponses)\n",dns.ancount);
    printf("%04X : NSCOUNT (Nombre d'authorités)\n",dns.nscount);
    printf("%04X : ARCOUNT (Nombre d'additionnels)\n",dns.arcount);

    size_t offset = 12; // on met l'offset à al position 12 car c'est la que se termine l'entete

    // Parcourir les questions
    printf("\n-- Questions --\n");
    for (int q = 0; q < dns.qdcount; q++) {
        printf("-- Question %d --\n", q + 1);
        
        int name_length = 0;
        int name_offset = offset;

        while (dns_response[name_offset] != 0x00) {
            int label_length = dns_response[name_offset];
            name_offset += label_length + 1;
            name_length += label_length + 1;
        }
        name_offset++; // Passer le 0x00
        
        
        for (int i = 1; i < name_length; i++) {
            printf("%.2X ", dns_response[offset + i]);
        }
        
        printf(": \"%.*s\"\n", name_length, dns_response + offset);
        offset = name_offset;
        
        


        printf("%.2X %.2X : QTYPE\n", dns_response[offset], dns_response[offset + 1]);
        printf("%.2X %.2X : QCLASS\n", dns_response[offset + 2], dns_response[offset + 3]);

        // Passer à la réponse suivante
        offset += 4;
    }

    // Parcourir les réponses
    printf("\n-- Reponses --\n");
    for (int i = 0; i < dns.ancount; i++) {
        printf("-- Reponse %d --\n", i + 1);

        // Compression de nom
        if ((dns_response[offset] & 0xc0) == 0xc0) {
            printf("%.2X %.2X :\n", dns_response[offset], dns_response[offset + 1]);
            offset += 2;
        } else {
            int nameOffset = offset;
            while (dns_response[offset] != 0x00) {
                if ((dns_response[offset] & 0xc0) == 0xc0) {
                    printf("%.2X %.2X (Compression) :\n", dns_response[offset], dns_response[offset + 1]);
                    offset+=2;
                    break;
                }
                printf("%.2X ", dns_response[offset]);
                offset++;
            }
            
            printf(": \"");
	    for (int i = nameOffset; i < offset; i++) {
		 printf("%c", dns_response[i]);
	    }
	    

	    printf("\"\n");
	    printf(": \"%.*s\"\n", (int)(offset - nameOffset), dns_response + nameOffset);
        }

        printf("%.2X %.2X : TYPE\n", dns_response[offset], dns_response[offset + 1]);
        printf("%.2X %.2X : CLASS\n", dns_response[offset + 2], dns_response[offset + 3]);
        printf("%.2X %.2X %.2X %.2X : TTL\n", dns_response[offset + 4], dns_response[offset + 5], dns_response[offset + 6], dns_response[offset + 7]);
        printf("%.2X %.2X : RDLENGTH\n", dns_response[offset + 8], dns_response[offset + 9]);


	unsigned char rdlength = (dns_response[offset + 8] << 8) | dns_response[offset + 9];

        // Type A (IPv4 address)
        if (dns_response[offset] == 0x00 && dns_response[offset + 1] == 0x01) {
            printf("%.2X %.2X %.2X %.2X :'", dns_response[offset + 10], dns_response[offset + 11], dns_response[offset + 12], dns_response[offset + 13]);
            printf("%d.%d.%d.%d'\n", dns_response[offset + 10], dns_response[offset + 11], dns_response[offset + 12], dns_response[offset + 13]);
            printf("*** REPONSE %d : %.2X %.2X %.2X %.2X EST L’ADRESSE IPv4 DE '%d.%d.%d.%d' ***\n", i + 1, dns_response[offset + 10], dns_response[offset + 11], dns_response[offset + 12], dns_response[offset + 13],dns_response[offset + 10], dns_response[offset + 11], dns_response[offset + 12], dns_response[offset + 13] );
        } else {
            printf("%.2X %.2X %.2X %.2X : \n", dns_response[offset + 10], dns_response[offset + 11], dns_response[offset + 12], dns_response[offset + 13]);
           
        }

        offset += rdlength + 10; // Passer à la réponse suivante
    }

    // Parcourir les autorités
    printf("\n-- Autorites --\n");
    for (int j = 0; j < dns.nscount; j++) {
        printf("-- Autorite %d --\n", j + 1);

        // Compression de nom
        if ((dns_response[offset] & 0xc0) == 0xc0) {
            printf("%.2X %.2X : Compression de nom\n", dns_response[offset], dns_response[offset + 1]);
            offset += 2;
        } else {
            int nameOffset = offset;
            while (dns_response[offset] != 0x00) {
                if ((dns_response[offset] & 0xc0) == 0xc0) {
		    printf("%.2X %.2X (Compression) :\n", dns_response[offset], dns_response[offset + 1]);
                    offset+=2;
                    break;
                }
                printf("%.2X ", dns_response[offset]);
                offset++;
            }
        
        }

        printf("%.2X %.2X : TYPE\n", dns_response[offset], dns_response[offset + 1]);
        printf("%.2X %.2X : CLASS\n", dns_response[offset + 2], dns_response[offset + 3]);
        printf("%.2X %.2X %.2X %.2X : TTL\n", dns_response[offset + 4], dns_response[offset + 5], dns_response[offset + 6], dns_response[offset + 7]);
        printf("%.2X %.2X : RDLENGTH\n", dns_response[offset + 8], dns_response[offset + 9]);

        unsigned char rdlength = (dns_response[offset + 8] << 8) | dns_response[offset + 9];

        // Type NS (name server)
        printf("Nom du serveur : ");
        for (int k = 0; k < rdlength; k++) {
            printf("%c", dns_response[offset + 10 + k]);
        }
        printf("\n");

        offset += rdlength + 10; // Passer à l'autorité suivante
    }

    // Parcourir les informations additionnelles
    printf("\n-- Informations additionnelles --\n");
    for (int m = 0; m < dns.arcount; m++) {
        printf("-- Information additionnelle %d --\n", m + 1);

        // Compression de nom
        if ((dns_response[offset] & 0xc0) == 0xc0) {
            printf("%.2X %.2X : Compression de nom\n", dns_response[offset], dns_response[offset + 1]);
            offset += 2;
        } else {
            while (dns_response[offset] != 0x00) {
                printf("%.2X ", dns_response[offset]);
                offset++;
            }
            offset++;
            printf(": Nom de l'information additionnelle\n");
        }

        printf("%.2X %.2X : TYPE\n", dns_response[offset], dns_response[offset + 1]);
        printf("%.2X %.2X : CLASS\n", dns_response[offset + 2], dns_response[offset + 3]);
        printf("%.2X %.2X %.2X %.2X : TTL\n", dns_response[offset + 4], dns_response[offset + 5], dns_response[offset + 6], dns_response[offset + 7]);
        printf("%.2X %.2X : RDLENGTH\n", dns_response[offset + 8], dns_response[offset + 9]);

        unsigned char rdlength = (dns_response[offset + 8] << 8) | dns_response[offset + 9];

        // Type A (IPv4 address)
        if (dns_response[offset] == 0x00 && dns_response[offset + 1] == 0x01) {
            printf("%.2X %.2X %.2X %.2X : Adresse IPv4\n", dns_response[offset + 10], dns_response[offset + 11], dns_response[offset + 12], dns_response[offset + 13]);
            printf("*** INFORMATION ADDITIONNELLE %d : %.2X %.2X %.2X %.2X EST L’ADRESSE IPv4 ***\n", m + 1, dns_response[offset + 10], dns_response[offset + 11], dns_response[offset + 12], dns_response[offset + 13]);
        } else {
            printf(" : Nom canonique\n");
        }

        offset += rdlength + 10; // Passer à l'information additionnelle suivante
    }
}



int create(char * domaine) {
    
    int NS_QUERY_LEN= 12 + 5 + strlen(domaine)+1;

    //12 premiers octets du tableau = entete
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
  

    int offset = 12;
   
    
    /*while (*domaine) {
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
     */
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

    analyze(answer, len);

printf(" -------------------------------- \n") ;

    return EXIT_SUCCESS;
}

/******************************************************************************************/

/******************************************************************************************/



int main (int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <domain_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    create(argv[1] ) ;
    return 0; 
}
