#include <stdio.h>
#include <time.h>
#include <termio.h>
#include <unistd.h>
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




int p1x;
int p1y;
int p2x;
int p2y;
bool bomb1;
bool bomb2;

int b1x;
int b2x;
int b1y;
int b2y;

bool end;

char map[8][8];

void move(int m);
int getch(void);
void die(int a);


void *firstbombThreadRun(){
	if(bomb1 == false){
		bomb1 = true;
                b1x = p1x;
                b1y = p1y;
               	map[b1y][b1x] = 'B';
                sleep(2);
		die(1);		
       }

}

void *secondbombThreadRun(){
	if(bomb2 == false){
                 bomb2 = true;
                 b2x = p2x;
                 b2y = p2y;
                 map[b2y][b2x] = 'B';
                 sleep(2);
                 die(2);
       }

}

int main(void)
{
	p1x = 0;	p1y = 0;
	p2x = 7;	p2y = 7;
	bomb1 = false;
	bomb2 = false;
    int key;
    int i,j;
    
    for(i = 0; i<8; i++){
    	for(j = 0; j<8; j++){
    		map[i][j] = 'O';
		}
	}
    
    map[p1y][p1x] = '1';
    map[p2y][p2x] = '2';
 
    pthread_t firstThread, secondThread, firstbombThread, secondbombThread;
    int threadErr;


       
    while(1) 
    {
	if(end){
		end = false;
		sleep(7);
	     	p1x = 0;        p1y = 0;
	         p2x = 7;        p2y = 7;
 	         bomb1 = false;
 	         bomb2 = false;

		 for(i = 0; i<8; i++){
	         	for(j = 0; j<8; j++){
                		 map[i][j] = 'O';
                	 }
        	 }
 
	    	 map[p1y][p1x] = '1';
    		 map[p2y][p2x] = '2';
 

	}

       key = getch();           // 3. 방향키가 입력됬을 때 224 00 이 버퍼에 있다. 224부터 빼준다. 
        if(key == 27||key ==0) {
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
		for(j = 0; j<8; j++){
			printf("%c",map[i][j]);	
		}
		printf("\n");
	}
    }
    return 0;
}

void die(int a){
 	printf("BOMB!!!!!\n");
	if(a == 1){
		bomb1 = false;
		map[b1y][b1x] = 'O';
		if(((p1x>=b1x-3 && p1x<=b1x+3) && (p1y>=b1y-3 && p1y<=b1y+3))&&((p2x>=b1x-3 && p2x<= b1x+3) && (p2y>=b1y-3 && p2y<=b1y+3))){
                         printf("DRaW\n");
			 end = true;
                 }else if((p1x>=b1x-3 && p1x<=b1x+3) && (p1y>=b1y-3 && p1y<=b1y+3)){
                         printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 end = true;
                 }else if((p2x>=b1x-3 && p2x<= b1x+3) && (p2y>=b1y-3 && p2y<=b1y+3)){
                         printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
			 printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
			 printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
			 printf("1P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
			 end = true;
                 }else{
			printf("continue\n");
		 }			
 
         }else{
		bomb2 = false;
		map[b2y][b2x] = 'O';
                 if(((p1x>=b2x-3 && p1x<=b2x+3) && (p1y>=b2y-3 && p1y<=b2y+3))&&((p2x>=b2x-3 && p2x<= b2x+3) && (p2y>=b2y-3 && p2y<=b2y+3))){
                         printf("DRaW\n");
			 end = true;
                 }else if((p1x>=b2x-3 && p1x<=b2x+3) && (p1y>=b2y-3 && p1y<=b2y+3)){
                         printf("2P WIN\n");
			 end = true;
                 }else if((p2x>=b2x-3 && p2x<= b2x+3) && (p2y>=b2y-3 && p2y<=b2y+3)){
                         printf("1P WIN\n");
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
				if(map[p1y][p1x] != 'B')
				map[p1y][p1x] = 'O';
				p1x--;
				map[p1y][p1x] = '1';
			}
			break;
		case RIGHT:
			if(p1x<7){
				if(map[p1y][p1x] != 'B')
				map[p1y][p1x] = 'O';
				p1x++;
				map[p1y][p1x] = '1';
			}
			break;
		case UP:
			if(p1y>0){
				if(map[p1y][p1x] != 'B')
				map[p1y][p1x] = 'O';
				p1y--;
				map[p1y][p1x] = '1';
			}
			break;
		case DOWN:
			if(p1y<7){
				if(map[p1y][p1x] != 'B')
				map[p1y][p1x] = 'O';
				p1y++;
				map[p1y][p1x] = '1';
			}
			break;
		case LEFT2:
			if(p2x>0){
				if(map[p2y][p2x] != 'B')
				map[p2y][p2x] = 'O';
				p2x--;
				map[p2y][p2x] = '2';
			}
			break;
		case RIGHT2:
			if(p2x<7){
                                if(map[p2y][p2x] != 'B')
				map[p2y][p2x] = 'O';
				p2x++;
				map[p2y][p2x] = '2';
			}
			break;
		case UP2:
			if(p2y>0){
                                if(map[p2y][p2x] != 'B')
				map[p2y][p2x] = 'O';
				p2y--;
				map[p2y][p2x] = '2';
			}
			break;
		case DOWN2:
			if(p2y<7){
                                if(map[p2y][p2x] != 'B')
				map[p2y][p2x] = 'O';
				p2y++;
				map[p2y][p2x] = '2';
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

