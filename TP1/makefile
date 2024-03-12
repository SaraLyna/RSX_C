CC = gcc
all : DNSsimple  DNSRequete 

DNSsimple : DNSsimple.c
	$(CC) $? -o $@
	
	
requeteDNS : DNSRequete.c
	$(CC) $? -o $@
	
	
	
run: DNSsimple DNSRequete analyseDNS
	./DNSsimple  & ./DNSRequete  
	
