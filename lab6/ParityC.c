#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int port;
    printf("Enter port: ");
    scanf("%d",&port);

    struct sockaddr_in server;
    char data[1024];

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter binary data: ");
    scanf("%s", data);

    sendto(s, data, strlen(data) + 1, 0,
           (struct sockaddr*)&server, sizeof(server));

    recvfrom(s, data, sizeof(data), 0, NULL, NULL);
    printf("Server: %s\n", data);

    close(s);
    return 0;
}
