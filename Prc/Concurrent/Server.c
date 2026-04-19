#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

void serviceClient(int c, struct sockaddr_in client);

int main(){
    int port;
    printf("ENter port number: ");
    scanf("%d",&port);
    struct sockaddr_in server,client;
    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        printf("Socket creation error!\n");
        return 0;
    }
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(port);
    server.sin_family=AF_INET;

    printf("Server created\n");
    if(bind(s,(struct sockaddr*)&server,sizeof(server))<0){
        printf("Bind Failed\n");
        close(s);
        return 0;
    }
    if(listen(s, 5)<0){
        printf("Server is not listening\n");
        close(s);
        return 0;
    }
    printf("Server is listening\n");
    socklen_t clen=sizeof(client);
    int clientCount=0;
    while(1){
        int c=accept(s, (struct sockaddr*)&client, &clen);
        int pid=fork();
        if(pid==-1){
            printf("Fork Error\n");
            close(c);
            continue;
        }
        clientCount++;
        if(clientCount>2){
            char* msg="Limit exceeded! terminate session\n";
            puts(msg);
            send(c,msg, strlen(msg),0);
            close(c);
        }
        if(pid==0){//child process
            close(s);
            serviceClient(c,client);
            close(c);
            return 0;
        }
    }
}

void serviceClient(int c, struct sockaddr_in client){
    FILE *fp=fopen("manipal.txt","a");
    printf("Serviced by %d",getpid());
    char buff[1024];
    char * addr=inet_ntoa(client.sin_addr);
    int portC=ntohs(client.sin_port);
    int r=recv(c,buff,sizeof(buff)-1, 0);
    if(r>0){
        buff[r]='\0';
        printf("[%s:%d]- %s\n",addr,portC,buff);
        buff[strcspn(buff, "\n")] = '\0';
        if(fp){
            fprintf(fp,"%s", buff);
            fclose(fp);
        }
        char content[1024];
        FILE *rfp=fopen("manipal.txt", "r");
        while(rfp && fgets(content, sizeof(content), rfp)){
            puts(content);
        }
    }
}
