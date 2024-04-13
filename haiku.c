#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <bits/waitflags.h>

#define PORT 5575
#define MAXLINE 1024

void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void generate_haiku(char *buffer) {
    char haikus[9][MAXLINE] = {
        "Brilla sol dorado,\nen el cielo despejado,\ndía ha comenzado.",
        "Hojas caen lentas,\nen el bosque silencioso,\notoño se siente.",
        "Río fluye libre,\nbajo el puente de piedra,\nva hacia el mar grande.",
        "Nieve en la montaña,\ncubre todo con su manto,\ninviero está aquí.",
        "Vuela mariposa,\nentre flores y colores,\ndanza de la vida.",
        "Llueve en el jardín,\ngotas bailan en las hojas,\nmúsica del cielo.",
        "Luna en el cielo,\nguía de los navegantes,\nfaro en la noche.",
        "Montaña nevada,\nen su cima el sol se esconde,\ndía se despide.",
        "Un viejo estanque\nSalta una rana, el sonido\nDel agua es profundo."
    };

    srand(time(NULL)); // Inicializa el generador de números aleatorios
    int randomIndex = rand() % 9; 
    strcpy(buffer, haikus[randomIndex]);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    pid_t childpid;
    char buffer[MAXLINE];
    char clientAddr[MAXLINE];
    int fd[2];

    signal(SIGCHLD, sigchld_handler); // manejar señal SIGCHLD
    signal(SIGINT, SIG_IGN); // ignorar señal SIGINT

    if (pipe(fd) == -1) {
        fprintf(stderr, "Error al crear la tuberia.\n");
        return 1;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Error en la creacion del socket.\n");
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Error en el bind del socket.\n");
        exit(0);
    }

    if ((listen(sockfd, 5)) != 0) {
        printf("Error en el listen.\n");
        exit(0);
    }

    len = sizeof(cliaddr);

    while(1) {
        connfd = accept(sockfd, (struct sockaddr*)&cliaddr, &len);
        if (connfd < 0) {
            printf("Error en el accept.\n");
            exit(0);
        }

        if ((childpid = fork()) == 0) {
            close(sockfd);

            generate_haiku(buffer);
            write(connfd, buffer, sizeof(buffer));

            close(fd[0]); // cerrar el extremo de lectura de la tubería en el proceso hijo
            write(fd[1], inet_ntoa(cliaddr.sin_addr), strlen(inet_ntoa(cliaddr.sin_addr)) + 1);
            close(fd[1]);

            exit(0);
        }
        else {
            close(connfd);
            close(fd[1]); // cerrar el extremo de escritura de la tubería en el proceso padre
            read(fd[0], clientAddr, MAXLINE);
            printf("Dirección IP del cliente: %s\n", clientAddr);
            close(fd[0]);
        }
    }

    close(sockfd);
    return 0;
}
