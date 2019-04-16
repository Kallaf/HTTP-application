#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <thread>

using namespace std;


void split(char str[1024],char splited[100][50])
{
    int i = 0; 
    char *ptr = strtok(str," ");
    while(ptr != NULL)
    {
        strcpy(splited[i], ptr);
        ptr = strtok(NULL, " ");
        i++;
    }
}

int isFileExists(const char *path)
{
    if (access(path, F_OK) == -1)
        return 0;

    return 1;
}

void* handle_client_connection(int client_socket)
{
    char request[1024] = {0};

    read( client_socket , request, 1024);
    printf("%s\n",request );

    char recieved_str[100][50];
    split(request,recieved_str);
    char header[50] = "";
    char page[50] = "";

    if(!strcmp(recieved_str[0],"GET"))
    {
        
        if(!strcmp(recieved_str[1],"/"))
            strcpy(page,"index.html");
        else{
            for(int i=1;i<=strlen(recieved_str[1]);i++)
                page[i-1] = recieved_str[1][i];
        }

        if (isFileExists(page))
        {
            strcpy(header,"HTTP/1.0 200 OK\r\n\r\n");
            write(client_socket , header , strlen(header));
            printf("Header sent: %ssending: %s\n\n",header,page);
            FILE *fp;
            fp = fopen(page, "r");
            char buffer[1024];
            do{
                int nb = fread(buffer, 1, 1024, fp);
                write(client_socket, buffer, nb);
            }while(!feof(fp));
            fclose(fp);
			printf("file sent\n");

        }
        else
            {
                strcpy(header,"HTTP/1.0 404 Not Found\r\n\r\n");
                write(client_socket , header , strlen(header));
                printf("Header sent: %s\n",header);
            
            }
    }else if(!strcmp(recieved_str[0],"POST"))
    {
		strcpy(header,"HTTP/1.0 200 OK\r\n\r\n");
		write(client_socket , header , strlen(header));
		for(int i=1;i<=strlen(recieved_str[1]);i++)
                page[i-1] = recieved_str[1][i];

        
		printf("Header sent: %srecieving: %s\n\n",header,page);
   		char path[50] = "RecievedFiles/";
   		strcat(path,page);
        FILE *fp;
        fp = fopen(path, "w");
   		char buffer[1024];
	 	while(1)
        {
			int length = read(client_socket, buffer, 1024);
			fwrite(buffer, 1,length, fp);
			if(length < 1024)
				break;
			
        }
        fclose(fp);
        printf("file recieved\n");
        
    }

    
    
    close(client_socket);
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    int server_fd,bind_port = atoi(argv[1]);
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( bind_port );
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 20) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        int client_socket;
        printf("\nListening on 127.0.0.1:%d\n\n",bind_port);
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        thread client_handler(handle_client_connection,client_socket);
        client_handler.join();
    }
    return 0;
}
