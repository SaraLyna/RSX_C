CC = gcc
all : DNSsimple sendUDP receiveUDP  requeteDNS recupDatagram analyseDNS

DNSsimple : DNSsimple.c
	$(CC) $? -o $@
	
receiveUDP : receiveUDP.c
	$(CC) $? -o $@
	
sendUDP : sendUDP.c
	$(CC) $? -o $@
	
requeteDNS : requeteDNS.c
	$(CC) $? -o $@
	
recupDatagram : recupDatagram.c
	$(CC) $? -o $@
	
analyseDNS : analyseDNS.c
	$(CC) $? -o $@
	
run: DNSsimple sendUDP receiveUDP requeteDNS recupDatagram analyseDNS
	./DNSsimple & ./sendUDP & ./receiveUDP & ./requeteDNS & ./recupDatagram & ./analyseDNS
	
