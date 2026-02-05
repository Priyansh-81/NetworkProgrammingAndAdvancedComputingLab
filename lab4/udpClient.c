// udp_client_file_db_loop.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

int main() {
    int sockfd, port;
    char buffer[BUFF_SIZE];
    struct sockaddr_in serv_addr;
    socklen_t len = sizeof(serv_addr);

    printf("Enter server port: ");
    scanf("%d", &port);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        int choice;
        char key_value[50];

        printf("\n----- MENU -----\n");
        printf("1. Search by Registration Number\n");
        printf("2. Search by Name\n");
        printf("3. Search by Subject Code\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 0) break;

        if (choice == 1) strcpy(buffer, "REG");
        else if (choice == 2) strcpy(buffer, "NAME");
        else if (choice == 3) strcpy(buffer, "SUB");
        else {
            printf("Invalid choice\n");
            continue;
        }

        printf("Enter value: ");
        scanf("%s", key_value);

        strcat(buffer, " ");
        strcat(buffer, key_value);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&serv_addr, len);

        memset(buffer, 0, BUFF_SIZE);
        recvfrom(sockfd, buffer, BUFF_SIZE, 0,
                 (struct sockaddr *)&serv_addr, &len);

        printf("\n--- Server Response ---\n%s\n", buffer);
    }

    close(sockfd);
    return 0;
}
