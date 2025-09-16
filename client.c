#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
int sock;
    struct sockaddr_in servAddr;
    struct hostent *hp, *gethostbyname();

    char servIp[16];
    int servPort;
    printf("Введите IP сервера: \n");
    if (scanf("%15s", servIp) != 1)
    {
        printf("Ошибка при вводе  IP.");
        return 1;
    }
    while (getchar() != '\n');

    printf("Введите порт: \n");
    if (scanf("%d", &servPort) != 1)
    {
        printf("Ошибка при вводе порта.");
        return 1;
    }
    while (getchar() != '\n');

    printf("IP: %s\tPORT: %d\n", servIp, servPort);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf("Ошибка соединения сокета.");
        return 1;
    }

    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(servPort);

    if (inet_pton(AF_INET, servIp, &servAddr.sin_addr) <= 0)
    {
        printf("Ошибка при конвертировании IP.");
        return 1;
    }
  
    if( connect( sock, (struct sockaddr *)&servAddr, sizeof( servAddr ) ) < 0 ){
        perror("Клиент не может соединиться.\n"); 
        return 1;
    }

    printf ("CLIENT: Готов к пересылке\n") ;

    for (int i = 0; i < 20; i++) {

        if( send ( sock, &i, sizeof(i), 0 ) < 0 ) {
            perror( "Проблемы с пересылкой.\n" ); 
            return 1;
        }
        printf("Число %d отправлено на сервер.\n", i);

        sleep(2);
    }

    close(sock); 
    return 0;

}