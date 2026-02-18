#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int port;
    printf("Enter port: ");
    scanf("%d",&port);
    struct sockaddr_in server, client;
    char data[1024];
    socklen_t len = sizeof(client);

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));

    printf("Parity Server running...\n");

    while (1) {
        recvfrom(s, data, sizeof(data), 0,
                 (struct sockaddr*)&client, &len);

        int count = 0;
        for (int i = 0; data[i]; i++) {
            if (data[i] == '1') count++;
        }

        if (count % 2 == 0)
            strcpy(data, "No error (Even parity)");
        else
            strcpy(data, "Error detected");

        sendto(s, data, strlen(data) + 1, 0,
               (struct sockaddr*)&client, len);
    }

    close(s);
    return 0;
}
