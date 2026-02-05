#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFF_SIZE 1024

struct DNS_Record {
    char hostname[50];
    char ip[20];
};

struct DNS_Record db[] = {
    {"www.google.com", "142.250.190.46"},
    {"www.youtube.com", "208.65.153.238"},
    {"www.facebook.com", "157.240.22.35"},
    {"localhost", "127.0.0.1"}
};

int db_count = 4;

/* Remove trailing newline */
void trim_newline(char *str) {
    str[strcspn(str, "\n")] = 0;
}

int main() {
    int port;
    printf("Enter port number: ");
    scanf("%d", &port);

    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server listening on port %d...\n", port);

    while (1) {
        int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        char buffer[BUFF_SIZE] = {0};
        int bytes = recv(new_socket, buffer, BUFF_SIZE - 1, 0);

        if (bytes <= 0) {
            close(new_socket);
            continue;
        }

        trim_newline(buffer);

        if (strcmp(buffer, "quit") == 0) {
            printf("Server shutting down...\n");
            close(new_socket);
            break;
        }

        printf("Client requested IP for: %s\n", buffer);

        char response[BUFF_SIZE] = "DNS Error: Hostname not found";

        for (int i = 0; i < db_count; i++) {
            if (strcmp(db[i].hostname, buffer) == 0) {
                strcpy(response, db[i].ip);
                break;
            }
        }

        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
