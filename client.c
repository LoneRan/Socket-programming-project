#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>


#define AWSPortNumTCP "24672"

#define HOST "localhost"
#define MAX 100
// Client -> AWS: clent sends the map ID, the source node in the map and the transimission file size(unit"bit) to AWS via TCP
int Vexnum;
int dist[MAX] = {0};
int vexb[MAX] = {0};
float delay[MAX] = {0};
float Tt = 0;
int sv;
float FIL = 0;
int transpeed;

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}


int main(int argc, char* argv[]){
	
	
	
    char map_id ;
    char *SourceVerte ;
    char *filesiz  ;
    
    /*
    for(int count = 1; count < argc; count++)
    {
    printf("%d: %s\n", count, argv[count]);
    }
    strcpy(map_id,argv[0]);
    strcpy(SourceVerte,argv[1]);
    strcpy(filesiz,argv[2]);    That's where cause the segamentation fault 11
    */
    map_id = *argv[1];
    SourceVerte = argv[2];
    filesiz = argv[3];
    long SourceVertex = atoi(SourceVerte);
    long filesize = atol(filesiz);
    //int filesize = IntegerStr2Integer1(argv[3]);
    //printf("filesize is %ld", filesize);
     

	int sockfd = 0;
	struct addrinfo hints, *servinfo, *p;
	int rv;
    //set up TCP, code from Beej
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(HOST, AWSPortNumTCP, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect. \n");
		exit(0);
		return 2;
	}
	freeaddrinfo(servinfo); // all done with this structure
    printf("The client is up and running. \n");
    printf("The client has sent query to AWS using TCP over port %s; start vertex %ld; map %c; file size %ld \n", AWSPortNumTCP, SourceVertex, map_id, filesize);
    send(sockfd, (char*)&map_id, sizeof map_id, 0);
    send(sockfd, (char *)&SourceVertex, sizeof SourceVertex, 0);
    send(sockfd, (char *)&filesize, sizeof filesize,0);

   
    recv(sockfd, (char *)&sv, sizeof sv, 0);
    recv(sockfd, (char *)&vexb, sizeof vexb, 0);
    recv(sockfd, (char *)&dist, sizeof dist, 0);
    recv(sockfd, (char *)&delay, sizeof delay, 0);
    recv(sockfd, (char *)&Vexnum, sizeof Vexnum, 0);
    recv(sockfd, (char *)&Tt,sizeof Tt, 0);
    
    printf("The client has received results from AWS:\n");
    printf("------------------------------------------------\n");
    printf("Destination    Min Length    Tt    Tp     Delay\n");
    printf("------------------------------------------------\n");
    
    for(int i = 0; i < Vexnum; i++){
        if(i != sv){
            float Tp = delay[i] - Tt;
            printf("%d                  %.2d       %.2f   %.2f   %.2f\n", vexb[i], dist[i], Tt, Tp, delay[i]);
        }else{
            printf("%d                  0       0.00   0.00    0.00\n", vexb[i]);
        }
    }
    printf("------------------------------------------------\n");

}
