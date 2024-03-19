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

char* data;
static void erro(char *msg);
static int get_data(char *nameFile, int* _dataSize);

struct mesg_buffer{
    long mesg_type;
    char mesg_text[MAX_BYTES];
}message;

int main(int argc, char** argv){

    key_t key;
	int msgid, dataSize, nPacks;

    if(argc != 2) 
    {
        printf("Execute in this way: ./sender file_in.txt\n");
        exit(EXIT_FAILURE);
    }

    if(get_data(argv[1], &dataSize) == FAILURE)
    {
        erro("openning file");
    }

    if((dataSize%MAX_BYTES) != 0)
    { 
        nPacks = dataSize/MAX_BYTES+1;
    }else{
        nPacks = dataSize/MAX_BYTES;
    }

    key = ftok("progfile", 65); 
    
    if((msgid = msgget(key, 0666 | IPC_CREAT)) == FAILURE)
    {
        perror("error creating message queue");
    }

    message.mesg_type = 1; 
    sprintf(message.mesg_text, "%d", dataSize);

    if( msgsnd(msgid, &message, sizeof(message), 0) == FAILURE)
    {
        erro("Sending data");
    }

    for(int i = 0; i < nPacks; i++)
    { 
        strncpy(message.mesg_text, data + (i * MAX_BYTES), MAX_BYTES);
        if(msgsnd(msgid, &message, sizeof(message), 0) == FAILURE)
        {
            erro("Sending data");
        }
    }

    free(data); 
    printf("Sender: file reading and sending is now complete.\n");
    exit(EXIT_SUCCESS);
}

static int get_data(char *nameFile, int* _dataSize)
{
    FILE *file = fopen(nameFile,"r");
    if( file == NULL)
    {
      return FAILURE;
    }

    fseek(file, 0L, SEEK_END);
    *_dataSize = ftell(file);
    
    fseek(file, -(*_dataSize), SEEK_CUR);
    data = (char*) malloc( (*_dataSize)  * sizeof(char));
    fread(data, sizeof(char), *_dataSize, file);
 
    fclose(file);
    return SUCESS;
}

static void erro(char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}
