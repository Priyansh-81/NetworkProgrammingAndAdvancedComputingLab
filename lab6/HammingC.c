#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>


int main() {
    int port;
    printf("Enter port: ");
    scanf("%d",&port);
    struct sockaddr_in server;
    int data[30], total;

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter total bits: ");
    scanf("%d", &total);

    printf("Enter code bits:\n");
    for (int i = 1; i <= total; i++)
        scanf("%d", &data[i]);

    sendto(s, &total, sizeof(total), 0,
           (struct sockaddr*)&server, sizeof(server));

    sendto(s, data, sizeof(int) * total, 0,
           (struct sockaddr*)&server, sizeof(server));

    recvfrom(s, data, sizeof(int) * total, 0, NULL, NULL);

    printf("Corrected code:\n");
    for (int i = 1; i <= total; i++)
        printf("%d ", data[i]);

    printf("\n");

    close(s);
    return 0;
}
