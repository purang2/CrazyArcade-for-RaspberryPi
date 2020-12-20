#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

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


#define RGB565_RED 0xF800
#define RGB565_GREEN 0x07E0
#define RGB565_BLUE 0x001F



/* RPI64_CLIENT .C */

int p1x;         /* player 1's location */
int p1y;
int p2x;        /* player 2's location */
int p2y;
bool bomb1;
bool bomb2;

int b1x;         /* Bombs' location */
int b2x;
int b1y;
int b2y;

int end;         /* Game-over flag (Does game over?)*/

//char map[8][8];          /* LED Matrix 2D array */
unsigned short *map;
//unsigned shor map[8][8];
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
                *(map+b1y*8+b1x) = RGB565_BLUE; /* BOMB : 3*/
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
                 *(map+b2y*8+b2x) =RGB565_BLUE;
                 sleep(2);
                 die(2);
       }

}


void paint(int end){
        int i, j;
        if(end == 1)
                for(i = 0; i<8; i++)
                        for(j = 0; j<8; j++)
                                *(map+i*8+j) = RGB565_RED;
        else if(end == 2)
                for(i = 0; i<8; i++)
                        for(j = 0; j<8; j++)
                                *(map+i*8+j) = RGB565_GREEN;
        else if(end == 3)
                for(i = 0; i<8; i++)
                        for(j = 0; j<8; j++)
                                *(map+i*8+j) = RGB565_BLUE;
}


/* RPI_CLIENT .C */

int main()
{
        end = 0;
        /*variables for the Logic */
        p1x = 0;        p1y = 0;
        p2x = 7;        p2y = 7;
        bomb1 = false;
        bomb2 = false;
        int key;
        int i,j;
        map= (unsigned short*)malloc(1024);
        /* Initialization */
        for(i = 0; i<8; i++)
                    for(j = 0; j<8; j++)
                        *(map+i*8+j)=0;
        *(map+p1y*8+p1x) = RGB565_RED;
        *(map+p2y*8+p2x) = RGB565_GREEN;

        pthread_t firstThread, secondThread, firstbombThread, secondbombThread;
        int threadErr;

        /* variables for Socket Network*/
        int sockfd;
        int len;
        struct sockaddr_in address;
        int result;
        //char ch = 'A';
        char ch;



        while(1){


        /////////////////////////////////////////
        /* Socket (네트워크) variables */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        //while(1){

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr("127.0.0.1");
        address.sin_port = htons(2020);
        len = sizeof(address);

        result = connect(sockfd, (struct sockaddr *)&address, len);

        if(result == -1){
                perror("oops: sockfd connect error");
                exit(1);
        }
        /////////////////////////////////////////


        //Main Logic : 게임
        //while(1){

                if(end){
                        end = false;
                        sleep(7);

                        p1x = 0;        p1y = 0;
                        p2x = 7;        p2y = 7;
                        bomb1 = false;
                        bomb2 = false;

                        for(i = 0; i<8; i++)
                                for(j=0; j <8;j++)
                        *(map+i*8+j) =0;
                        //memset(map, 0 ,64);
                        *(map+p1y*8+p1x) = RGB565_RED; // P1's Location /
                        *(map+p2y*8+p2x) = RGB565_GREEN; // P2's Location

                }

        /*if(end!=0){
                printf("QUIT");
                paint(end);
                write(sockfd, map, 1024);
                read(sockfd, map, 1024);
                close(sockfd);
                exit(0);
        }       */
                printf("input: ");
                //scanf("%c\n", &ch);

                key = getch();           // 3. 방향키가 입력瑛 때 27 0000 이 버퍼에 있다. 27부터 빼준다.
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
                //if(end)       continue;

                for(i = 0; i<8; i++){
                        for(j = 0; j<8; j++) printf("%u",*(map+i*8+j));
                        printf("\n");
                }

                //write(sockfd, &ch, 1);
                //read(sockfd, &ch, 1);

                /* socket size = char(1)x8x8 */
                /* last bit(65): Win-flag (isWin?->false/default:0, True:'1' or '2' or '3' (W/L/D) )*/
                write(sockfd, map, 1024);
                read(sockfd, map, 1024);


                //printf("char from server = %c\n", ch);

                if(end!=0){
                        printf("quit by Win\n");
                        paint(end);
                        close(sockfd);
                        //exit(0);
                }
                else {
                        printf("not quit \n");

                        close(sockfd);
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
        int i, j;
        printf("BOMB!!!!!\n");
        if(a == 1){
                bomb1 = false;
                *(map+b1y*8+b1x) = 0; /* 기존위치를 0으로*/
                if(((p1x>=b1x-2 && p1x<=b1x+2) && (p1y>=b1y-2 && p1y<=b1y+2))&&((p2x>=b1x-2 && p2x<= b1x+2) && (p2y>=b1y-2 && p2y<=b1y+2))){
                         printf("DRAW\n");
                         *(map+64) = 3; //map[64]는 winner 비트 winner bit = 3(Draw)

                         end = 3;
                 }else if((p1x>=b1x-2 && p1x<=b1x+2) && (p1y>=b1y-2 && p1y<=b1y+2)){
                         printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
                         printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
                         printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
                         printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
                         printf("2P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
                         end = 2;
                         *(map+64) = 2; // winner bit = 2(2pwin) /
                 }else if((p2x>=b1x-2 && p2x<= b1x+2) && (p2y>=b1y-2 && p2y<=b1y+2)){
                         printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
                         printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
                         printf("1P WIN!!!!!!\n!!!!!!congraturation!!!!!!\n");
                         printf("1P WIN!!!!!!!\n!!!!congraturation!!!!!!\n");
                         *(map+64) = 1; // winner bit = 1(1pwin)

                         end = 1;

                 }else{
                        printf("continue\n");
                 }

         }else{
                bomb2 = false;
                *(map+b2y*8+b2x) = 0;
                 if(((p1x>=b2x-2 && p1x<=b2x+2) && (p1y>=b2y-2 && p1y<=b2y+2))&&((p2x>=b2x-2 && p2x<= b2x+2) && (p2y>=b2y-2 && p2y<=b2y+2))){
                         printf("DRaW\n");
                         *(map+64) = 3; // winner bit = 3(Draw) /
                         end = 3;
                 }else if((p1x>=b2x-2 && p1x<=b2x+2) && (p1y>=b2y-2 && p1y<=b2y+2)){
                         printf("2P WIN\n");
                         *(map+64) = 2; // winner bit = 2(2PWIN)

                         end = 2;
                 }else if((p2x>=b2x-2 && p2x<= b2x+2) && (p2y>=b2y-2 && p2y<=b2y+2)){
                         printf("1P WIN\n");
                         *(map+64) = 1; // winner bit = 1(1PWIN)
                         end = 1;
                 }else{
                        printf("continue\n");
                 }
         }

 }




void move(int m){
        switch(m){
                case LEFT:
                        if(p1x>0){
                                if(*(map+p1y*8+p1x) != RGB565_BLUE) /* 'B' == 3*/
                                *(map+p1y*8+p1x) = 0;
                                p1x--;
                                *(map+p1y*8+p1x) = RGB565_RED;
                        }
                        break;
                case RIGHT:
                        if(p1x<7){
                                if(*(map+p1y*8+p1x) != RGB565_BLUE)
                                *(map+p1y*8+p1x) = 0;
                                p1x++;
                                *(map+p1y*8+p1x) = RGB565_RED;
                        }
                        break;
                case UP:
                        if(p1y>0){
                                if(*(map+p1y*8+p1x) != RGB565_BLUE)
                                *(map+p1y*8+p1x) = 0;
                                p1y--;
                                *(map+p1y*8+p1x) = RGB565_RED;
                        }
                        break;
                case DOWN:
                        if(p1y<7){
                                if(*(map+p1y*8+p1x) != RGB565_BLUE)
                                *(map+p1y*8+p1x) = 0;
                                p1y++;
                                *(map+p1y*8+p1x) = RGB565_RED;
                        }
                        break;
                case LEFT2:
                        if(p2x>0){
                                if(*(map+p2y*8+p2x) != RGB565_BLUE)
                                *(map+p2y*8+p2x) = 0;
                                p2x--;
                                *(map+p2y*8+p2x) = RGB565_GREEN;
                        }
                        break;
                case RIGHT2:
                        if(p2x<7){
                                if(*(map+p2y*8+p2x) != RGB565_BLUE)
                                *(map+p2y*8+p2x) = 0;
                                p2x++;
                                *(map+p2y*8+p2x) = RGB565_GREEN;
                        }
                        break;
                case UP2:
                        if(p2y>0){
                                if(*(map+p2y*8+p2x) != RGB565_BLUE)
                                *(map+p2y*8+p2x) = 0;
                                p2y--;
                                *(map+p2y*8+p2x) = RGB565_GREEN;
                        }
                        break;
                case DOWN2:
                        if(p2y<7){
                                if(*(map+p2y*8+p2x) != RGB565_BLUE)
                                *(map+p2y*8+p2x) = 0;
                                p2y++;
                                *(map+p2y*8+p2x) = RGB565_GREEN;
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
