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

#define PortNumUDP "23672"
#define PortNumTCP "24672"//make adjustment
#define BACKLOG 10 // how many pending connections queue will hold
#define APortNum "21672"
#define BPortNum "22672"
#define HOST "localhost"
#define MAX 100

//AWS -> ServerA: AWS forwards the map ID and source node to serverA via UDP
int propspeed =0;
int transpeed =0;
int Vexnum;
int dist[MAX] = {0};
int vexb[MAX] = {0};
float delay[MAX] = {0};
float Tt = 0;
float FIL = 0;
long SourceVertex;
int sv;


void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}




int PathFinding(char map_id, long SourceVertex)
{
    int pfsock = 0 ;
	struct addrinfo hints, *servinfo, *p;
	int rv;

//set up UDP code from beej
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_DGRAM;


    if ((rv = getaddrinfo(HOST, APortNum, &hints, &servinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((pfsock = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
    }
//exchange messages  with server A
    sendto(pfsock, (char*)&map_id, sizeof map_id, 0, p->ai_addr,p->ai_addrlen);
    sendto(pfsock, (char*)&SourceVertex, sizeof SourceVertex, 0, p->ai_addr,
           p->ai_addrlen);

    printf("The AWS has sent map ID and starting vertex to server A using UDP over port %s.\n", APortNum);
    
    recvfrom(pfsock, (char *)&Vexnum, sizeof Vexnum, 0, p->ai_addr,NULL);
    //printf("The Vexnum is %d\n", Vexnum);
    recvfrom(pfsock, (char *)&vexb, sizeof vexb, 0, p->ai_addr,&p->ai_addrlen);
    recvfrom(pfsock, (char *)&dist, sizeof dist, 0, p->ai_addr,&p->ai_addrlen);
    recvfrom(pfsock, (char *)&sv, sizeof sv, 0, p->ai_addr,&p->ai_addrlen);
    printf("The AWS has receieved shortest path from serverA:\n");
    printf("---------------------------\n");
    printf("Destination   Min Length\n");
    printf("---------------------------\n");
    
    for (int i = 0; i < Vexnum; i++)
        printf("%d           %d\n",vexb[i], dist[i]);
    printf("---------------------------\n");

    
    recvfrom(pfsock, (char *)&propspeed, sizeof propspeed, 0, p->ai_addr,&p->ai_addrlen);
    
    recvfrom(pfsock, (char *)&transpeed, sizeof transpeed, 0, p->ai_addr,&p->ai_addrlen);
    
    return 0;
    
   
}

int DelayCalcu(int propspeed, int transpeed, long filesize)
{
    int pfsock = 0 ;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;


    if ((rv = getaddrinfo(HOST, BPortNum, &hints, &servinfo))
            != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((pfsock = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
                == -1) {
            perror("talker: socket");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }

    sendto(pfsock, (char*)&propspeed, sizeof propspeed, 0, p->ai_addr,p->ai_addrlen);
    sendto(pfsock, (char*)&transpeed, sizeof transpeed, 0, p->ai_addr,p->ai_addrlen);
    sendto(pfsock, (char*)&filesize, sizeof filesize, 0, p->ai_addr,p->ai_addrlen);
    sendto(pfsock, (char*)&Vexnum, sizeof Vexnum, 0, p->ai_addr,p->ai_addrlen);
    sendto(pfsock, (char*)&vexb, sizeof vexb, 0, p->ai_addr,p->ai_addrlen);
    sendto(pfsock, (char*)&dist, sizeof dist, 0, p->ai_addr,p->ai_addrlen);
    
    printf("The AWS has sent path length, propagation speed and transmission speed to server B using UDP over port %s\n", BPortNum);
    recvfrom(pfsock, (char *)&delay, sizeof delay, 0, NULL, NULL);
    
    printf("The AWS has received delays from server B:\n");
    printf("---------------------------------------------\n");
    printf("Destination          Tt       Tp        Delay\n");
    printf("---------------------------------------------\n");
    FIL = filesize;
    Tt = ((float)(FIL/(transpeed)/8));
    
    for(int i = 0; i < Vexnum; i++){
        if(i != SourceVertex){
            float Tp = delay[i] - Tt;
            printf("%d                  %.2f       %.2f       %.2f\n", vexb[i], Tt, Tp, delay[i]);
        }else{
            printf("%d                  0.00       0.00       0.00\n", vexb[i]);
        }
    }
    printf("---------------------------------------------\n");
    return 0;
}

int main(){
	//set up TCP from Beej
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    
    //TCP code from Beej
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(HOST, PortNumTCP, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}
	freeaddrinfo(servinfo); // all done with this structure

	//listen
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	
    printf( "The AWS is up and running. \n");


    while(1){
		sin_size = sizeof their_addr;	
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");			
			exit(1);
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
		struct sockaddr_in addrTheir;
		memset(&addrTheir, 0, sizeof(addrTheir));
		int len = sizeof(addrTheir);
		getpeername(new_fd, (struct sockaddr *) &addrTheir, (socklen_t *) &len);
        int client_port = addrTheir.sin_port;

        char map_id;
        
        long filesize;

        recv(new_fd, (char *)&map_id, sizeof map_id, 0);
        recv(new_fd, (char *)& SourceVertex, sizeof SourceVertex, 0);
        recv(new_fd, (char *)& filesize, sizeof filesize, 0);
        
        printf("The AWS has received map ID %c, start vertex %ld and file size %ld from the client using TCP over port %d.\n", map_id, SourceVertex, filesize, client_port);
        
        PathFinding(map_id, SourceVertex);
        
        DelayCalcu(propspeed, transpeed, filesize);
        
        send(new_fd, (char *)&sv, sizeof sv,0);
        send(new_fd, (char *)&vexb, sizeof vexb,0);
        send(new_fd, (char *)&dist, sizeof dist,0);
        send(new_fd, (char *)&delay, sizeof delay,0);
        send(new_fd, (char *)&Vexnum, sizeof Vexnum,0);
        send(new_fd, (char *)&Tt, sizeof Tt,0);
        printf("The AWS has sent calculated delay to client using TCP over port %d.\n",client_port);
        
        
        
        
        close(new_fd);
    }

}
