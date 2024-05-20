#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define SUCESS 0
#define FAILURE -1
#define MAX_BYTES 50

void save_data(char *nameFile, char *data);

struct mesg_buffer{
	long mesg_type;
	char mesg_text[MAX_BYTES];
}message;

int main(int argc, char** argv)
{
    key_t key;
    char filename[20], buffer[11];
    int msgid, len, dataSize, nPacks,remPacks=0;

    if(argc != 2)
    {
        printf("Execute in this way: ./sender file_out.txt\n");
        exit(EXIT_FAILURE);
    }
    strcpy(filename, argv[1]);
   
	key = ftok("progfile", 65);

    if((msgid = msgget(key, 0666 | IPC_CREAT)) == FAILURE)
    {
        perror("error creating message queue");
    }
	
    len = msgrcv(msgid, &message, sizeof(message), 1, 0);
    dataSize = atoi(message.mesg_text);

    if( (remPacks = dataSize%MAX_BYTES) != 0)
    { 
        nPacks = dataSize/MAX_BYTES+1;
    }else
    {
        nPacks = dataSize/MAX_BYTES;
    }

    for(int i = 0; i < nPacks; i++)
    {
        if((len = msgrcv(msgid, &message, sizeof(message), 1, 0)) == FAILURE){
            perror("Error in recvfrom");
        }
        if((remPacks!=0) && (i == (nPacks -1))){
            strncpy(buffer, message.mesg_text, remPacks); 
            buffer[remPacks]='\0';
        }else{
            strncpy(buffer, message.mesg_text, MAX_BYTES);
            buffer[MAX_BYTES]='\0';
        }
        save_data(filename, buffer); 
    }
    
	printf("Receiver: file reception and saving is now complete.\n");
	msgctl(msgid, IPC_RMID, NULL);

	return EXIT_SUCCESS;
}

void save_data(char *nameFile, char *data)
{
    FILE *file = fopen(nameFile,"a");
    if(file == NULL)
    {
      exit(EXIT_FAILURE);
    }
    fprintf(file,"%s",data);
    fclose(file);
}
