#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

void handle_client(int s, struct sockaddr_in client);

int main() {
    int port;
    printf("Enter the port number: ");
    scanf("%d", &port);

    struct sockaddr_in server, client;
    socklen_t clen = sizeof(client);
    FILE *fp = fopen("manipal.txt", "w");
    if (fp) {
        fprintf(fp, "Manipal");
        fclose(fp);
    }

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        return 0;
    }

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    server.sin_family = AF_INET;

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind error");
        close(s);
        return 0;
    }

    if (listen(s, 8) < 0) {
        perror("Server is not listening");
        return 0;
    }
    printf("Server is listening on the port %d\n", port);

    int clientCount = 0;

    while (1) {
        int acp = accept(s, (struct sockaddr*)&client, &clen);
        if (acp < 0) continue;
        clientCount++;
        if (clientCount > 2) {
            printf("\nClient limit exceeded! Terminating sessions...\n");
            char *msg = "terminate session";
            send(acp, msg, strlen(msg), 0);
            close(acp);
            close(s);
            exit(0);
        }

        int childprocess = fork();
        if (childprocess == -1) {
            perror("FORK error");
            close(acp);
            continue;
        } else if (childprocess == 0) {
            close(s);
            handle_client(acp, client);
            close(acp);
            exit(0);
        }
        close(acp);
    }
    return 0;
}

void handle_client(int connfd, struct sockaddr_in cli_addr) {
    char buffer[1024] = {0};
    char cli_ip[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &cli_addr.sin_addr, cli_ip, INET_ADDRSTRLEN);
    int cli_port = ntohs(cli_addr.sin_port);

    int bytes_read = read(connfd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("\n[Client %s:%d] sent: %s\n", cli_ip, cli_port, buffer);
        FILE *fp = fopen("manipal.txt", "a");
        if (fp) {
            fprintf(fp, " %s", buffer);
            fclose(fp);
        }
        FILE *read_fp = fopen("manipal.txt", "r");
        char content[1024];
        if (read_fp && fgets(content, sizeof(content), read_fp)) {
            printf("Result: %s\n", content);
            fclose(read_fp);
        }
    }
}
