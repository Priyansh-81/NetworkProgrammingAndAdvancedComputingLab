#include <ctype.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>

void toLower(char str[]){
    for(int i=0;str[i]!='\0';i++){
        str[i]=tolower((unsigned char)str[i]);
    }
}

bool isVowel(char a){
    if(a=='a'||a=='e'||a=='i'||a=='o'||a=='u'){
        return true;
    }
    return false;
}

bool isPallin(char str[]){
    int l=0;
    int r=strlen(str)-1;
    while(l<r){
        if(str[l]!=str[r]){
            return false;
        }
        l++;
        r--;
    }
    return true;
}
int numVowels(char str[]){
    int count=0;
    int i=0;
    while(str[i]!='\0'){
        if(isVowel(str[i])){
            count++;
        }
        i++;
    }
    return count;
}
int main(){
    int port_num;
    printf("Enter port number to connect to: ");
    scanf("%d",&port_num);
    struct sockaddr_in client, server;
    int s=socket(AF_INET,SOCK_DGRAM,0);
    if(s==-1){
        printf("\nSocket creation failed\n");
        return 0;
    }
    printf("\nSocket created\n");
    server.sin_addr.s_addr=htonl(INADDR_ANY);
    server.sin_port=htons(port_num);
    server.sin_family=AF_INET;

    int r=bind(s,(struct sockaddr*)&server,sizeof(server));
    if(r==-1){
        printf("\nSocket binding failed\n");
        close(s);
        return 0;
    }
    printf("Bind successful\n");

    printf("UDP: Ready to receive\n");
    while(1){
        char buff[50];
        socklen_t len,clen;
        clen=sizeof(client);
        int recvack=recvfrom(s,buff,sizeof(buff), 0, (struct sockaddr*)&client,&clen);
        if(recvack==-1){
            printf("\nFailed to recieve message from the client\nClosing the socket\n");
            close(s);
            return 0;
        }
        buff[recvack]='\0';

        toLower(buff);

        if(strcmp(buff,"halt")==0){
            printf("\nClosing connection\n");
            close(s);
            return 0;
        }

        printf("\nString to check: %s received\n",buff);
        char temp[100];
        if(isPallin(buff)){
            snprintf(temp, sizeof(temp),"Length: %zu\nVowel Count: %d\n",strlen(buff), numVowels(buff));
        }else{
            strcpy(temp,"\nNot a palindrome\n");
        }
        int sendack=sendto(s,temp, strlen(temp)+1,0,(struct sockaddr*)&client,clen);
        if(sendack==-1){
            printf("\nFailed\n");
            close(s);
            return 0;
        }
        printf("\nReply Sent\nServer is ready\n");
    }
    close(s);
    return 0;
}
