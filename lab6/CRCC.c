#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int port;
    printf("Enter port:");
    scanf("%d",&port);
    struct sockaddr_in server;
    char data[100], gen[20], result[100];

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter data: ");
    scanf("%s", data);

    printf("Enter generator: ");
    scanf("%s", gen);

    sendto(s, data, strlen(data) + 1, 0,
           (struct sockaddr*)&server, sizeof(server));

    sendto(s, gen, strlen(gen) + 1, 0,
           (struct sockaddr*)&server, sizeof(server));

    recvfrom(s, result, sizeof(result), 0, NULL, NULL);
    printf("Server: %s\n", result);

    close(s);
    return 0;
}
