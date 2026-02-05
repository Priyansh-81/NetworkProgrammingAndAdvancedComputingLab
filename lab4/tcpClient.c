#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

int main() {
    int sock = 0, choice, port;
    printf("Enter port number: ");
    scanf("%d",&port);
    struct sockaddr_in serv_addr;
    char buffer[BUFF_SIZE], input[BUFF_SIZE];

    while (1) {
        printf("\n--- STUDENT INFO SYSTEM ---\n");
        printf("1. Registration Number (Get Name & Address)\n");
        printf("2. Name of Student (Get Enrollment Details)\n");
        printf("3. Subject Code (Get Marks)\n");
        printf("4. Exit\n");
        printf("Enter Choice: ");
        scanf("%d", &choice);
        
        // Clear input buffer (eats the newline)
        getchar();

        if (choice == 4) {
            printf("Exiting Client...\n");
            break;
        }

        if (choice < 1 || choice > 4) {
            printf("Invalid Option. Try again.\n");
            continue;
        }

        printf("Enter Search Query: ");
        fgets(input, BUFF_SIZE, stdin);
        input[strcspn(input, "\n")] = 0; // Remove newline character

        // --- Connection Setup (Must be done per request) ---
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Connection Failed");
            close(sock);
            continue;
        }

        // --- Send Data ---
        send(sock, &choice, sizeof(choice), 0);
        send(sock, input, strlen(input), 0);

        // --- Receive Response ---
        memset(buffer, 0, BUFF_SIZE);
        recv(sock, buffer, BUFF_SIZE, 0);
        printf("\n--- Server Response ---\n%s\n", buffer);

        // --- Close Connection for this request ---
        close(sock);
    }
    return 0;
}