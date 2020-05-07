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

#define BPortNum "22672"
#define HOST "localhost"
#define MAX 100
int propspeed;
int transpeed;
long filesize;
int Vexnum = 0;
int dist[MAX] = {0};
int vexb[MAX] = {0};
float delay[MAX] = {0};
float Dist[MAX] = {0};
float FIL = 0;
int main(void){
    int sockfd;
    int rv;
    struct addrinfo hints;  // the struct addrinfo have already been filled with relevant info
    struct addrinfo *servinfo; //point out the result
    struct sockaddr_storage their_addr;
    struct addrinfo *p;  //tempoary point
    socklen_t addr_len;
    //UDP code from Beej
    memset(&hints, 0, sizeof hints);  // make sure the struct is empty
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP dgram sockets
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(HOST, BPortNum, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 0;
        }
    // loop through all the results and bind to the first we can----Beej
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
                == -1) {
            perror("serverB: socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("serverB: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "serverB: failed to bind socket\n");
        return 0;
    }
    freeaddrinfo(servinfo);
    printf( "The Server B is up and running using UDP on port %s.\n", BPortNum);
    while(1){
        addr_len = sizeof their_addr;
        

        recvfrom(sockfd, (char *)&propspeed, sizeof propspeed , 0,(struct sockaddr *)&their_addr, &addr_len);
        recvfrom(sockfd, (char *)&transpeed, sizeof transpeed , 0,(struct sockaddr *)&their_addr, &addr_len);
        recvfrom(sockfd, (char *)&filesize, sizeof filesize , 0,(struct sockaddr *)&their_addr, &addr_len);
        recvfrom(sockfd, (char *)&Vexnum, sizeof Vexnum , 0,(struct sockaddr *)&their_addr, &addr_len);
        recvfrom(sockfd, (char *)&vexb, sizeof dist , 0,(struct sockaddr *)&their_addr, &addr_len);
        recvfrom(sockfd, (char *)&dist, sizeof dist , 0,(struct sockaddr *)&their_addr, &addr_len);
        
        printf("The Server B has recieved data for calculation:\n");
        printf("* Propagation speed: %d km/s;\n",propspeed);
        printf("* Transmission speed: %d Bytes/s;\n",transpeed);
        printf("* filesize: %ld bits;\n",filesize);
        for(int i = 0; i < Vexnum; i++){
            printf("* path length for destination %d: %d\n",vexb[i],dist[i]);
        }
        printf("The Server B has finished the calculation of the delays:\n");
        printf("------------------------\n");
        printf("Destination        Delay\n");
        for(int j = 0; j < Vexnum; j++){
            Dist[j] = dist[j];
            FIL = filesize;
            delay[j] = ((float)(Dist[j]/(propspeed))+(float)(FIL/(transpeed)/8));
            //delay[j] = 0.23;
            printf("%d                     %.2f\n", vexb[j], delay[j]);
        }
        printf("------------------------\n");
        printf("The Server B has finished sending the output to AWS.\n");
        sendto(sockfd, (char *)& delay, sizeof delay , 0,(struct sockaddr *) &their_addr, addr_len);
        //printf("The Server B has finished sending the output to AWS");
    }
}
