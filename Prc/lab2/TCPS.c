#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

static char *global_file_content_buffer = NULL;

int compareStrings_by_offset(const void *a, const void *b) {
    size_t offset1 = *(const size_t *)a;
    size_t offset2 = *(const size_t *)b;
    return strcmp(global_file_content_buffer + offset1, global_file_content_buffer + offset2);
}

int countSub(char* filename, char* target){
    int count = 0;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file for reading in countSub");
        return -1;
    }
    char line[1024];
    while(fgets(line, sizeof(line), fp) != NULL){
        if(strstr(line, target)){
            count++;
        }
    }
    fclose(fp);
    return count;
}

void replace(char *filename, char* s1, char* s2){
    FILE *fp_read, *fp_write;
    char temp_filename[256];
    char line[1024];
    char output_line[4096];
    char *token;
    char *rest_of_line;

    snprintf(temp_filename, sizeof(temp_filename), "%s.tmp", filename);

    fp_read = fopen(filename, "r");
    if (fp_read == NULL) {
        perror("Error opening file for reading in replace");
        return;
    }

    fp_write = fopen(temp_filename, "w");
    if (fp_write == NULL) {
        perror("Error creating temporary file in replace");
        fclose(fp_read);
        return;
    }

    while (fgets(line, sizeof(line), fp_read) != NULL) {
        output_line[0] = '\0';
        rest_of_line = line;

        while ((token = strstr(rest_of_line, s1)) != NULL) {
            strncat(output_line, rest_of_line, token - rest_of_line);
            strcat(output_line, s2);
            rest_of_line = token + strlen(s1);
            if (strlen(output_line) + strlen(rest_of_line) + strlen(s2) >= sizeof(output_line) - 1) {
                fprintf(stderr, "Warning: Output buffer overflow risk in replace function for line. Skipping further replacements on this line.\n");
                break;
            }
        }
        strcat(output_line, rest_of_line);
        fputs(output_line, fp_write);
    }

    fclose(fp_read);
    fclose(fp_write);

    remove(filename);
    rename(temp_filename, filename);
}

void rearrange(char * filename){
    FILE *fp_read, *fp_write;
    char temp_filename[256];
    char line_buffer[1024];
    long file_size = 0;
    int num_lines = 0; //
    size_t *line_offsets = NULL;

    snprintf(temp_filename, sizeof(temp_filename), "%s.tmp", filename);

    fp_read = fopen(filename, "r");
    if (fp_read == NULL) {
        perror("Error opening file for reading (1st pass) in rearrange");
        return;
    }
    fseek(fp_read, 0, SEEK_END);
    file_size = ftell(fp_read);
    fseek(fp_read, 0, SEEK_SET);

    while (fgets(line_buffer, sizeof(line_buffer), fp_read) != NULL) {
        num_lines++;
    }
    fclose(fp_read);

    if (file_size == 0 || num_lines == 0) {
        fp_write = fopen(temp_filename, "w");
        if (fp_write) fclose(fp_write);
        remove(filename);
        rename(temp_filename, filename);
        return;
    }
    global_file_content_buffer = (char *)malloc(file_size + num_lines + 1);
    if (global_file_content_buffer == NULL) {
        perror("Memory allocation failed for global_file_content_buffer in rearrange");
        return;
    }
    global_file_content_buffer[0] = '\0';

    line_offsets = (size_t *)malloc(sizeof(size_t) * num_lines);
    if (line_offsets == NULL) {
        perror("Memory allocation failed for line_offsets array in rearrange");
        free(global_file_content_buffer);
        global_file_content_buffer = NULL;
        return;
    }

    fp_read = fopen(filename, "r");
    if (fp_read == NULL) {
        perror("Error opening file for reading (2nd pass) in rearrange");
        free(global_file_content_buffer);
        global_file_content_buffer = NULL;
        free(line_offsets);
        return;
    }

    int current_line_idx = 0;
    size_t current_buffer_pos = 0;
    while (fgets(line_buffer, sizeof(line_buffer), fp_read) != NULL) {
        line_offsets[current_line_idx] = current_buffer_pos;
        size_t len = strlen(line_buffer);

        if (len > 0 && line_buffer[len - 1] == '\n') {
            line_buffer[len - 1] = '\0';
            len--;
        } else {
            line_buffer[len] = '\0';
        }

        strcpy(global_file_content_buffer + current_buffer_pos, line_buffer);
        current_buffer_pos += (len + 1);
        current_line_idx++;
    }
    fclose(fp_read);

    qsort(line_offsets, num_lines, sizeof(size_t), compareStrings_by_offset);

    fp_write = fopen(temp_filename, "w");
    if (fp_write == NULL) {
        perror("Error creating temporary file in rearrange (write)");
        free(global_file_content_buffer);
        global_file_content_buffer = NULL;
        free(line_offsets);
        return;
    }

    for (int i = 0; i < num_lines; i++) {
        fputs(global_file_content_buffer + line_offsets[i], fp_write);
        fputc('\n', fp_write);
    }

    fclose(fp_write);

    free(global_file_content_buffer);
    global_file_content_buffer = NULL;
    free(line_offsets);

    remove(filename);
    rename(temp_filename, filename);
}


int main(){
    printf("Enter port number: ");
    int port;
    scanf("%d",&port);

    struct sockaddr_in server, client;
    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        printf("Socket error\n");
        return 0;
    }

    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(port);
    server.sin_family=AF_INET;

    if(bind(s,(struct sockaddr*)&server,sizeof(server))<0){
        printf("Bind Error\n");
        close(s);
        return 0;
    }

    if(listen(s,1)<0){
        printf("Server is not listening\n");
        close(s);
        return 0;
    }
    printf("Server is listening\n");

    socklen_t clen=sizeof(client);
    int a=accept(s,(struct sockaddr*)&client,&clen);
    if(a<0){
        printf("Client connection error\n");
        close(s);
        return 0;
    }

    char filename[1024];
    int n=recv(a,filename,sizeof(filename)-1,0);
    if (n < 0) {
        perror("Error receiving filename");
        close(a);
        close(s);
        return 0;
    }
    filename[n]='\0';
    filename[strcspn(filename, "\n")] = 0;

    FILE *f_check = fopen(filename,"r");
    if(f_check==NULL){
        send(a,"File not found\n",strlen("File not found\n"),0);
        close(a);
        close(s);
        return 0;
    }
    fclose(f_check);
    send(a,"File found\n",strlen("File found\n"),0);

    int server_should_run = 1;

    while(server_should_run){
        int opt;
        int r_opt = recv(a,&opt,sizeof(opt),0);
        if (r_opt <= 0) {
            printf("Client disconnected or error receiving option. Terminating...\n");
            server_should_run = 0;
        } else {
            printf("Option received: %d\n", opt);

            char buff[1024];
            int r_data;
            char s1[1024];
            char s2[1024];
            int count;

            switch(opt){
                case 1:
                    send(a,"Target to search for: ",strlen("Target to search for: "),0);
                    r_data=recv(a,buff,sizeof(buff)-1,0);
                    if (r_data <= 0) { server_should_run = 0; break; }
                    buff[r_data]='\0';
                    buff[strcspn(buff, "\n")] = 0;
                    count=countSub(filename,buff);
                    send(a,&count,sizeof(count),0);
                    break;
                case 2:
                    send(a,"Element to replace: ",strlen("Element to replace: "),0);
                    r_data=recv(a,s1, sizeof(s1)-1,0);
                    if (r_data <= 0) { server_should_run = 0; break; }
                    s1[r_data]='\0';
                    s1[strcspn(s1, "\n")] = 0;

                    send(a,"The replacement: ",strlen("The replacement: "),0);
                    r_data=recv(a,s2,sizeof(s2)-1,0);
                    if (r_data <= 0) { server_should_run = 0; break; }
                    s2[r_data]='\0';
                    s2[strcspn(s2, "\n")] = 0;

                    replace(filename,s1, s2);
                    send(a,"Operation complete\n",strlen("Operation complete\n"),0);
                    break;
                case 3:
                    rearrange(filename);
                    send(a,"Operation complete\n",strlen("Operation complete\n"),0);
                    break;
                case 4:
                    printf("Terminating...\n");
                    send(a,"Terminating...\n",strlen("Terminating...\n"),0);
                    server_should_run = 0;
                    break;
                default:
                    send(a,"Invalid Choice\n",strlen("Invalid Choice\n"),0);
            }
        }
    }

    close(a);
    close(s);
    return 0;
}
