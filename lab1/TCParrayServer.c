#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void sortAsc(int a[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=0;j<n-i-1;j++)
            if(a[j] > a[j+1]){
                int t=a[j]; a[j]=a[j+1]; a[j+1]=t;
            }
}

void sortDesc(int a[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=0;j<n-i-1;j++)
            if(a[j] < a[j+1]){
                int t=a[j]; a[j]=a[j+1]; a[j+1]=t;
            }
}

int main(){
    int port;
    printf("Enter port number: ");
    scanf("%d",&port);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s==-1){
        printf("\nSocket failed\n");
        return 0;
    }
    printf("\nSocket created sucessfully\n");

    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int r=bind(s, (struct sockaddr*)&server, sizeof(server));
    listen(s, 5);
    if(r==-1){
        printf("\nBind failed\n");
        close(s);
        return 0;
    }
    printf("\nBind Successfull\n");

    socklen_t len = sizeof(client);
    int ns = accept(s, (struct sockaddr*)&client, &len);
    if(ns==-1){
        printf("\nAccept failed\n");
        return 0;
    }

    printf("\nClient connected\n");

    while(1){
        int choice, n, arr[50], key;

        recv(ns, &choice, sizeof(choice), 0);
        if(choice == 5){
            printf("Client exited\n");
            break;
        }

        recv(ns, &n, sizeof(n), 0);
        recv(ns, arr, sizeof(int)*n, 0);

        char result[200] = "";

        if(choice == 1){
            recv(ns, &key, sizeof(key), 0);
            printf("\ngot it 1\n");
            int found = 0;
            for(int i=0;i<n;i++)
                if(arr[i]==key) found=1;

            if(found) strcpy(result, "Number Found\n");
            else strcpy(result, "Number Not Found\n");
        }

        else if(choice == 2){
            sortAsc(arr,n);
            printf("\ngot it 2\n");
            strcat(result, "Ascending Order: ");
            for(int i=0;i<n;i++){
                char t[10];
                sprintf(t,"%d ",arr[i]);
                strcat(result,t);
            }
        }

        else if(choice == 3){
            sortDesc(arr,n);
            printf("\ngot it 3\n");
            strcat(result, "Descending Order: ");
            for(int i=0;i<n;i++){
                char t[10];
                sprintf(t,"%d ",arr[i]);
                strcat(result,t);
            }
        }

        else if(choice == 4){
            strcat(result,"Even: ");
            printf("\ngot it 4\n");
            for(int i=0;i<n;i++)
                if(arr[i]%2==0){
                    char t[10]; sprintf(t,"%d ",arr[i]);
                    strcat(result,t);
                }

            strcat(result,"\nOdd: ");
            for(int i=0;i<n;i++)
                if(arr[i]%2!=0){
                    char t[10]; sprintf(t,"%d ",arr[i]);
                    strcat(result,t);
                }
        }

        send(ns, result, strlen(result)+1, 0);
    }

    close(ns);
    close(s);
    return 0;
}
