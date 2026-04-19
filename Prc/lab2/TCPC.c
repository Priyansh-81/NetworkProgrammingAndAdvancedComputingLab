#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(){
    int port;
    printf("Enter port: ");
    scanf("%d",&port);
    clear_input_buffer();

    struct sockaddr_in server;
    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        printf("Socket creation error\n");
        return 0;
    }

    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    server.sin_port=htons(port);
    server.sin_family=AF_INET;

    if(connect(s, (struct sockaddr*)&server, sizeof(server))<0){
        printf("Connection failed\n");
        close(s);
        return 0;
    }
    printf("Connected to server...\n");

    char filename[50];
    char server_response[1024];
    int r;

    printf("Enter filename: ");
    fgets(filename,sizeof(filename),stdin);
    filename[strcspn(filename, "\n")] = 0;
    send(s,filename,strlen(filename),0);

    r = recv(s, server_response, sizeof(server_response)-1, 0);
    if (r <= 0) {
        printf("Server disconnected or error receiving file status.\n");
        close(s);
        return 0;
    }
    server_response[r] = '\0';
    printf("%s", server_response);

    if (strstr(server_response, "File not found") != NULL) {
        close(s);
        return 0;
    }

    while(1){
        printf("\n--- Menu ---\n");
        printf("1. Count substring occurrences\n");
        printf("2. Replace substring\n");
        printf("3. Rearrange file (sort lines)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");

        int opt;
        scanf("%d",&opt);
        clear_input_buffer();

        send(s,&opt,sizeof(opt),0);

        if (opt == 4) {
            r = recv(s, server_response, sizeof(server_response)-1, 0);
            if (r > 0) {
                server_response[r] = '\0';
                printf("%s\n", server_response);
            }
            break;
        }

        char buffer[1024];
        int count;

        switch(opt){
            case 1:
                r = recv(s, buffer, sizeof(buffer)-1, 0);
                if (r <= 0) {
                    printf("Server disconnected.\n");
                    close(s);
                    return 0;
                }
                buffer[r] = '\0';
                printf("%s", buffer);

                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                send(s, buffer, strlen(buffer), 0);

                r = recv(s, &count, sizeof(count), 0);
                if (r <= 0) {
                    printf("Server disconnected.\n");
                    close(s);
                    return 0;
                }
                printf("Substring count: %d\n", count);
                break;
            case 2:
                r = recv(s, buffer, sizeof(buffer)-1, 0);
                if (r <= 0) {
                    printf("Server disconnected.\n");
                    close(s);
                    return 0;
                }
                buffer[r] = '\0';
                printf("%s", buffer);

                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                send(s, buffer, strlen(buffer), 0);

                r = recv(s, server_response, sizeof(server_response)-1, 0);
                if (r <= 0) {
                    printf("Server disconnected.\n");
                    close(s);
                    return 0;
                }
                server_response[r] = '\0';
                printf("%s", server_response);

                fgets(buffer, sizeof(buffer), stdin);
                buffer[strcspn(buffer, "\n")] = 0;
                send(s, buffer, strlen(buffer), 0);

                r = recv(s, server_response, sizeof(server_response)-1, 0);
                if (r <= 0) {
                    printf("Server disconnected.\n");
                    close(s);
                    return 0;
                }
                server_response[r] = '\0';
                printf("%s\n", server_response);
                break;
            case 3:
                r = recv(s, server_response, sizeof(server_response)-1, 0);
                if (r <= 0) {
                    printf("Server disconnected.\n");
                    close(s);
                    return 0;
                }
                server_response[r] = '\0';
                printf("%s\n", server_response);
                break;
            default:
                r = recv(s, server_response, sizeof(server_response)-1, 0);
                if (r <= 0) {
                    printf("Server disconnected.\n");
                    close(s);
                    return 0;
                }
                server_response[r] = '\0';
                printf("%s\n", server_response);
                break;
        }
    }
}
