#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define BUFF_SIZE 1024

// Database Structure
struct Student {
    char reg_no[20];
    char name[50];
    char address[100];
    char dept[10];
    int semester;
    char section[5];
    char courses[50];
    char subject_code[10];
    int marks;
};

// Mock Database
struct Student db[] = {
    {"101", "Alice", "123 Main St", "CSE", 4, "A", "OS, CN", "CS401", 85},
    {"102", "Bob", "456 Park Ave", "ECE", 4, "B", "Signals, Analog", "EC402", 78},
    {"103", "Charlie", "789 Road", "IT", 4, "A", "Java, Web", "IT403", 90}
};
int db_size = 3;

int main() {
    int port;
    printf("Enter port number: ");
    scanf("%d",&port);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1, choice;
    char buffer[BUFF_SIZE], input[BUFF_SIZE], response[BUFF_SIZE];
    pid_t pid;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
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

    printf("Student Info Server listening on port %d...\n", port);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        // Parent receives Option and Input data first
        recv(new_socket, &choice, sizeof(choice), 0);
        recv(new_socket, input, sizeof(input), 0);

        pid = fork();

        if (pid == 0) { // Child Process
            close(server_fd); // Child doesn't need listener
            memset(response, 0, BUFF_SIZE);
            int found = 0;

            if (choice == 1) { // Registration No -> Name, Address
                for(int i=0; i<db_size; i++) {
                    if(strcmp(db[i].reg_no, input) == 0) {
                        sprintf(response, "PID: %d\nName: %s\nAddress: %s", getpid(), db[i].name, db[i].address);
                        found = 1;
                        break;
                    }
                }
            } 
            else if (choice == 2) { // Name -> Enrollment Details
                for(int i=0; i<db_size; i++) {
                    if(strcmp(db[i].name, input) == 0) {
                        sprintf(response, "PID: %d\nDept: %s, Sem: %d, Sec: %s\nCourses: %s", 
                                getpid(), db[i].dept, db[i].semester, db[i].section, db[i].courses);
                        found = 1;
                        break;
                    }
                }
            } 
            else if (choice == 3) { // Subject Code -> Marks
                // Note: In real DB, subject code needs to be linked to a student. 
                // Here assuming we search just for the code present in the struct.
                for(int i=0; i<db_size; i++) {
                    if(strcmp(db[i].subject_code, input) == 0) {
                        sprintf(response, "PID: %d\nSubject: %s\nMarks Obtained: %d", 
                                getpid(), db[i].subject_code, db[i].marks);
                        found = 1;
                        break;
                    }
                }
            }

            if (!found) strcpy(response, "Record not found.");
            
            send(new_socket, response, strlen(response), 0);
            close(new_socket);
            exit(0);
        } else {
            // Parent Process
            close(new_socket); // Parent closes connected socket
        }
    }
    return 0;
}