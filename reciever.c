// C Program for Message Queue (Reader Process)
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#define MAX_BYTES 20


void saveData(char *nameFile, char *data);

struct mesg_buffer {/* estrutura para a message queue*/
	long mesg_type;
	char mesg_text[MAX_BYTES];
} message;

int main(int argc, char** argv)
{
    key_t key;
	int msgid, len, dataSize, nPacks,remPacks=0;
    char filename[20], buffer[11];

    if(argc != 2) {
        printf("Execute in this way: ./sender file_out.txt\n");
        exit(-1);
    }
    strcpy(filename, argv[1]);/*guardar nome do file*/
   
	key = ftok("progfile", 65);/*gerar chave unica*/

    if((msgid = msgget(key, 0666 | IPC_CREAT))==-1){/*cria message queue e retorna id*/
        perror("error creating message queue");
    }
	
    len = msgrcv(msgid, &message, sizeof(message), 1, 0);
    dataSize = atoi(message.mesg_text);/*receber a messagem tamanho dos dados do ficheiro*/

    if((remPacks = dataSize%MAX_BYTES)!=0){ /*calcular numero de pacotes necessarios*/
        nPacks = dataSize/MAX_BYTES+1;
    }else{
        nPacks = dataSize/MAX_BYTES;
    }

    for(int i = 0; i < nPacks; i++){
        if((len = msgrcv(msgid, &message, sizeof(message), 1, 0)) == -1){
            perror("Error in recvfrom");
        }
        if((remPacks!=0) && (i == (nPacks -1))){
            strncpy(buffer, message.mesg_text, remPacks); /*pacote com dados inferiores a 10 bytes*/
            buffer[remPacks]='\0';
        }else{
            strncpy(buffer, message.mesg_text, MAX_BYTES);
            buffer[MAX_BYTES]='\0';
        }
        saveData(filename, buffer); /*gravar dados no file*/
    }
	printf("Receiver: file reception and saving is now complete.\n");
	msgctl(msgid, IPC_RMID, NULL);
	return 0;
}
void saveData(char *nameFile, char *data){
    FILE *file = fopen(nameFile,"a");/*abrir ficheiro para escrever*/
    if( file == NULL){
      exit(-1);
    }
    fprintf(file,"%s",data);//escrever para o ficheiro
    fclose(file);//fecha o ficheiro
}