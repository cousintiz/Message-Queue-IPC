all: sender reciever

sender: sender.c 
	gcc -Wall sender.c -o sender
	
reciever: reciever.c 
	gcc -Wall reciever.c -o reciever
