#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


int main() {
    int port;
    printf("Enter port:");
    scanf("%d",&port);
    struct sockaddr_in server;
    int packet;
    char response[100];

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        printf("Enter packet size (-1 to stop): ");
        scanf("%d", &packet);

        sendto(s, &packet, sizeof(packet), 0,
               (struct sockaddr*)&server, sizeof(server));

        if (packet == -1)
            break;

        recvfrom(s, response, sizeof(response), 0, NULL, NULL);
        printf("Server: %s\n", response);
    }

    close(s);
    return 0;
}
