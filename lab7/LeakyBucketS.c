#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>


int main() {
    int port;
    printf("Enter port: ");
    scanf("%d",&port);
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    int bucketSize, outputRate, bucket = 0;
    int packet;
    char response[100];

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));

    printf("Enter bucket size: ");
    scanf("%d", &bucketSize);

    printf("Enter output rate: ");
    scanf("%d", &outputRate);

    printf("Leaky Bucket Server running...\n");

    while (1) {
        recvfrom(s, &packet, sizeof(packet), 0,
                 (struct sockaddr*)&client, &len);

        if (packet == -1)
            break;

        if (bucket + packet > bucketSize) {
            sprintf(response, "Packet dropped");
        } else {
            bucket += packet;
            sprintf(response, "Packet accepted, bucket=%d", bucket);
        }

        bucket -= outputRate;
        if (bucket < 0) bucket = 0;

        sendto(s, response, strlen(response) + 1, 0,
               (struct sockaddr*)&client, len);
    }

    close(s);
    return 0;
}
