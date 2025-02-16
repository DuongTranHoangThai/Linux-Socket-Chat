/*
 ============================================================================
 Name        : Socket_chat.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12346
#define BUFFER_SIZE 1024

int Server(void);
int Client(const char* server_ip, int port);

int main(int argc, char *argv[]) {
	if(argc > 1)
	{
		if(argc == 2 && strcmp(argv[1], "-server") == 0)
		{
			Server();
		}
		else if(argc == 4 && strcmp(argv[1], "-client") == 0)
		{
			Client(argv[2],atoi(argv[3]));
		}
		else
		{
			printf(" Use ./Socket_chat -server or ./Socket_chat -client <server_ip> <port>\n");
		}
	}
	else
	{
		printf(" Use ./Socket_chat -server or ./Socket_chat -client <server_ip> <port>\n");
	}
}

int Server(void){
	//	For TCP protocol Server socket() -> bind() -> listen() -> accept() -> read(),write() -> close()
		printf("TCP Server\n");
		int socket_fd;
		struct sockaddr_in  socket_addr, client_addr;
		socklen_t client_addr_length = sizeof(client_addr);
		char buffer[BUFFER_SIZE];

		//Config the socket address struct
		 socket_addr.sin_family = AF_INET;
		 socket_addr.sin_addr.s_addr = INADDR_ANY;  //Accept any IP for client
		 socket_addr.sin_port = htons(PORT);

		//Create new socket with TCP protocol
		socket_fd = socket(AF_INET,SOCK_STREAM,0);

		// Enable SO_REUSEADDR option
		int opt = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		    perror("setsockopt(SO_REUSEADDR) failed");
		    close(socket_fd);
		    return -1;
		}

		//Bind the socket
		if(bind(socket_fd,(struct sockaddr*)&socket_addr,sizeof(socket_addr)) < 0)
		{
			printf("Bind error \n"); return -1;
		}

		//Listen to max 5 connection
		listen(socket_fd, 5);
		printf("Waiting for a connection \n");

		//Accept
		int new_socket = accept(socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_length);
		if(new_socket < 0)
		{
			printf("Accept connection error \n");	return -1;
		}

		// Print client information
		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
		int client_port = ntohs(client_addr.sin_port);
		printf("Connection established with client: IP = %s, Port = %d\n", client_ip, client_port);

		//Send hello to the client
		bzero(buffer, BUFFER_SIZE);
		strcpy(buffer,"Hello client \n");
		if(write(new_socket,buffer, strlen(buffer)) < 0)
		{
			printf("write error \n");
		}

		while(1)
		{

			//Listen to the client
			bzero(buffer,BUFFER_SIZE);

			int n = read(new_socket, buffer, BUFFER_SIZE);
			if(n < 0) //Read failed
			{
				printf("Read from client fail \n");
				break;
			}
			if(n == 0) //Client request disconnected
			{
				printf("Client disconnected ! \n");
				break;
			}


			printf("Client: %s \n",buffer);

			//Client request disconnection
			if(strncmp("Bye",buffer,3) == 0)
				break;

			//Read input
			bzero(buffer,BUFFER_SIZE);
			printf("Type:");
			//int c; while ((c = getchar()) != '\n' && c != EOF); // Empty the stdin buffer
			fgets(buffer, BUFFER_SIZE, stdin);

			//Send response
			if(write(new_socket, buffer, strlen(buffer)) < 0)
			{
				printf("Send error !!!\n");
			}
			printf("Please wait response \n");

		}

		close(new_socket);
		close(socket_fd);

		return EXIT_SUCCESS;
}

int Client(const char* server_ip, int port)
{
	char buffer[BUFFER_SIZE];

	//For TCP protocol Client socket() ->connect() -> read(),write() -> close()
	printf("TCP Client\n");
	int sock_fd;
	sock_fd = socket(AF_INET,SOCK_STREAM,0);

	//Configure the socket address struct
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	printf("Setup the connection to connect with server IP:%s\n",server_ip);

	//Connect to server
	if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
	{
		printf ("Connect error"); return -1;
	}

	printf("Connected to server at %s\n", server_ip);

	while(1)
	{
		//Listen to the client
		bzero(buffer,BUFFER_SIZE);
		int n = read(sock_fd, buffer, BUFFER_SIZE);
		if(n < 0) //Read failed
		{
			printf("Read from Server fail \n");
			break;
		}
		if(n == 0) //Client request disconnected
		{
			printf("Server disconnected ! \n");
			break;
		}
		printf("Server: %s \n",buffer);

		//Get input
		printf("Type:");
		bzero(buffer,BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin); //TODO: if stdin is contain the newline(which is typed by user while program is blocking in read() )) this fget will imidiatealy return. Same issue for Clinet code

		//Response
		if(write(sock_fd, buffer, strlen(buffer)) < 0)
		{
			printf("Send error !!!\n");
		}
		printf("Please wait response \n");



	}

	close(sock_fd);

	return EXIT_SUCCESS;
}
