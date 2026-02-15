#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int port;
    printf("Enter port: ");
    scanf("%d", &port);
    getchar();

    char msg[1024];
    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    msg[strcspn(msg, "\n")] = 0;

    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    send(s, msg, strlen(msg), 0);

    char buffer[1024] = {0};
    int valread = read(s, buffer, 1024);
    if (valread > 0) {
        if (strstr(buffer, "terminate session")) {
            printf("Server: %s\n", buffer);
        } else {
            printf("Response received.\n");
        }
    }

    close(s);
    return 0;
}
