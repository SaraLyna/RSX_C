#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define NS_ANSWER_MAXLEN 512


unsigned char answer[NS_ANSWER_MAXLEN];


typedef struct DNS_Answer {
    char domain_name[256];
    char ipv4_address[16];
} DNS_Answer;


DNS_Answer analyze(unsigned char *answer, ssize_t len) {
    DNS_Answer result;
    memset(&result, 0, sizeof(DNS_Answer));
    
    if (len > 12) {
        // Vérification si la réponse est une réponse DNS
        if ((answer[2] & 0x80) == 0x80) {
            unsigned char *ipv4_start = answer + len - 4;
            snprintf(result.ipv4_address, sizeof(result.ipv4_address), "%d.%d.%d.%d", ipv4_start[0], ipv4_start[1], ipv4_start[2], ipv4_start[3]);
        }
    }
    return result;
}


void printDNSIPv4(DNS_Answer result) {
    printf("IPv4 Address: %s\n", result.ipv4_address);
}


int main (int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <reponse>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    ssize_t len = strlen(argv[1]) / 2;
    if (len > NS_ANSWER_MAXLEN) {
        fprintf(stderr, "Error: Response length exceeds maximum length\n");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < len; i++) {
        sscanf(argv[1] + i * 2, "%2hhx", &answer[i]);
    }
    
    DNS_Answer result = analyze(answer, len);
    printDNSIPv4(result);
    return 0; 
}

