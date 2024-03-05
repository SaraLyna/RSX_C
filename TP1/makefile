CC = gcc
all : DNSsimple  DNSRequete analyseDNS

DNSsimple : DNSsimple.c
	$(CC) $? -o $@
	
	
requeteDNS : DNSRequete.c
	$(CC) $? -o $@
	
	
analyseDNS : analyseDNS.c
	$(CC) $? -o $@
	
run: DNSsimple DNSRequete analyseDNS
	./DNSsimple  & ./DNSRequete  & ./analyseDNS
	
