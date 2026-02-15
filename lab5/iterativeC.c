#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int s;
    struct sockaddr_in server;
    int port;

    printf("Enter port: ");
    scanf("%d", &port);
    while (getchar() != '\n');

    char str1[512], str2[512], buffer[512];

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); exit(1); }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        close(s);
        exit(1);
    }
    printf("Enter first string: ");
    fgets(str1, sizeof(str1), stdin);
    str1[strcspn(str1, "\n")] = 0;
    printf("Enter second string: ");
    fgets(str2, sizeof(str2), stdin);
    str2[strcspn(str2, "\n")] = 0; 
    char msg[1024];
    sprintf(msg, "%s\n%s\n", str1, str2);
    write(s, msg, strlen(msg));

    int n = read(s, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server response: %s\n", buffer);
    }

    close(s);
    return 0;
}