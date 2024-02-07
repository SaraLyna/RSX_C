CC = gcc
all : DNSsimple sendUDP receiveUDP 

DNSsimple : DNSsimple.c
	$(CC) $? -o $@
	
receiveUDP : receiveUDP.c
	$(CC) $? -o $@
	
sendUDP : sendUDP.c
	$(CC) $? -o $@
	
run: DNSsimple sendUDP receiveUDP
	./DNSsimple & ./sendUDP & ./receiveUDP
	
