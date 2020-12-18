#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include <time.h>
#include <termio.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>


#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

//대문자 wasd 
#define LEFT2 65
#define RIGHT2 68
#define UP2 87
#define DOWN2 83 

#define BOMB1 32 //space
#define BOMB2 9 //Tab



/* RPI_CLIENT .C */



int getch(void);

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



int getch(void){
	int ch;
	
	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag &= ~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);

	ch = getchar();

	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}
