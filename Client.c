#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444
const int NUM_INTERFACE = 5;

struct control{
	int id;
	char interface[50], alias[50], filter[50], rule[50], mode[50];
};

void show(struct control list[]){
	printf("Interface Alias Filter Rule Mode\n");
	for(int i = 1; i < NUM_INTERFACE; i++){
		printf("%s\t", list[i].interface);
		printf("%s\t", list[i].alias);
		printf("%s\t", list[i].filter);
		printf("%s\t", list[i].rule);
		printf("%s\t", list[i].mode);
		printf("\n");
	}
}


int main(){

	int clientSocket, ret;
	struct sockaddr_in serverAddr;
	char buffer[1024];

	struct control list_control[5];
	
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");
	for(int i = 1; i < NUM_INTERFACE; i++){
		recv(clientSocket, list_control[i].interface, 50, 0);
		recv(clientSocket, list_control[i].alias, 50, 0);
		recv(clientSocket, list_control[i].filter, 50, 0);
		recv(clientSocket, list_control[i].rule, 50, 0);
		recv(clientSocket, list_control[i].mode, 50, 0);
	}
	show(list_control);
	while(1){
		printf("Client: \t");
		scanf("%s", &buffer[0]);
		send(clientSocket, buffer, 1024, 0);

		if(strcmp(buffer, ":exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}
		if(strcmp(buffer, "get") == 0){
			scanf("%s", &buffer[0]);
			send(clientSocket, buffer, 50, 0);
			scanf("%s", &buffer[0]);
			send(clientSocket, buffer, 50, 0);
			char status[50] = "False!";
			recv(clientSocket, status, 50, 0);
			printf("%s\n", status);
			for(int i = 1; i < NUM_INTERFACE; i++){
				recv(clientSocket, list_control[i].interface, 50, 0);
				recv(clientSocket, list_control[i].alias, 50, 0);
				recv(clientSocket, list_control[i].filter, 50, 0);
				recv(clientSocket, list_control[i].rule, 50, 0);
				recv(clientSocket, list_control[i].mode, 50, 0);
			}
			show(list_control);
			printf("Client get: %s\n", status);
		}
		else if(strcmp(buffer, "update") == 0){
			scanf("%s", &buffer[0]);
			send(clientSocket, buffer, 50, 0);
			scanf("%s", &buffer[0]);
			send(clientSocket, buffer, 50, 0);
			char status[50] = "False!";
			recv(clientSocket, status, 50, 0);
			printf("Server update: %s\n", status);
			if(strcmp(status, "Premiss Denied!") == 0) continue;
			for (int i = 0; i < 5; ++i)
			{
				recv(clientSocket, buffer, 1024, 0);
				printf("%s", buffer);
				scanf("%s", &buffer[0]);
				send(clientSocket, buffer, strlen(buffer), 0);
			}
		}

		else if(strcmp(buffer, "delete") == 0){
			scanf("%s", &buffer[0]);
			send(clientSocket, buffer, strlen(buffer), 0);
			char status[50] = "False!";
			recv(clientSocket, status, 50, 0);
			printf("Server delete: %s\n", status);
		}
		else{
			recv(clientSocket, buffer, 1024, 0);
			printf("Server:\t%s\n", buffer);
		}
	}

	return 0;
}
