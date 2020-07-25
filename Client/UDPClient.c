// Client code for UDP socket programming
// Author: Vicente Fernández Lledó


#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdbool.h>

#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT_NO 15050
#define NET_BUF_SIZE 32
#define sendrecvflag 0

// Colors for console
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

// Functions
void clearBuf(char* b)
{
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
}

int recvFile(char* buf, int s)
{
	int i;
	char ch;
	for (i = 0; i < s; i++) {
		ch = buf[i];
		if (ch == EOF){
    		return 1;
    	} else {
			printf("%c", ch);
    	}
	}
	return 0;
}

int sendFile(FILE* fp, char* buf, int s)
{
	int i, len;
	if (fp == NULL) {
		buf[0] = EOF;
		return 1;
	}
  	char ch;
  	for (i = 0; i < s; i++) {
      	ch = fgetc(fp);
      	buf[i] = ch;
      	if (ch == EOF)
          	return 1;
  }
	return 0;
}

int saveFile(FILE* fp, char* buf, int s)
{
	int i;
	char ch[1];
	for (i = 0; i < s; i++) {
		ch[0] = buf[i];
		if (ch[0] == EOF){
      		return 1;
    	} else {
      		fputs(ch, fp);
			printf("%c", ch[0]);
    	}
	}
	return 0;
}



// MAIN CODE
int main()
{
	int sockfd, nBytes;
	char net_buf[NET_BUF_SIZE];
	char fileName[NET_BUF_SIZE];
	char instruction[256];
	bool start = false;
	FILE* fp;

	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(PORT_NO);
	addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);


	// SOCKET()
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);

	if (sockfd < 0)
		printf("\nfile descriptor not received!!\n");
	else
		printf("\nfile descriptor %d received\n", sockfd);


	// RUNNING
	while (1) {

	    printf("\nPossible operations:\n  - upload: upload a file to the server\n  - download: download a file from the server\n  - list: list files in Server\n  - exit: close client\n");
	    printf("\nPlease enter instructions:\n");
	    scanf("%s", &instruction);

	    //DOWNLOAD FUNCTION
	    if (strcmp(instruction, "download") == 0 ){

	      	clearBuf(net_buf);
	      	printf("\nPlease enter file name to receive:\n");
	      	scanf("%s", net_buf);
	      	strcpy(fileName, net_buf);

	      	sendto(sockfd, net_buf, NET_BUF_SIZE,
	        	sendrecvflag, (struct sockaddr*)&addr_con,
	        	addrlen);

	      	start = true;
	      	printf("\n---------Server Answer---------\n");

	      	while (1) {

	        	// receive
	        	clearBuf(net_buf);
	        	nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
	                sendrecvflag, (struct sockaddr*)&addr_con,
	                &addrlen);

	        	//If file doesn't exist, jump out
	        	if (start && net_buf[0] == EOF){
	          		printf(RED "File not Found!" RESET);
	          		printf("\n-------------------------------\n");
	          		break;
	        	}

		        //If file exists, open it in write mode
		        if (start && net_buf[0] != EOF){ //
		          	fp = fopen(fileName, "w");
		          	start = false;
		        }

	        	// process
	        	if (saveFile(fp, net_buf, NET_BUF_SIZE)) {
	          		fclose(fp);
	          		printf("-------------------------------\n");
	          		printf(GREEN "\n%s saved in client directory!\n" RESET, fileName);
	          		break;
	        	}
	      	}


	    } else if (strcmp(instruction, "upload") == 0 ) {

			//UPLOAD FILE FUNCTION
			clearBuf(net_buf);
			clearBuf(fileName);

	        printf("\nPlease enter file name to send:\n");
	        scanf("%s", net_buf);
	        strcpy(fileName, net_buf);

			fp = fopen(fileName, "r");
		    printf("\nFile Name Received: %s\n", fileName);

			if (fp == NULL){
				printf(RED "\nFile open failed!\n" RESET);
				continue;
			} else {
				printf("\nFile Successfully opened!\n");
			}

			clearBuf(net_buf);
			strcpy(net_buf, "\nup");
			strcat(net_buf, fileName);
			strcat(net_buf, "\n");
			sendto(sockfd, net_buf, NET_BUF_SIZE,
				sendrecvflag,
				(struct sockaddr*)&addr_con, addrlen);
			clearBuf(net_buf);

			while (1) {

				// process
				if (sendFile(fp, net_buf, NET_BUF_SIZE)) {
				 	sendto(sockfd, net_buf, NET_BUF_SIZE,
						sendrecvflag,
						(struct sockaddr*)&addr_con, addrlen);
					break;
				}

				// send
				sendto(sockfd, net_buf, NET_BUF_SIZE,
					sendrecvflag,
					(struct sockaddr*)&addr_con, addrlen);
				clearBuf(net_buf);
			}

			printf(GREEN "\nFile Successfully sent!\n" RESET);
			if (fp != NULL)
				fclose(fp);



	    } else if (strcmp(instruction, "list") == 0 ) {

		    //LIST FUNCTION
		    clearBuf(net_buf);
		    strcpy(net_buf, "\nlist");

		    sendto(sockfd, net_buf, NET_BUF_SIZE,
		      sendrecvflag, (struct sockaddr*)&addr_con,
		      addrlen);

		    printf("\n---------Server Answer---------\n");

	    	while (1) {
				// receive
				clearBuf(net_buf);
	  			nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE,
	                sendrecvflag, (struct sockaddr*)&addr_con,
	                &addrlen);

	        	// process
	        	if (recvFile(net_buf, NET_BUF_SIZE)) {
	        		break;
	        	}
	      	}

	    	printf("\n-------------------------------\n");

	    } else if (strcmp(instruction, "exit") == 0 ) {

	      	printf(RED "Closing client...\n" RESET);
	      	return 0;

	    } else {

	      	printf(RED "\nPlease insert a valid command.\n" RESET);

	    }

	}

	return 0;
}
