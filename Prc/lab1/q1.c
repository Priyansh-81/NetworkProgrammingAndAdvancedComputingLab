#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

void asc_sort(int*, int);
void desc_sort(int*, int);
void send_split(int*, int);
int look_up(int*, int ,int);

int acp;

int main(){
    int port;
    printf("Enter the port number: ");
    scanf("%d",&port);

    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s==-1){
        printf("Socket creation error\n");
        return 0;
    }
    printf("Socket created successfully\n");
    struct sockaddr_in server, client;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_family=AF_INET;
    server.sin_port=htons(port);

    if(bind(s,(struct sockaddr*)&server,sizeof(server))==-1){
        printf("Bind failed\n");
        close(s);
        return 0;
    }
    if(listen(s,1)<0){
        printf("Server listen problem\n");
        close(s);
        return 0;
    }
    printf("Server is listening\n");
    socklen_t clen=sizeof(client);
    acp=accept(s,(struct sockaddr*)&client,&clen);
    if(acp<0){
        printf("Client can not be connected\n");
        close(s);
        return 0;
    }
    printf("Client connected\n");

    while(1){
        int choice;
        recv(acp,&choice,sizeof(choice),0);
        printf("Choice received: %d\n",choice);
        if(choice==5){
            printf("Bye bye client\n");
            close(acp);
            break;
        }
        printf("Receiving the input array size:\n");
        int n;
        recv(acp,&n,sizeof(n),0);
        int arr[n];
        printf("Array size recv: %d\n next receiving Array arr\n",n);
        recv(acp,arr,sizeof(int)*n,0);
        printf("Array received\n");
        for(int i=0;i<n;i++){
            printf("%d\t",arr[i]);
        }
        char buff[100]="";

        switch(choice){
            case 1:
                printf("Asc sort\n");
                asc_sort(arr,n);
                break;
            case 2:
                printf("Desc sort\n");
                desc_sort(arr,n);
                break;
            case 3:
                printf("Split\n");
                send_split(arr,n);
                break;
            case 4:
                printf("Look Up\n");
                int target;
                recv(acp,&target,sizeof(target),0);
                look_up(arr,n,target);
                break;
            default:
                strcpy(buff,"Invalid Choice\n");
                send(acp,buff,strlen(buff),0);
        }
    }
    close(acp);
    close(s);
    return 0;
}

void asc_sort(int *arr, int n){
    for(int i=0;i<n-1;i++){
        for(int j=i+1;j<n;j++){
            if(arr[i]>arr[j]){
                int temp=arr[i];
                arr[i]=arr[j];
                arr[j]=temp;
            }
        }
    }
    send(acp,arr,sizeof(int)*n,0);
}

void desc_sort(int *arr, int n){
    for(int i=0;i<n-1;i++){
        for(int j=i+1;j<n;j++){
            if(arr[i]<arr[j]){
                int temp=arr[i];
                arr[i]=arr[j];
                arr[j]=temp;
            }
        }
    }
    send(acp,arr,sizeof(int)*n,0);
}

void send_split(int *arr, int n){
    int even[n], odd[n];
    int e=0,o=0;

    for(int i=0;i<n;i++){
        if(arr[i]%2==0)
            even[e++]=arr[i];
        else
            odd[o++]=arr[i];
    }

    send(acp,&e,sizeof(e),0);
    send(acp,even,sizeof(int)*e,0);
    send(acp,&o,sizeof(o),0);
    send(acp,odd,sizeof(int)*o,0);
}

int look_up(int *arr, int n, int target){
    int found=0;
    for(int i=0;i<n;i++){
        if(arr[i]==target){
            found=1;
            break;
        }
    }
    send(acp,&found,sizeof(found),0);
    return found;
}
