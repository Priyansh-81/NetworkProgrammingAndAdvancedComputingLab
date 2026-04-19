#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>

void handleClient(struct sockaddr_in client,int ack){
    char buffer[1024],response[1024],input[1024];
    int opt;
    int r=recv(ack,&opt,sizeof(opt),0);
    if(r<0){
        return;
    }
    r=recv(ack,buffer,sizeof(buffer),0);
    if(r<0){
        return;
    }
    buffer[r]='\0';

    if(opt==4){
        send(ack,"Bye Bye!",strlen("Bye Bye!"),0);
        close(ack);
        return;
    }

    switch(opt){
        case 1:
            searchbyname();
            break;
        case 2:
            searchbyreg();
            break;
        case 3:
            searchby();
            break;
        default:
            send(ack,"Invalid Choice",strlen("Invalid Choice"),0);
    }
}

int main(){
    int port;
    printf("Enter port number: ");
    scanf("%d",&port);
    int s=socket(AF_INET, SOCK_STREAM, 0);
    if(s<0){
        printf("Socket Error!\n");
        return 0;
    }
    struct sockaddr_in server, client;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(port);
    server.sin_family=AF_INET;

    if(bind(s,(struct sockaddr*)&server,sizeof(server))<0){
        printf("Bind error\n");
        close(s);
        return 0;
    }

    if(listen(s,8)<0){
        printf("Server aint listening lil bro!\n");
        close(s);
        return 0;
    }
    printf("Server is listening...\n");

    socklen_t clen=sizeof(client);
    int count=0;
    while(1){
        int ack=accept(s,(struct sockaddr*)&client,&clen);
        if(ack<0){
            printf("Client not connected\n");
            continue;
        }
        int pid= fork();
        if(pid<0){
            printf("Fork Error\n");
            close(ack);
        }
        count++;
        if(count>5){
            printf("Server Overloaded\nTerminating new connections\n");
            send(ack,"Server Overloaded, please try again later",sizeof("Server Overloaded, please try again later"),0);
            close(ack);
            continue;
        }
        if(pid==0){
            //child process
            close(s);//child does not need to listen
            handleClient(client,ack);
            close(ack);
            break;
        }
    }
    return 0;
}
