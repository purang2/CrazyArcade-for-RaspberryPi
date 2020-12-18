#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>


/* RPI_CLIENT .C */


int main()
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	//char ch = 'A';
	char ch;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(9734);
	len = sizeof(address);

	result = connect(sockfd, (struct sockaddr *)&address, len);

	if(result == -1){
		perror("oops: client3");
		exit(1);
	}
	

	while(1){	
		printf("input: ");
		scanf("%c\n", &ch);


		write(sockfd, &ch, 1);	
		read(sockfd, &ch, 1);
	
	
		printf("char from server = %c\n", ch);
	
		if(ch == 'Q' || ch== 'q'){
			printf("quit by q input\n");
			close(sockfd);
			exit(0);
		}
		else {
			printf("not quit \n");
		}
	}
}


