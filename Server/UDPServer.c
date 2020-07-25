// Server code for UDP socket programming
//Author: Vicente Fernández Lledó


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
#define PORT_NO 15050
#define NET_BUF_SIZE 32
#define sendrecvflag 0


// Functions
void clearBuf(char* b)
{
	int i;
	for (i = 0; i < NET_BUF_SIZE; i++)
		b[i] = '\0';
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
	int fNlength;
	FILE* fp;

	DIR *d;
	struct dirent *dir;

	struct sockaddr_in addr_con;
	int addrlen = sizeof(addr_con);
	addr_con.sin_family = AF_INET;
	addr_con.sin_port = htons(PORT_NO);
	addr_con.sin_addr.s_addr = INADDR_ANY;


	// SOCKET()
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);

	if (sockfd < 0)
		printf("\nfile descriptor not received!!\n");
	else
		printf("\nfile descriptor %d received\n", sockfd);

	// BIND()
	if (bind(sockfd, (struct sockaddr*) &addr_con, sizeof(addr_con)) == 0)
		printf("\nSuccessfully binded!\n");
	else
		printf("\nBinding Failed!\n");

	while (1) {
		printf("\nWaiting for instruction...\n");

		// receive first info or file
	    clearBuf(net_buf);
		nBytes = recvfrom(sockfd, net_buf,
						NET_BUF_SIZE, sendrecvflag,
						(struct sockaddr*)&addr_con, &addrlen);

		//Check if Server has recieved list command
	    if (strcmp(net_buf, "\nlist")==0){
	      	printf("LIST Command Received\n");
	      	d = opendir(".");

	    	if (d) {
	        	while ((dir = readdir(d)) != NULL) {
	            	strncpy(fileName, dir->d_name, 254);
	            	fNlength = strlen(dir->d_name);
	            	fileName[fNlength] = '\n';
	            	sendto(sockfd, fileName, NET_BUF_SIZE,
	              		sendrecvflag, (struct sockaddr*)&addr_con,
	              		addrlen);
	        }

	        	closedir(d);
	    	}

	    	net_buf[0]=EOF;
	    	sendto(sockfd, net_buf, NET_BUF_SIZE,
	        	sendrecvflag, (struct sockaddr*)&addr_con,
	        	addrlen);
	    	continue;
		}

		//Check if server has received upload command
		if(net_buf[0]=='\n' && net_buf[1]=='u' && net_buf[2]=='p'){
			clearBuf(fileName);
			for (int i = 3; i < NET_BUF_SIZE; i++){
				if (net_buf[i]=='\n')
					break;
				fileName[i-3] = net_buf[i];
			}

			//receive file
			printf("Receiving %s\n", fileName);
			fp = fopen(fileName, "w");
			while (1){

				clearBuf(net_buf);
				nBytes = recvfrom(sockfd, net_buf,
								NET_BUF_SIZE, sendrecvflag,
								(struct sockaddr*)&addr_con, &addrlen);

				//Añadir buff a buff al archivo y cerrarlo
				if (saveFile(fp, net_buf, NET_BUF_SIZE)) {
		          	fclose(fp);
		          	printf("\n%s saved in server directory!\n", fileName);
		          	break;
		        }

			}

			continue;
		}

		//Otherwise, a file name is received, so it has to be sended
		fp = fopen(net_buf, "r");
		printf("\nFile Name Received: %s\n", net_buf);
		if (fp == NULL)
			printf("\nFile open failed!\n");
		else
			printf("\nFile Successfully opened!\n");

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
		if (fp != NULL)
			fclose(fp);

	}
	return 0;
}
