#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>


int analyze(const unsigned char *reponse, int longueur_reponse) {
    int dns_header_size = 12; 
    int dns_answer_offset = dns_header_size; 
    
    // Vérification si la réponse contient des réponses
    if (longueur_reponse > dns_header_size) {
        // Vérification si la réponse est une réponse DNS
        if ((reponse[2] & 0x80) == 0x80) {
            // Nombre de réponses dans la section de réponse
            unsigned short answer_count = ntohs(*(unsigned short*)(reponse + 6));
            
            // Parcourir les réponses DNS
            for (int i = 0; i < answer_count; i++) {
                // Vérification si la réponse est un enregistrement de type A (IPv4)
                if (reponse[dns_answer_offset] == 0xC0 && reponse[dns_answer_offset + 1] == 0x0C) {
                    unsigned short type = ntohs(*(unsigned short*)(reponse + dns_answer_offset + 2));
                    unsigned short data_length = ntohs(*(unsigned short*)(reponse + dns_answer_offset + 10));
                    
                    if (type == 0x0001 && data_length == 4) { // Type A (IPv4) et longueur des données 4 (IPv4)
                        // Extraction et affichage de l'adresse IPv4
                        struct in_addr ipv4_addr;
                        memcpy(&ipv4_addr, reponse + dns_answer_offset + 12, 4);
                        printf("Adresse IPv4 extraite : %s\n", inet_ntoa(ipv4_addr));
                    }
                }
                
                // Déplacer l'offset vers la prochaine réponse DNS
                dns_answer_offset += 12 + ntohs(*(unsigned short*)(reponse + dns_answer_offset + 10));
            }
        }
    }
    return 0;
}

int main (int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s reponse\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    size_t len;
    analyze(argv[1], len ) ;
    return 0; 
}
