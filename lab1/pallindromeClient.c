#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<strings.h>
#include<unistd.h>
#include<stdbool.h>
#include<arpa/inet.h>
int main(){
    int port_num;
    printf("Enter port number: ");
    scanf("%d",&port_num);
    struct sockaddr_in server;
    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s==-1){
        printf("\nSocket Creation failed\nClient End\n");
        return 0;
    }
    printf("\nSocket Created\n");
    server.sin_family=AF_INET;
    server.sin_port=htons(port_num);
    server.sin_addr.s_addr= inet_addr("127.0.0.1");

    int r=connect(s,(struct sockaddr *)&server,sizeof(server));

    if(r==-1){
        printf("\nConnection failed\n");
        return 0;
    }
    printf("\nConnected to server\n");
    while(1){
        printf("Enter the string to check for pallindrome: ");
        char buff[50];
        scanf("%s",buff);
        if(strcmp(buff,"halt")==0){
            printf("Disconnecting\n");
            close(s);
            return 0;
        }
        int sendack=send(s,buff,strlen(buff)+1,0);
        if(sendack==-1){
            printf("\nSending failed\n");
            close(s);
            return 0;
        }
        printf("\nWaiting for reply\n");
        char temp[50];
        int recack=recv(s,temp,sizeof(temp),0);
        if(recack==-1){
            printf("\nMessage reply failed\n");
            close(s);
            return 0;
        }
        temp[recack]='\0';
        printf("%s\n",temp);
    }
    close(s);
    return 0;

}
