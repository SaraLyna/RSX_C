CC = gcc
all : sendUDP receiveUDP

	
receiveUDP : receiveUDP.c
	$(CC) $? -o $@
	
sendUDP : sendUDP.c
	$(CC) $? -o $@
	
	
run: sendUDP receiveUDP
	./sendUDP & ./receiveUDP
	
