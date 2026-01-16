#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main(){
    int port;
    printf("Enter port number: ");
    scanf("%d",&port);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s==-1){
        printf("Socket failed\n");
        return 0;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int r=connect(s, (struct sockaddr*)&server, sizeof(server));
    printf("Connected to server\n");
    if(r==-1){
        printf("\nCould not connect\n");
        close(s);
        return 0;
    }
    printf("\nConnection successfull\n");

    while(1){
        int choice,n,arr[50],key;

        printf("\n1.Search\n2.Sort Asc\n3.Sort Desc\n4.Split Odd/Even\n5.Exit\n");
        printf("Enter choice: ");
        scanf("%d",&choice);

        send(s, &choice, sizeof(choice), 0);
        if(choice==5) break;

        printf("Enter number of elements: ");
        scanf("%d",&n);
        send(s, &n, sizeof(n), 0);

        printf("Enter elements:\n");
        for(int i=0;i<n;i++)
            scanf("%d",&arr[i]);

        send(s, arr, sizeof(int)*n, 0);

        if(choice==1){
            printf("Enter number to search: ");
            scanf("%d",&key);
            send(s, &key, sizeof(key), 0);
        }

        char reply[200];
        recv(s, reply, sizeof(reply), 0);
        printf("\nServer Reply:\n%s\n", reply);
    }

    close(s);
    return 0;
}
