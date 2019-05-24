########## -------- cliente.c --------- ###########
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
 
#define PORTA_PADRAO 7171
 
#define INICIO_MATRIZ  1
#define INICIO_TAM_MSG 10
#define INICIO_MSG     12
 
 

typedef enum boolean{false, true}bool;
 

int Erro(char mensagem[], int);
 

void IniciaSocket(int *, char ip[]);
 

bool VerificaRecuso(char c);
 

bool AnalisaFimPacote(char pacote[]);
 
void Desencapsula(char pacote[], char matriz_jogo[], char mensagem_servidor[]);
int  ConvertChar  (char);
void CharParaInt  (char pacote[], int *);
void ImprimirRecebido(char matriz_jogo[], char mensagem_servidor[]);
 
int main(int argc,char *argv[]){
    int  sock_programa;
    char ip[13];
 
    strcpy(ip, argv[1]);
    printf("IP: %s\n", ip);
 
    if(argc<1)
        return Erro("Insira IP do servidor!", -1);
 
    IniciaSocket(&sock_programa, ip);
    if(sock_programa == -1)
        return Erro("Erro: Socket!!!", -1);
 
    int  teste_erro;
    bool fim = false;
    char pacote_envio  [100];
    char pacote_receber[100];
 
    char matriz_jogo[9];
    char mensagem_servidor[88];
 
  
   testeerro= recv(sock_programa,pacote_receber,100,0);
    pacote_receber[teste_erro] = '\0';
    Desencapsula(pacote_receber, matriz_jogo, mensagem_servidor);
    system("clear");
    ImprimirRecebido(matriz_jogo, mensagem_servidor);
 
 
   
   testeerro= recv(sock_programa,pacote_receber,100,0);
    pacote_receber[teste_erro] = '\0';
    Desencapsula(pacote_receber, matriz_jogo, mensagem_servidor);
    system("clear");
    ImprimirRecebido(matriz_jogo, mensagem_servidor);
    
 
    do{
        do{
            if(teste_erro!=1 && !fim){
                
                fgets(pacote_envio,100,stdin);
                pacote_envio[strlen(pacote_envio)-1] = '\0';
               testeerro= send(sock_programa,pacote_envio,strlen(pacote_envio),0);
 
               testeerro= recv(sock_programa,pacote_receber,100,0);
                pacote_receber[teste_erro] = '\0';
                Desencapsula(pacote_receber, matriz_jogo, mensagem_servidor);
                system("clear");
                ImprimirRecebido(matriz_jogo, mensagem_servidor);
 
            }
            if(teste_erro!=-1)
                fim = AnalisaFimPacote(pacote_receber);
            if(fim)
                break;
        }while(VerificaRecuso(pacote_receber[0]) && teste_erro!=-1);
 
        if(teste_erro!=1 && !fim){
            
           testeerro= recv(sock_programa,pacote_receber,100,0);
            pacote_receber[teste_erro] = '\0';
            Desencapsula(pacote_receber, matriz_jogo, mensagem_servidor);
            system("clear");
            ImprimirRecebido(matriz_jogo, mensagem_servidor);
 
            if(teste_erro!=-1)
                fim = AnalisaFimPacote(pacote_receber);
        }
        if(fim)
            break;
    }while(teste_erro!=1 && !fim);
 
    printf("\n\n");
    close(sock_programa);
    return 0;
}
 
int Erro(char mensagem[], int returno){
    printf("\n%s\n", mensagem);
    return returno;
}
 
void IniciaSocket(int *sock_programa, char ip[]){
    struct sockaddr_in addr;
 
    *sock_programa = socket(AF_INET,SOCK_STREAM,0);
 
    if(*sock_programa == -1)
        printf("ERRO ao criar o socket!\n");
 
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORTA_PADRAO);
    addr.sin_addr.s_addr = inet_addr(ip);
 
    memset(&addr.sin_zero,0,sizeof(addr.sin_zero));
 
    printf("Tentando conectar com servidor...\n");
 
    if(connect(*sock_programa,(struct sockaddr*)&addr,sizeof(addr)) == -1){
        printf("Erro ao conectar!\n");        
        *sock_programa = -1;
    }else{
        printf("Conectado!!!... Aguarde uma resposta do servidor...\n\n");
    }
}       
 
bool VerificaRecuso(char c){
    if(c == 'x')
        return true;
    else
        return false;
}
 
bool AnalisaFimPacote(char pacote[]){
    char auxiliar_fim[3];
    int i;
 
    for(i=0; i<3; i++)
        auxiliar_fim[i] = pacote[INICIO_MSG+i];
 
    auxiliar_fim[3] = '\0';
 
    if(strcmp("FIM", auxiliar_fim) == 0)
        return true;
    else
        return false;
}
 
void Desencapsula(char pacote[], char matriz_jogo[], char mensagem_servidor[]){
    int i, cont=0;
    int tam = 0;
 
    for (i=INICIO_MATRIZ; i<INICIO_MATRIZ+9; i++){
        if(pacote[i]=='*')
            matriz_jogo[cont] = ' ';
        else
            matriz_jogo[cont] = pacote[i];
        cont++;
    }
    matriz_jogo[cont] = '\0';
 
    CharParaInt(pacote, &tam);
 
    cont = 0;
    for (i=INICIO_MSG; i<INICIO_MSG+tam; i++){
        mensagem_servidor[cont] = pacote[i];
        cont++;
    }
    mensagem_servidor[cont] = '\0';
}
 
void CharParaInt(char pacote[], int *tam){
    *tam = 10 * ConvertChar(pacote[INICIO_TAM_MSG]);
    *tam = *tam + ConvertChar(pacote[INICIO_TAM_MSG+1]);
}
 
int ConvertChar(char c){
    return (c)-48;
}
 
void ImprimirRecebido(char matriz_jogo[], char mensagem_servidor[]){
    int tam, i;
    printf("\n\n\n\t+--------------------------------------------------------+\n");
    printf("\t�                                                        �\n");
    printf("\t�                 Jogo da velha em Socket                �\n");
    printf("\t�                                                        �\n");
    printf("\t�                                                        �\n");
    printf("\t�                      +-----------+                     �\n");
    printf("\t�                      � %c � %c � %c �                     �\n", matriz_jogo[0], matriz_jogo[1], matriz_jogo[2]);
    printf("\t�                      �---+---+---�                     �\n");
    printf("\t�                      � %c � %c � %c �                     �\n", matriz_jogo[3], matriz_jogo[4], matriz_jogo[5]);
    printf("\t�                      �---+---+---�                     �\n");
    printf("\t�                      � %c � %c � %c �                     �\n", matriz_jogo[6], matriz_jogo[7], matriz_jogo[8]);
    printf("\t�                      +-----------+                     �\n");
    printf("\t�                                                        �\n");
    printf("\t�                                                        �\n");
    printf("\t�                                                        �\n");
    printf("\t+--------------------------------------------------------+\n");
    printf("\t Servidor: %s\n", mensagem_servidor);
    printf("\t Coordenada:");
}
