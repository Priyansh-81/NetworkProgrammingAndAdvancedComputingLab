#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>

bool isPalindrome(char str[]){
    int lenght=strlen(str);
    int l=0;
    int r=lenght-1;
    while(l<r){
        if(str[l]!=str[r]){
            return false;
        }
        l++;
        r--;
    }
    return true;
}

int main(){
    int port_num;
    char buff[50];
    struct sockaddr_in client,server;
    socklen_t len;
    printf("Enter port number: ");
    scanf("%d",&port_num);
    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s==-1){
        printf("\nError Creating socket\nServer End\n");
        return 0;
    }
    printf("\nSocket Created\nServer End\n");
    server.sin_family=AF_INET;
    server.sin_port=htons(port_num);//we use htons to conver little_endian of intel systems to big endian to networks
    server.sin_addr.s_addr=htonl(INADDR_ANY);//htonl to convert little to big, and INADDR_ANY so that server can accept request from any client

    int r= bind(s,(struct sockaddr*)&server,sizeof(server));
    if(r==-1){
        printf("\nBinding sockets failed\n");
        return 0;
    }
    printf("\nSocket binded\n");

    int t=listen(s, 1);//server passive open, 1 indicates server will serve to one client
    if(t==-1){
        close(s);
        return 0;
    }
    printf("\nSocket listening\n");
    len=sizeof(client);
    int acp=accept(s,(struct sockaddr*)&client,&len);
    if(acp==-1){
        printf("\nFailed to accept\n");
        close(s);
        return 0;
    }
    printf("\nClient socket accepted\n");
    while(1){
        int recack=recv(acp,buff, sizeof(buff)-1,0);
        if(recack==-1){
            printf("Interruption in receiving the message\n");
            close(s);
            close(acp);
            return 0;
        }
        buff[recack]='\0';
        if(strcmp(buff,"halt")==0){
            printf("Bye bye\n");
            close(acp);
            close(s);
            return 0;
        }
        printf("Pallindrome string received\nString is: %s\n",buff);
        char temp[50];
        if(isPalindrome(buff)){
            strcpy(temp,"The string is a pallindrome\n");
        }else{
            strcpy(temp,"The string is not a pallindrome\n");
        }
        int sendack=send(acp,temp, strlen(temp)+1,0);
        if(sendack==-1){
            printf("\nMessage delivery failed\n");
            close(s);
            close(acp);
            return 0;
        }
    }

    close(acp);
    close(s);
    return 0;

}
