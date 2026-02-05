// udp_server_file_db_fixed.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_SIZE 2048  // bigger buffer for full record

// Search the file by key (REG, NAME, SUB) and return full student block
void search_db(const char *key_type, const char *key_value, char *result) {
    FILE *fp = fopen("db.txt", "r");
    if (!fp) {
        strcpy(result, "Database file not found\n");
        return;
    }

    char line[200];
    int found = 0;
    char buffer[BUFF_SIZE] = {0};

    // First, find if record exists
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, key_type, strlen(key_type)) == 0) {
            char *value = line + strlen(key_type);
            value[strcspn(value, "\n")] = 0; // remove newline
            if (strcmp(value, key_value) == 0) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        strcpy(result, "Record not found\n");
        fclose(fp);
        return;
    }

    // Rewind and copy the full student block
    fseek(fp, 0, SEEK_SET);
    int copy = 0;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0; // remove newline
        if (strncmp(line, "---", 3) == 0 && copy) break; // end of block
        if (copy || (strncmp(line, key_type, strlen(key_type)) == 0 &&
                     strcmp(line + strlen(key_type), key_value) == 0)) {
            copy = 1;
            strcat(buffer, line);
            strcat(buffer, "\n");
        }
    }

    strcpy(result, buffer);
    fclose(fp);
}

int main() {
    int port;
    printf("Enter server port: ");
    scanf("%d", &port);

    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len = sizeof(cli_addr);
    char buffer[BUFF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("UDP File-based DB Server running on port %d...\n", port);

    while (1) {
        memset(buffer, 0, BUFF_SIZE);
        recvfrom(sockfd, buffer, BUFF_SIZE, 0,
                 (struct sockaddr *)&cli_addr, &len);

        pid_t pid = fork();
        if (pid == 0) { // Child process
            char response[BUFF_SIZE] = {0};
            pid_t cpid = getpid();

            // Parse option and full value (handles spaces)
            char option[10];
            char *value;
            value = strchr(buffer, ' ');
            if (value != NULL) {
                *value = '\0'; // terminate option
                value++;       // move to start of actual value
            } else {
                value = "";
            }
            strcpy(option, buffer);

            // Search DB
            if (strcmp(option, "REG") == 0) {
                search_db("REG:", value, response);
            }
            else if (strcmp(option, "NAME") == 0) {
                search_db("NAME:", value, response);
            }
            else if (strcmp(option, "SUB") == 0) {
                search_db("SUB:", value, response);
            }
            else {
                strcpy(response, "Invalid option\n");
            }

            // Prepend PID
            char final_resp[BUFF_SIZE];
            sprintf(final_resp, "PID: %d\n%s", cpid, response);

            sendto(sockfd, final_resp, strlen(final_resp), 0,
                   (struct sockaddr *)&cli_addr, len);

            exit(0); // child exits
        }
    }

    close(sockfd);
    return 0;
}
