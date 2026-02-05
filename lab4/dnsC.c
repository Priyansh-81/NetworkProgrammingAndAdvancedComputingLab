#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

int main() {
    int port;
    printf("Enter port number: ");
    scanf("%d", &port);

    while (1) {
        int sock;
        struct sockaddr_in serv_addr;
        char buffer[BUFF_SIZE] = {0};
        char hostname[50];

        printf("Enter Hostname (e.g., www.google.com) (quit to exit): ");
        scanf("%49s", hostname);

        if (strcmp(hostname, "quit") == 0) {
            printf("Bye Bye \\n");
            break;
        }

        // Create socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            continue;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection Failed");
            close(sock);
            continue;
        }

        // Send hostname (include null terminator)
        send(sock, hostname, strlen(hostname) + 1, 0);

        // Receive response
        int bytes = recv(sock, buffer, BUFF_SIZE - 1, 0);
        if (bytes > 0) {
            printf("Resolved IP: %s\n", buffer);
        } else {
            printf("No response from server\n");
        }

        close(sock);
    }

    return 0;
}
