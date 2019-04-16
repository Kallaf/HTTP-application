#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

using namespace std;

void split(char str[1024],char splited[15][20])
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

int server_port;
string server_ip;

void get(string fileName)
{
	struct sockaddr_in address; 
	int sock = 0; 
	struct sockaddr_in serv_addr; 
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return; 
	} 
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(server_port); 
	if(inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr)<=0) 
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return; 
	} 
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return; 
	}

	string message = "GET /"+fileName+" HTTP/1.1\r\nHost: "+server_ip+":"+fileName+"\r\n\r\n";
	send(sock , message.c_str() , message.length() , 0 );

	char responds[1024] = {0}; 
	read( sock , responds, 1024);
	printf("server responds: %s\nrecieving: %s\n\n",responds,fileName.c_str());
	char recieved_str[15][20];
	split(responds,recieved_str);
	if(!strcmp(recieved_str[1],"200"))
	{
		FILE *fp;
		char path[50] = "Recieved_Client_Files/";
   		strcat(path,fileName.c_str());
        fp = fopen(path, "w");
   		char buffer[1024];
	 	while(1)
        {
			int length = read(sock, buffer, 1024);
			if(!length)
				break;
			fwrite(buffer, 1,length, fp);
        }
        fclose(fp);
        printf("file recieved\n");
	} 


}



int isFileExists(const char *path)
{
    if (access(path, F_OK) == -1)
        return 0;

    return 1;
}

void post(string fileName)
{
	if(isFileExists(fileName.c_str()))
	{
		struct sockaddr_in address; 
		int sock = 0; 
		struct sockaddr_in serv_addr; 
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		{ 
			printf("\n Socket creation error \n"); 
			return; 
		} 
		memset(&serv_addr, '0', sizeof(serv_addr)); 
		serv_addr.sin_family = AF_INET; 
		serv_addr.sin_port = htons(server_port); 
		if(inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr)<=0) 
		{ 
			printf("\nInvalid address/ Address not supported \n"); 
			return; 
		} 
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
		{ 
			printf("\nConnection Failed \n"); 
			return; 
		}

		string message = "POST /"+fileName+" HTTP/1.1\r\nHost: "+server_ip+":"+fileName+"\r\n\r\n";
		send(sock , message.c_str() , message.length() , 0 );

		char responds[1024] = {0}; 
		read( sock , responds, 1024);
		printf("server responds: %s\nsending: %s\n\n",responds,fileName.c_str());

		char recieved_str[15][20];
		split(responds,recieved_str);
		if(!strcmp(recieved_str[1],"200"))
		{
			FILE *fp;
            fp = fopen(fileName.c_str(), "r");
            char buffer[1024];
            do{
                int nb = fread(buffer, 1, 1024, fp);
                write(sock, buffer, nb);
            }while(!feof(fp));
            fclose(fp);
        	printf("file sent\n");
		}


	}else
		cout << "no such file or directory\n";
}

int main(int argc, char const *argv[]) 
{ 
	server_port = atoi(argv[2]);
	server_ip = argv[1];
	while(1)
	{
		cout << "\n1)POST\n2)GET\n3)Exit\nSelect Operation: ";
		int x;
		string fileName;
		cin >> x;
		switch(x)
		{
			case 1:
				cout << "input file name : ";
				cin >> fileName;
				post(fileName);
				break;
			case 2:
				cout << "input file name : ";
				cin >> fileName;
				get(fileName);
				break;
			default:
				return 0;
		}
	}

	return 0; 
} 
