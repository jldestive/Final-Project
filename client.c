#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define IP "127.0.0.0"
#define PORT 5575
#define MAXLINE 1024

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];
    struct in_addr addr;

    if (argc > 2) {
        printf("La cantidad maxima de parametros es 2");
        exit(0);
    }

    if (argc == 2 && inet_pton(AF_INET, argv[1], &addr) <= 0) {
        printf("Dirección IP invalida.\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error en la creación del socket.\n");
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = (argc == 2)? inet_addr(argv[1]) : inet_addr(IP);
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Error en la conexión.\n");
        exit(0);
    }

    read(sockfd, buffer, sizeof(buffer));
    printf("Haiku recibido del servidor:\n%s\n", buffer);

    close(sockfd);
    return 0;
}
