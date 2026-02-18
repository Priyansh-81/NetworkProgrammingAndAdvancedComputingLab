#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>


int main() {
    int port;
    printf("Enter port: ");
    scanf("%d",&port);
    struct sockaddr_in server, client;
    int data[30], total;
    socklen_t len = sizeof(client);

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));

    printf("Hamming Server running...\n");

    recvfrom(s, &total, sizeof(total), 0,
             (struct sockaddr*)&client, &len);

    recvfrom(s, data, sizeof(int) * total, 0,
             (struct sockaddr*)&client, &len);

    int error = 0;

    for (int i = 1; i <= total; i *= 2) {
        int parity = 0;
        for (int j = 1; j <= total; j++) {
            if (j & i)
                parity ^= data[j];
        }
        if (parity)
            error += i;
    }

    if (error != 0) {
        data[error] ^= 1;
        printf("Error at position: %d corrected\n", error);
    } else {
        printf("No error\n");
    }

    sendto(s, data, sizeof(int) * total, 0,
           (struct sockaddr*)&client, len);

    close(s);
    return 0;
}
