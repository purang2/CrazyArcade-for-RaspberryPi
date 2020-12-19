#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

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


/* RPI64_CLIENT .C */

int p1x;         /* player 1's location */
int p1y;
int p2x;	/* player 2's location */
int p2y;
bool bomb1;
bool bomb2;

int b1x;         /* Bombs' location */
int b2x;
int b1y;
int b2y;

bool end;         /* Game-over flag (Does game over?)*/

//char map[8][8];          /* LED Matrix 2D array */
//unsigned short *map;
unsigned shor map[8][8];
void move(int m);
int getch(void);
void die(int a);
//void key_process(int key);

/* Multi Threading : Bomb function Bomb1-Bomb2 concurrent */

void *firstbombThreadRun(){
	if(bomb1 == false){
		bomb1 = true;
                b1x = p1x;
                b1y = p1y;
               	//map[b1y][b1x] = 'B';
                map[b1y][b1x] = 3; /* BOMB : 3*/
		sleep(2);
		die(1);		
       }

}

void *secondbombThreadRun(){
	if(bomb2 == false){
                 bomb2 = true;
                 b2x = p2x;
                 b2y = p2y;
                 //map[b2y][b2x] = 'B';
                 map[b1y][b1x] =3; 
		 sleep(2);
                 die(2);
       }

}





/* RPI_CLIENT .C */

int main()
{
	
	/*variables for the Logic */
	p1x = 0;	p1y = 0;
	p2x = 7;	p2y = 7;
	bomb1 = false;
	bomb2 = false;
    	int key;
    	int i,j;
    
	/* Initialization */
    	for(i = 0; i<8; i++) 
		    for(j = 0; j<8; j++)
            map[i][j]=0;
	map[p1y][p1x] = 1;
	map[p2y][p2x] = 2;
 	
    	pthread_t firstThread, secondThread, firstbombThread, secondbombThread;
    	int threadErr;
	
	/* variables for Socket Network*/
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	//char ch = 'A';
	char ch;
	
	
	/////////////////////////////////////////
	/* Socket (네트워크) variables */
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
	/////////////////////////////////////////	

	
	//Main Logic : 게임
	while(1){	
		
		if(end){
			end = false;
			sleep(7);
	     		
			p1x = 0;        p1y = 0;
	         	p2x = 7;        p2y = 7;
 	         	bomb1 = false;
 	         	bomb2 = false;

		 	for(i = 0; i<8; i++)		 
				for(j=0; j <8;j++)
            map[i][j] =0; 
			//memset(map, 0 ,64);
	    	 	map[p1y][p1x] = 1; /* P1's Location */
    		 	map[p2y][p2x] = 2; /* P2's Location */
 
		}
		
		
		printf("input: ");
		//scanf("%c\n", &ch);
		
		key = getch();           // 3. 방향키가 입력瑛 때 224 00 이 버퍼에 있다. 224부터 빼준다. 
        	//key_process(key);
		
		if(key== 27|| key ==0) {
			key = getch();
			key = getch();
            	switch(key) {
              		case 68:
                  		printf("1p left\n");
                  		move(LEFT);
                  		break;
              		case 67:
                  		printf("1p right\n");
                  		move(RIGHT);
                  		break;
              		case 65:
                  		printf("1p up\n");
                  		move(UP);
                  		break;
              		case 66:
                  		printf("1p down\n");
                  		move(DOWN);
                  		break;
         	}
        	}
		else{
			if(key == BOMB1){
				if(threadErr = pthread_create(&firstbombThread, NULL, firstbombThreadRun, NULL)){
					printf("Thread Err = %d", threadErr);
				}

			}else if(key == BOMB2){
				if(threadErr = pthread_create(&secondbombThread, NULL, secondbombThreadRun, NULL)){
					printf("Thread Err = %d", threadErr);
				}
			}else if(key == UP2|| key == 119){
				printf("2p up\n");
				move(UP2);
			}else if(key == LEFT2 || key == 97){
				printf("2p left\n");
				move(LEFT2);
			}else if(key == DOWN2 || key == 115){
				printf("2p down\n");
				move(DOWN2);
			}else if(key == RIGHT2 || key == 100){
				printf("2p right\n");
				move(RIGHT2);
			}
			else
				printf("NOPE\n");
		}
		if(end)	continue;
			
		for(i = 0; i<8; i++){
			for(j = 0; j<8; j++) printf("%u",map[i][j]);	
			printf("\n");
		}
		
		//write(sockfd, &ch, 1);	
		//read(sockfd, &ch, 1);
		
		/* socket size = char(1)x8x8 */
		/* last bit(65): Win-flag (isWin?->false/default:0, True:'1' or '2' or '3' (W/L/D) )*/
		write(sockfd, &map, 64);	
		read(sockfd, &map, 64);
	
	
		//printf("char from server = %c\n", ch);
	
		if(die){
			printf("quit by Win\n");
			close(sockfd);
			exit(0);
		}
		else {
			printf("not quit \n");
		}
	}
}
/*
void key_process(int key){
	if(key== 27|| key ==0) {
		key = getch();
		key = getch();
            	switch(key) {
            		case 68:
                  		printf("1p left\n");
                  		move(LEFT);
                  		break;
              		case 67:
                  		printf("1p right\n");
                  		move(RIGHT);
                  		break;
              		case 65:
                  		printf("1p up\n");
                  		move(UP);
                  		break;
              		case 66:
                  		printf("1p down\n");
                  		move(DOWN);
                  		break;
         	}
        }
	else{
		if(key == BOMB1){
			if(threadErr = pthread_create(&firstbombThread, NULL, firstbombThreadRun, NULL)){
				printf("Thread Err = %d", threadErr);			
			}
		}else if(key == BOMB2){
			if(threadErr = pthread_create(&secondbombThread, NULL, secondbombThreadRun, NULL)){
				printf("Thread Err = %d", threadErr);
			}
		}else if(key == UP2|| key == 119){
			printf("2p up\n");
			move(UP2);
		}else if(key == LEFT2 || key == 97){
			printf("2p left\n");
			move(LEFT2);
		}else if(key == DOWN2 || key == 115){
			printf("2p down\n");
			move(DOWN2);
		}else if(key == RIGHT2 || key == 100){
			printf("2p right\n");
			move(RIGHT2);
		}
		else
			printf("NOPE\n");
		}
	}
}	
*/

void die(int a){
 	printf("BOMB!!!!!\n");
	if(a == 1){
		bomb1 = false;
		map[b1y][b1x] = 0; /* 기존위치를 0으로*/
		if(((p1x>=b1x-3 && p1x<=b1x+3) && (p1y>=b1y-3 && p1y<=b1y+3))&&((p2x>=b1x-3 && p2x<= b1x+3) && (p2y>=b1y-3 && p2y<=b1y+3))){
                         printf("DRAW\n");
			 //map[64] = 3; /*map[64]는 winner 비트 winner bit = 3(Draw) */
			 end = true;
                 }else if((p1x>=b1x-3 && p1x<=b1x+3) && (p1y>=b1y-3 && p1y<=b1y+3)){
                         printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 
			 //map[64] = 2; /* winner bit = 2(2pwin) */
			 end = true;
                 }else if((p2x>=b1x-3 && p2x<= b1x+3) && (p2y>=b1y-3 && p2y<=b1y+3)){
                         printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
			 printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
			 printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
			 printf("1P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 //map[64] = 1; /* winner bit = 1(1pwin) */
			 end = true;
                 }else{
			printf("continue\n");
		 }			
 
         }else{
		bomb2 = false;
		map[b2y][b2x] = 0;
                 if(((p1x>=b2x-3 && p1x<=b2x+3) && (p1y>=b2y-3 && p1y<=b2y+3))&&((p2x>=b2x-3 && p2x<= b2x+3) && (p2y>=b2y-3 && p2y<=b2y+3))){
                         printf("DRaW\n");
			 //map[64] = 3; /* winner bit = 3(Draw) */
			 end = true;
                 }else if((p1x>=b2x-3 && p1x<=b2x+3) && (p1y>=b2y-3 && p1y<=b2y+3)){
                         printf("2P WIN\n");
			 //map[64] = 2; /* winner bit = 2(2PWIN) */
			 end = true;
                 }else if((p2x>=b2x-3 && p2x<= b2x+3) && (p2y>=b2y-3 && p2y<=b2y+3)){
                         printf("1P WIN\n");
			 //map[64] = 1; /* winner bit = 1(1PWIN) */
			 end = true;
                 }else{
			printf("continue\n");
		 }
         }
 
 }




void move(int m){
	switch(m){
		case LEFT:
			if(p1x>0){
				if(map[p1y][p1x] != 3) /* 'B' == 3*/
				map[p1y][p1x] = 0;
				p1x--;
				map[p1y][p1x] = 1;
			}
			break;
		case RIGHT:
			if(p1x<7){
				if(map[p1y][p1x] != 3)
				map[p1y][p1x] = 0;
				p1x++;
				map[p1y][p1x] = 1;
			}
			break;
		case UP:
			if(p1y>0){
				if(map[p1y][p1x] != 3)
				map[p1y][p1x] = 0;
				p1y--;
				map[p1y][p1x] = 1;
			}
			break;
		case DOWN:
			if(p1y<7){
				if(map[p1y][p1x] != 3)
				map[p1y][p1x] = 0;
				p1y++;
				map[p1y][p1x] = 1;
			}
			break;
		case LEFT2:
			if(p2x>0){
				if(map[p2y][p2x] != 3)
				map[p2y][p2x] = 0;
				p2x--;
				map[p2y][p2x] = 2;
			}
			break;
		case RIGHT2:
			if(p2x<7){
                                if(map[p2y*8+p2x] != 3)
				map[p2y][p2x] = 0;
				p2x++;
				map[p2y][p2x] = 2;
			}
			break;
		case UP2:
			if(p2y>0){
                                if(map[p2y*8+p2x] != 3)
				map[p2y][p2x] = 0;
				p2y--;
				map[p2y][p2x] = 2;
			}
			break;
		case DOWN2:
			if(p2y<7){
                                if(map[p2y*8+p2x] != 3)
				map[p2y][p2x] = 0;
				p2y++;
				map[p2y][p2x] = 2;
			}
			break;
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
