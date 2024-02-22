CC = gcc
all : DNSsimple  requeteDNS recupDatagram analyseDNS

DNSsimple : DNSsimple.c
	$(CC) $? -o $@
	
	
requeteDNS : requeteDNS.c
	$(CC) $? -o $@
	
recupDatagram : recupDatagram.c
	$(CC) $? -o $@
	
analyseDNS : analyseDNS.c
	$(CC) $? -o $@
	
run: DNSsimple requeteDNS recupDatagram analyseDNS
	./DNSsimple  & ./requeteDNS & ./recupDatagram & ./analyseDNS
	
