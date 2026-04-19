#include<cstudio.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

typedef struct Domain_ipmap{
    char domain[220];
    char ipaddr[220];
}map;

int db_size=5;
map db[] = {
    {"www.google.com", "142.250.190.46"},
    {"www.youtube.com", "208.65.153.238"},
    {"www.facebook.com", "157.240.22.35"},
    {"localhost", "127.0.0.1"},
    {"www.x.com","162.159.140.229"}
};

void handleclient(int acp, struct sockaddr_in client){
    char buff[1024];
    char response[1024]="Domain not found in db\n";
    int r=recv(acp,buff,sizeof(buff),0);
    buff[r]='\0';
    for(int i=0;i<5;i++){
        if(strcmp(buff,db[i].domain)==0){
            sprintf(response,"%d: %s\n",getpid(),db[i].ipaddr);
            break;
        }
    }
    send(acp, response, strlen(response),0);

}

int main(){
    int port;
    printf("Enter port: ");
    scanf("%d",&port);
    struct sockaddr_in server, client;
    int s=socket(AF_INET, SOCK_STREAM, 0);
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(port);
    server.sin_family=AF_INET;
    if(s<0){
        return 0;
    }
    if(bind(s,(struct sockaddr*)&server,sizeof(server))<0){
        printf("bind error\n");
        close(s);
        return 0;
    }
    listen(s, 8);
    int clientCount=0;

    while(1){
        socklen_t clen=sizeof(client);
        int acp=accept(s, (struct sockaddr *)&client, &clen);
        printf("Client Connected\n");
        int pid=fork();
        if(pid<0){
            printf("Fork error\n");
        }
        clientCount++;
        if(clientCount>7){
            printf("Limit Exceeded\n");
            send(acp,"Limit Exceeded...",strlen("Limit Exceeded..."),0);
            close(acp);
            close(s);
            break;
        }
        if(pid==0){
            //child process
            close(s);
            handleclient(acp,client);
            close(acp);
            return 0;
        }
    }
    return 0;
}
