#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


void xor_op(char *a, char *b, char *result, int len) {
    for (int i = 1; i < len; i++)
        result[i - 1] = (a[i] == b[i]) ? '0' : '1';
}

int crc_check(char *data, char *gen) {
    int len = strlen(gen);
    char temp[100], result[100];

    strncpy(temp, data, len);

    for (int i = len; i <= strlen(data); i++) {
        if (temp[0] == '1')
            xor_op(temp, gen, result, len);
        else {
            for (int j = 1; j < len; j++)
                result[j - 1] = temp[j];
        }
        result[len - 1] = data[i];
        strncpy(temp, result, len);
    }

    for (int i = 0; i < len - 1; i++)
        if (temp[i] != '0')
            return 0;  // error

    return 1; // no error
}

int main() {
    int port;
    printf("Enter port: ");
    scanf("%d",&port);
    struct sockaddr_in server, client;
    char data[100], gen[20], result[100];
    socklen_t len = sizeof(client);

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr*)&server, sizeof(server));

    printf("CRC Server running...\n");

    recvfrom(s, data, sizeof(data), 0,
             (struct sockaddr*)&client, &len);
    recvfrom(s, gen, sizeof(gen), 0,
             (struct sockaddr*)&client, &len);

    if (crc_check(data, gen))
        strcpy(result, "No error detected");
    else
        strcpy(result, "Error detected");

    sendto(s, result, strlen(result) + 1, 0,
           (struct sockaddr*)&client, len);

    close(s);
    return 0;
}
