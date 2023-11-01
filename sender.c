#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BYTES 20

char* data; /*para guardar os dados*/
void erro(char *msg);
int getData(char *nameFile, int* _dataSize);

struct mesg_buffer{/* estrutura para a message queue*/
    long mesg_type;
    char mesg_text[MAX_BYTES];
} message;

int main(int argc, char** argv){

    key_t key;
	int msgid, dataSize, nPacks;

    if(argc != 2) {
        printf("Execute in this way: ./sender file_in.txt\n");
        exit(-1);
    }

    if(getData(argv[1],&dataSize) == -1)
        erro("openning file");
   
    if((dataSize%MAX_BYTES)!=0){ /*calcular numero de pacotes necessarios*/
        nPacks = dataSize/MAX_BYTES+1;
    }else{
        nPacks = dataSize/MAX_BYTES;
    }
    key = ftok("progfile", 65); /*gerar chave unica*/

    if((msgid = msgget(key, 0666 | IPC_CREAT))==-1){/*cria message queue e retorna id*/
        perror("error creating message queue");
    }
    message.mesg_type = 1; 

    sprintf(message.mesg_text, "%d", dataSize);  
    if( msgsnd(msgid, &message, sizeof(message), 0)==-1)/*enviar numero de bytes lidos do file*/
        erro("Sending data");

    for(int i = 0; i < nPacks; i++){ /*enviar os bacotes de 10 em 10 bytes*/
        strncpy(message.mesg_text, data + (i * MAX_BYTES), MAX_BYTES);
        if(msgsnd(msgid, &message, sizeof(message), 0)==-1)
            erro("Sending data");
    }
    free(data); /*libertar a memoria*/
    printf("Sender: file reading and sending is now complete.\n");
    exit(0);
}

int getData(char *nameFile, int* _dataSize){
    FILE *file = fopen(nameFile,"r");/*abrir ficheiro para leitura*/
    if( file == NULL){
      return -1;
    }

    fseek(file, 0L, SEEK_END);/*por fptr no fim do file*/
    *_dataSize = ftell(file);/*obter tamanho do ficheiro*/
    
    fseek(file, -(*_dataSize), SEEK_CUR);/*voltar a por fptr no inicio*/
    data = (char*) malloc( (*_dataSize)  * sizeof(char));/*alocar memoria para dados do file*/
    fread(data, sizeof(char), *_dataSize, file);/*ler ficheiro*/
 
    fclose(file);
    return 0;
}
void erro(char *msg){
    fprintf(stderr, "Error: %s\n", msg);
    exit(-1);
}
