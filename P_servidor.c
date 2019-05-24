############ Servidor #########

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
 
typedef enum boolean{false, true}bool;
 
int Matriz    [3][3];
int Coordends_dig[9];
int jogada;
 
int  Error(char mensagem[], int returno);         
void StatusJogo(int, int, int, int);            
 
bool IniciaSocketServidor(int *);               
bool IniciaSocketClientes(int *, int *, int *); 

void Encapsular      (char *, int);             
void EncapsularMatriz(char pacote[]);           
void IntParaChar     (int, char char_gerado[]); 
int  CharParaInt     (char);                    
char GetCaracter     (int);                     
 
void Jogada_Cli(int *, int *, int *, int *, int *);
void ImprimeMatriz(void);
 
bool Valida_Coordenada      (int coordenada);            
bool VerifCoordJaDigitada  (int aux);                   
int  VerificaFimMatriz     (int jogadas);               
int  SomaCoordenadas       (int x, int y, int z);       
                                                        
bool Verifica_Soma_Coord     (int aux);                   
void ConverteEmCoordenadaXY(int coord, int *i, int *j);
 
int main(){
    int sock_programa, i, j, cont=0;
    int sock_cliente_1, sock_cliente_2;
 
    if(!IniciaSocketServidor(&sock_programa))
        return Error("Falha ao manipular a variavel sock_programa!", -1);
 
    if(!IniciaSocketClientes(&sock_programa, &sock_cliente_1, &sock_cliente_2))
        return Error("Falha no accept()", -1);
 
 
    int  qtd_jogadas  = 0;
    int  verifica_fim = 0;
    int  teste_erro   = 0;
    char pacote_envio[100];
 
    /*Começo do Jogo*/
    Encapsular(pacote_envio, 0);
    teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
    teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);
 
    Encapsular(pacote_envio, 5);/*sock_cliente faz a jogada*/
    teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
    do{
        // Ouve e realiza a jogada de sock_cliente
        Jogada_Cli(&sock_cliente_1, &sock_cliente_2, &qtd_jogadas, &teste_erro, &verifica_fim);
        StatusJogo(1, 2,qtd_jogadas, 0);
 
        // Ouve e realiza a jogada de sock_cliente
        if(!(verifica_fim==9 || verifica_fim==3 || verifica_fim==-3))
            Jogada_Cli(&sock_cliente_2, &sock_cliente_1, &qtd_jogadas, &teste_erro, &verifica_fim);
        StatusJogo(2, 1, qtd_jogadas, 0);
 
        if(verifica_fim==9 || verifica_fim==3 || verifica_fim==-3)
            break;
    }while(teste_erro!=-1);
 
    if(verifica_fim == 9){
        StatusJogo(2, 1, qtd_jogadas, 3);
        Encapsular(pacote_envio, 4);
        teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
        teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);
    }else if(verifica_fim == 3){
        StatusJogo(2, 1, qtd_jogadas, 2);
        Encapsular(pacote_envio, 2);
        teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);
        Encapsular(pacote_envio, 3);
        teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
    }else if(verifica_fim == -3){
        StatusJogo(2, 1, qtd_jogadas, 1);
        Encapsular(pacote_envio, 2);
        teste_erro = send(sock_cliente_1,pacote_envio,strlen(pacote_envio),0);
        Encapsular(pacote_envio, 3);
        teste_erro = send(sock_cliente_2,pacote_envio,strlen(pacote_envio),0);
    }
    system("sleep 2");
    close(sock_cliente_1);
    close(sock_cliente_2);
    close(sock_programa);
    return 0;
}
 
int Error(char mensagem[], int returno){
    printf("\n%s\n", mensagem);
    return returno;
}
 
void StatusJogo(int origem, int destino, int numero_da_jogada, int status){
    printf("\n+-----------------------------------------------+\n");
    printf("\n           ---STATUS DO JOGO---\n\n");
    if(status==0){
        printf("\n Realizou Jogada              >>  sock_cliente_%d", origem);
        printf("\n Aguardando                   >>  sock_cliente_%d", origem);
        printf("\n Proximo realizar uma Jogada  >>  sock_cliente_%d", destino);
    }else if(status==1){
        printf("\n GANHOU O JOGO                >>  sock_cliente_1");
        printf("\n PERDEU O JOGO                >>  sock_cliente_2");
        printf("\n FIM                          >>  FIM DE JOGO");
    }else if(status==1){
        printf("\n GANHOU O JOGO                >>  sock_cliente_1");
        printf("\n PERDEU O JOGO                >>  sock_cliente_2");
        printf("\n FIM                          >>  FIM DE JOGO");
    }else{
        printf("\n GANHOU O JOGO                >>  EMPATE");
        printf("\n PERDEU O JOGO                >>  EMPATE");
        printf("\n FIM                          >>  FIM DE JOGO");
    }
 
    printf("\n Numero da Jogada             >>  %d\n", numero_da_jogada);
    printf(" MATRIZ:\n");
    ImprimeMatriz();
    printf("\n+-----------------------------------------------+\n");
    printf("\n");
}
 
bool IniciaSocketServidor(int *sock_programa){
    struct sockaddr_in addr;
 
    *sock_programa = socket(AF_INET,SOCK_STREAM,0);
    if(*sock_programa == -1){
        printf("Erro ao criar o socket!\n");
        return false;
    }
 
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PORTA_PADRAO);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(&addr.sin_zero,0,sizeof(addr.sin_zero));
 
    if(bind(*sock_programa,(struct sockaddr*)&addr,sizeof(addr)) == -1){
        printf("Erro na funcao bind()\n");
        return false;
    }
 
    if(listen(*sock_programa,1) == -1){
        printf("Erro na funcao listen()\n");
        return false;
    }
 
    return true;
}
 
bool IniciaSocketClientes(int *sock_programa, int *sock_cliente_1, int *sock_cliente_2){
 
    printf("Aguardando clientes...\n");
    *sock_cliente_1 = accept(*sock_programa,0,0);
    printf("sock_cliente_1 - Cliente 1 aceito!\n");
 
    *sock_cliente_2 = accept(*sock_programa,0,0);
    printf("sock_cliente_2 - Cliente 2 aceito!\n");
 
    if(*sock_cliente_1 == -1 || *sock_cliente_2 == -1)
        return false;
    else
        return true;
}
 
void Encapsular(char *pacote, int id_mensagem){
    int  tam = 0;    
    char tam_char[4];
    int i;
    char mensagem_servidor[50];
 
    strcpy(pacote, "-");
 
    EncapsularMatriz(pacote);
 
    switch(id_mensagem){
        case 0:
            strcpy(mensagem_servidor, "Aguarde sua vez para jogar!");
        break;
        case 1:
            pacote[0] = 'x';
            strcpy(mensagem_servidor, "Coordenada Invalida Solicitada!");
        break;
        case 2:
            strcpy(mensagem_servidor, "FIM DE JOGO, VOCE GANHOU!!!");
        break;
        case 3:
            strcpy(mensagem_servidor, "FIM DE JOGO, VOCE PERDEU!!!");
        break;
        case 4:
            strcpy(mensagem_servidor, "FIM DE JOGO,DEU VELHA!!!");
        break;
        case 5:
            strcpy(mensagem_servidor, "Faça uma jogada!");
        break;
    }

    tam = strlen(mensagem_servidor);
    IntParaChar(tam, tam_char);
    strcat(pacote, tam_char);
 

    strcat(pacote, mensagem_servidor);
 
    // Preenche o restante do pacote com '-'
    for(i=1+9+tam+1;i<100; i++)
        strcat(pacote, "-");
 
    pacote[100] = '\0';
}
 
void EncapsularMatriz(char pacote[]){
    int i, j;
 
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            if(Matriz[i][j] == 0)
                strcat(pacote, "*");
            else if(Matriz[i][j] == 1)
                strcat(pacote, "O");
            else if(Matriz[i][j] == -1)
                strcat(pacote, "X");
        }
    }
}
 
void IntParaChar(int n, char char_gerado[]) {
    int  resto = 0;
    int  cont  = 0;
    char num[2];
 
    while(n > 0){
        resto = n%10;
        n = n/10;    
 
        num[cont] = (char)GetCaracter(resto);
        cont++;
    }
    char_gerado[0] = num[1];
    char_gerado[1] = num[0];
    char_gerado[2] = '\0';
}
 
int CharParaInt(char n){
    return (n)-48;
}
 
char GetCaracter(int a){
    int b = a+48;
    return (char)b;
}
 
void Jogada_Cli(int *sock_origem, int *sock_destino, int *qtd_jogadas, int *teste_erro, int *verifica_fim){
    int  coordenada, coord_x, coord_y;
    char pacote_receber[100];
    char pacote_envio[100];
    bool coordenada_valida = true;
 
    do{
        *teste_erro = recv(*sock_origem,pacote_receber,100,0);// Recebe pacote do cliente;
 
        if(*teste_erro!=-1 && !(*verifica_fim == 9 || *verifica_fim == -3 || *verifica_fim == 3)){
            coordenada = CharParaInt(pacote_receber[0]);
            coordenada_valida = Valida_Coordenada(coordenada);
            coordenada = coordenada - 1;
 
            if(coordenada_valida){
                ConverteEmCoordenadaXY(coordenada, &coord_x, &coord_y);
 
                if(*qtd_jogadas%2==0)
                    Matriz[coord_x][coord_y] = -1;
                else
                    Matriz[coord_x][coord_y] = 1;
 
                *qtd_jogadas = *qtd_jogadas + 1;
 
                *verifica_fim = VerificaFimMatriz(*qtd_jogadas);
                if(!(*verifica_fim == 9 || *verifica_fim == -3 || *verifica_fim == 3)){
                    
                    Encapsular(pacote_envio, 5);
                    *teste_erro = send(*sock_destino, pacote_envio, strlen(pacote_envio),0);
                }else{
                    coordenada_valida = true;
                }
 
                 Encapsular(pacote_envio, 0);
                *teste_erro = send(*sock_origem, pacote_envio, strlen(pacote_envio),0);
 
            }else{
                
                 Encapsular(pacote_envio, 1);
                *teste_erro = send(*sock_origem,pacote_envio,strlen(pacote_envio),0);  
            }
        }
        *verifica_fim = VerificaFimMatriz(*qtd_jogadas);
        
        if(*teste_erro == -1 || *verifica_fim == 9 || *verifica_fim == -3 || *verifica_fim == 3)
            coordenada_valida = true;
 
    }while(!coordenada_valida);
}
 
void ImprimeMatriz(void){
    int i, j;
 
    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            if(Matriz[i][j] == 1)
                printf(" O ");
            else if (Matriz[i][j] == -1)
                printf(" X ");
            else
                printf(" - ");
        }
        printf("\n");
    }
}
 
bool Valida_Coordenada(int coordenada){
    if(coordenada>9 || coordenada <1)
        return false;
    else if(VerifCoordJaDigitada(coordenada))
        return false;
 
    return true;
}
 
bool VerifCoordJaDigitada(int aux){
    int i;
 
    for(i=0; i<9; i++){
        if(Coordenadas_dig[i] == aux){
            return true;
        }
    }
    for(i=0; i<9; i++){
        if(Coordenadas_dig[i] == 0){
            Coordenadas_dig[i] = aux;
            return false;
        }
    }
}
 
int VerificaFimMatriz(int jogadas){
    int diagonal[2], vertical[3], horizontal[3], i;
 
    vertical  [0] = vertical  [1] = vertical  [2] = 0;
    horizontal[0] = horizontal[1] = horizontal[2] = 0;
    diagonal  [0] = diagonal  [1] = 0;
 
    horizontal[0] = SomaCoordenadas(0,1,2);
    horizontal[1] = SomaCoordenadas(3,4,5);
    horizontal[2] = SomaCoordenadas(6,7,8);
 
    vertical[0] = SomaCoordenadas(0,3,6);
    vertical[1] = SomaCoordenadas(1,4,7);
    vertical[2] = SomaCoordenadas(2,5,8);
 
    diagonal[0] = SomaCoordenadas(0,4,8);
    diagonal[1] = SomaCoordenadas(2,4,6);
 
 
    for(i=0; i<3; i++){
        if(Verifica_Soma_Coord(horizontal[i])){
            return horizontal[i];
        }
        if(Verifica_Soma_Coord(vertical[i])){
            return vertical[i];
        }
        if(i<2){
            if(Verifica_Soma_Coord(diagonal[i])){
                return diagonal[i];
            }
        }
    }
 
    if(jogadas == 9)
        return 9;
 
    return 0;
}
 
int SomaCoordenadas(int x, int y, int z){
    int i, j, cont=0;
 
    ConverteEmCoordenadaXY(x, &i, &j);
    cont = cont + Matriz[i][j];
    ConverteEmCoordenadaXY(y, &i, &j);
    cont = cont + Matriz[i][j];
    ConverteEmCoordenadaXY(z, &i, &j);
    cont = cont + Matriz[i][j];
 
    return cont;
}
 
bool Verifica_Soma_Coord(int aux){
    if(aux==3 || aux==-3){
        return true;
    }else{
        return false;
    }
}
 
void ConverteEmCoordenadaXY(int coord, int *i, int *j){
    *i = (coord/3);
    *j = (coord%3);
}
