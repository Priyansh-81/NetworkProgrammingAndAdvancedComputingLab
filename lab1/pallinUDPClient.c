#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<strings.h>
#include<unistd.h>

int main(){
    int port_num;
    printf("Enter port number: ");
    scanf("%d",&port_num);
    struct sockaddr_in client, server;
    int s=socket(AF_INET,SOCK_DGRAM,0);
    if(s==-1){
        printf("\nSocket creation failed\n");
        return 0;
    }
    printf("\nSocket created successfully\n");

    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    server.sin_port=htons(port_num);
    server.sin_family=AF_INET;

    socklen_t len;
    len=sizeof(server);
    while(1){
        char buff[50];
        char reply[100];

        printf("\nEnter string: ");
        scanf("%s",buff);

        int sendack = sendto(s, buff, strlen(buff)+1, 0,(struct sockaddr*)&server, len);

        if(strncmp(buff, "halt", 4) == 0){
            close(s);
            return 0;
        }
        if(sendack == -1){
            printf("\nSend failed\n");
            close(s);
            return 0;
        }

        int recvack = recvfrom(s, reply, sizeof(reply), 0,(struct sockaddr*)&server, &len);

        if(recvack == -1){
            printf("\nReceive failed\n");
            close(s);
            return 0;
        }

        reply[recvack] = '\0';
        printf("\nServer reply:\n%s\n", reply);
    }
    close(s);
    return 0;
}
