#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

/*Includes about LED matrix */
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define FILEPATH "/dev/fb0" // device path
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS *sizeof(uint16_t))


/*
Define 16bit for Color 
RRRR RGGG GGGB BBBB
*/
#define RGB565_RED 0xF800 /* Red-color FrameBuffer */
#define RGB565_GREEN 0x07E0 /* GREEN-color FrameBuffer */
#define RGB565_BLUE 0x001F /* BLUE-color FrameBuffer */


/* RPI-server.c */

int main()
{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	
	
	/*vars for LED MATRIX*/
	int i;
	int fbfd;
	char *map;
	uint16_t *p;
	struct fb_fix_screeninfo fix_info;
	
	
	/* open the led frame buffer device */
	fbfd = open(FILEPATH, O_RDWR);
	if(fbfd == -1){
		perror("Error (call to 'open')");
		exit(EXIT_FAILURE);
	}
	
	/* read fixed screen info for the open device */
	if(ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
		perror("Error (call to 'ioctl')");
		close(fbfd);
		exit(EXIT_FAILURE);
	}
	
	/* now check the correct device has been found */
	if(strcmp(fix_info.id, "RPI-Sense FB") != 0 ){
		printf("%s\n", "Error: Rpi-Sense FB not found");
		close(fbfd);
		exit(EXIT_FAILURE);
	}
	
	/* map the led frame buffer device into memory */
	map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if( map== MAP_FAILED){
		close(fbfd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
	
	
	
	/* For running server */
	unlink("server socket");
	server_sockfd = socket(AF_INET, SOCK_STREAM , 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address,server_len);

	
	/*set a pointer to the start of the memory area */
	p=map;
	
	/*clear the led matrix */
	memset(map, 0 , FILESIZE);
	
	
	/* make wait-Queue and infinite-loop(Running-loop) */
	listen(server_sockfd, 5);
	while(1){
		listen(server_sockfd, 5);
	
		printf("server waiting\n");


		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

		//read(client_sockfd, &ch, 1);
		read(client_sockfd, &map, 8*8+1);
		
		/*Logic */
		for(i =0 ; i< NUM_WORDS; i++) {
		   if(map[i] == '1') map[i]=RGB565_RED;
		   else if(map[i] == '2') map[i]=RGB565_GREEN;
		   else if(map[i] == 'B') map[i]=RGB565_BLUE;	
		}
		
		if(map[64]!=0){
			usleep(1000*1000);
			printf("GAME OVER!!\n");
			if(map[64]=='1') {
				printf("1P WIN!!\n");
				for(i=0;i<64;i++) {map[i]=RGB565_RED; usleep(25*1000);}
			}
			if(map[64]=='2') {
				printf("2P WIN!!\n");
				for(i=0;i<64;i++) {map[i]=RGB565_GREEN; usleep(25*1000);}
			}
			if(map[64]=='3') {
				printf("NOBODY IS ALIVE !!\n DRAW GAME!!\n");
				for(i=0;i<64;i++) {map[i]=RGB565_BLUE; usleep(25*1000);}
			}
			
			/* clear the led matrix */
			memset(map, 0, FILESIZE+1);
			
			/* un-map and close */
			if(munmap(map, FILESIZE+1) == -1){
				perror("Error unmapping the file");
			}
			close(fbfd);
			
			return 0;
		}
		
		write(client_sockfd, &map , 8*8+1);
		
		if(ch == 'Q' || ch == 'q'){
			printf("close by q\n");
		       	close(client_sockfd);
		}
	}
}

