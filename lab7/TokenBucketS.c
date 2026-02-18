#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


int main() {
    int port;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    int bucketSize, tokenRate, tokens = 0;
    int packet;
    char response[100];

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));

    printf("Enter bucket size: ");
    scanf("%d", &bucketSize);

    printf("Enter token rate: ");
    scanf("%d", &tokenRate);

    printf("Token Bucket Server running...\n");

    while (1) {
        recvfrom(s, &packet, sizeof(packet), 0,
                 (struct sockaddr*)&client, &len);

        if (packet == -1)
            break;

        tokens += tokenRate;
        if (tokens > bucketSize)
            tokens = bucketSize;

        if (packet <= tokens) {
            tokens -= packet;
            sprintf(response, "Packet sent, tokens left=%d", tokens);
        } else {
            sprintf(response, "Packet dropped, not enough tokens");
        }

        sendto(s, response, strlen(response) + 1, 0,
               (struct sockaddr*)&client, len);
    }

    close(s);
    return 0;
}
