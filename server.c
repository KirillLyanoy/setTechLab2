#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void reaper (int sig) {
    int status;
    while (wait3(&status, WNOHANG, (struct rusage *) 0) >= 0);
}

int BufWork(int sockClient) {

    int data;
    int msgLength;

    

    while (1) {
        if( ( msgLength = recv( sockClient, &data, sizeof(int), 0 ) ) < 0 ){
            perror("Плохое получение дочерним процессом."); 
            return 1;
        }

        if ( msgLength == 0) {
            printf( "SERVER: Сокет %d закрыл соединение.\n\n", sockClient);
            break;
        }

        printf( "SERVER: Полученное число: %d\n\n", data);

        data = data * data;

        if (send (sockClient, &data, sizeof(data), 0) < 0) {
            printf("Ошибка передачи данных клиенту./n");           
        }

    }
}

int main() {
    int sockMain, sockClient, length, PID;
    struct sockaddr_in servAddr;

    if(( sockMain = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        perror("Сервер не может открыть главный socket."); return 1;
    }

    bzero( (char *) &servAddr, sizeof( servAddr ) );

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    servAddr.sin_port = 0;

    if ( bind( sockMain, (struct sockaddr *)&servAddr, sizeof(servAddr) ) ) {
        perror("Связывание сервера неудачно."); return 1;
    }

    length = sizeof( servAddr ) ;
    if ( getsockname( sockMain, &servAddr, &length ) ) {
        perror("Вызов getsockname неудачен."); return 1;
    }

    printf( "СЕРВЕР: номер порта - % d\n", ntohs(servAddr.sin_port ) );
    listen(sockMain, 5);

    signal(SIGCHLD, reaper);

    while(1) {

        sockClient = accept(sockMain, 0, 0);

        PID = fork();

        if (PID < 0) {
            printf("Ошибка при создании процесса.");
            return 1;
        }

        if (PID > 0) {
            printf("Создан процесс %d\n", PID);          
        }

        if (PID == 0) {
            BufWork(sockClient);
            close (sockClient);
            return 0;            
        } 

        close (sockClient);
    }
     
    return 0;    
}