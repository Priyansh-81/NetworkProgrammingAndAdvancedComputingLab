#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

int is_anagram(char *s1, char *s2) {
    int count[256] = {0};
    if (strlen(s1) != strlen(s2)) return 0;
    for (int i = 0; s1[i]; i++) {
        count[(unsigned char)s1[i]]++;
        count[(unsigned char)s2[i]]--;
    }
    for (int i = 0; i < 256; i++)
        if (count[i] != 0) return 0;
    return 1;
}

int read_line(int sock, char *buf, int size) {
    int i = 0;
    char ch;
    while (i < size - 1) {
        if (read(sock, &ch, 1) <= 0) break;
        if (ch == '\n') break;
        buf[i++] = ch;
    }
    buf[i] = '\0';
    return i;
}

int main() {
    int port;
    printf("Enter port: ");
    scanf("%d", &port);
    while (getchar() != '\n'); 

    int s = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server, client;
    socklen_t clen = sizeof(client);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);
    printf("Iterative Server running on port %d...\n", port);

    while (1) {
        int acp = accept(s, (struct sockaddr *)&client, &clen);
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str)-1] = '\0';
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
        printf("[%s] Connected to %s:%d\n", time_str, ip, ntohs(client.sin_port));
        char str1[512], str2[512];
        read_line(acp, str1, sizeof(str1));
        read_line(acp, str2, sizeof(str2));
        if (is_anagram(str1, str2))
            write(acp, "YES: Anagrams", 14);
        else
            write(acp, "NO: Not Anagrams", 17);
        close(acp);
    }
    return 0;
}