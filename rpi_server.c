#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>


/* RPI-server.c */

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	unlink("server socket");
	server_sockfd = socket(AF_INET, SOCK_STREAM , 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address,server_len);

	listen(server_sockfd, 15);
	while(1){
		listen(server_sockfd, 15);
		char ch;

		printf("server waiting\n");


		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

		read(client_sockfd, &ch, 1);
		//ch++;
		/*Logic */
		if(ch == 'A' || ch =='S'|| ch=='D' || ch =='W'){
			ch = 'X';
			printf("TRUE KEY\n");
			write(client_sockfd, &ch , 1);
	
		}
		
		else{
			write(client_sockfd, &ch, 1);
		}
		
		if(ch == 'Q' || ch == 'q'){
			printf("close by q\n");
		       	close(client_sockfd);
		}
	}
}

