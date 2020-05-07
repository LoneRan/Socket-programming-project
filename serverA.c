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



#define APortNum "21672"
#define MAX  100
#define INF  (~(0x1<<31))
#define MAXSIZE 1024
#define HOST "localhost"



char buf[100][1024];
int s[100][1024]={ 0 };

int Vexnum = 0;
int vexb[MAX] = {0};
int dist[MAX] = {0};
int prev[MAX] = {0};
int temper[MAX] = {0};
int new_array[10] = {0};
long sv;
typedef struct _graph
{
	char vexs[MAX];   //set of vertex
	int vexnum;       //number of vertex
	int edgnum;       //number of edges
	int matrix[MAX][MAX]; //adjacent matrix
}Graph, *PGraph;

typedef struct _EdgeData
{
	char start; //start of edge
	char end;   //end of edge
	int weight; //weight of edge
}EData;




void dijkstra(Graph G, long vs, int prev[], int dist[]) // Dijkstra code reference from www.cnblogs.com/skywang12345/
{
    int i,j,k = 0;
    int min;
    int tmp;
    int flag[MAX];      
    
    
    for (i = 0; i < G.vexnum; i++)
    {
        flag[i] = 0;              
        prev[i] = 0;              
        dist[i] = G.matrix[vs][i];
    }

    
    flag[vs] = 1;
    dist[vs] = 0;

    for (i = 1; i < G.vexnum; i++)
    {
        min = INF;
        for (j = 0; j < G.vexnum; j++)
        {
            if (flag[j]==0 && dist[j]<min)
            {
                min = dist[j];
                k = j;
            }
        }

        flag[k] = 1;

        for (j = 0; j < G.vexnum; j++)
        {
            tmp = (G.matrix[k][j]==INF ? INF : (min + G.matrix[k][j])); 
            if (flag[j] == 0 && (tmp  < dist[j]) )
            {
                dist[j] = tmp;
                prev[j] = k;
            }
        }
    }

	//print the outcome by dijkstra

    printf("The Server A has identified the following shortest paths:\n");
    printf("---------------------------\n");
    printf("Destination   Min Length\n");
    printf("---------------------------\n");
    
    for (i = 0; i < G.vexnum; i++)
        printf("%d           %d\n",G.vexs[i], dist[i]);
    printf("---------------------------\n");
}




int GetNum(char * str, int  i1, int  i2)
{
	int i,j;
	char tem[10];
    for (i = i1 + 1; i < i2; i++){
    	j = i -i1 -1;
    	tem[j] = str[i];
    };

    int res = atoi(tem);
    return res;
}

int NumMaps()
{
    int res = 0;
    for(int i = 0; i < 100; i++){
        if(s[i][0] >= 17 && s[i][0] <= 42) {
        	res = res + 1;
        }else if (s[i][0] == 0 && s[i][1] == 0){break;}
        //ASCII number from 65 to 90 is A to Z
    }
    return res;
}


int NumVertices(char ID){
    int flag1 = 0,flag2 = 0;
	int max = 0;
    int F[MAX] = {0};
    
    
    int asc = ID - 48;
	for(int i = 0; i < 100; i++){
		if(s[i][0] == asc){
			flag1 = i;  
			break;                      //find the location of specified map ID
		}
	}

	for(int q = flag1 + 1; q < 100; q++){
		if(s[q][0] >= 17 && s[q][0] <= 42){flag2 = q;break;}
        else if (s[q][0] == 0 && s[q][1] == 0){flag2 = q;break;}
	}
	for(int j = flag1 + 3; j < flag2; j++){        //skip the delay and throughput to the matrix imformation
        if(F[s[j][0]] == 0){
            max = max + 1;
            F[s[j][0]] = 1;
        }
        if(F[s[j][1]] == 0){
            max = max + 1;
            F[s[j][1]] = 1;
        }
        
                    //get the number of vertex of the matrix
	}
	Vexnum = max;
	return(Vexnum);
}

int NumEdges(char ID){
	int flag1 =0,flag2 = 0;

    
    int asc = ID - 48;
    for(int i = 0; i < 100; i++){
        if(s[i][0] == asc){
            flag1 = i;
            break;                      //find the location of specified map ID
        }
    }

	for(int q = flag1 + 1; q < 100; q++){
		if(s[q][0] >=17 && s[q][0] <= 42){flag2 = q;break;}
        else if (s[q][0] == 0 && s[q][1] == 0){flag2 = q;break;}
    }
	int res;
	res = flag2 - flag1 -3;
	return(res);
}

int PropSpeed(char ID)
{
    int res = 0;
    int asc = ID - 48;

    for(int i = 0; i < 100; i++){
        if(s[i][0] == asc){
            res = s[i+1][0];
            break;
        }
    }
    return res;
}

int TranSpeed(char ID)
{
    int res = 0;
    int asc = ID - 48;
    for(int i = 0; i < 100; i++){
        if(s[i][0] == asc){
            res = s[i+2][0];
            break;
        }
    }
    return res;
}


Graph* creatgraph(char  ID)
{
    for(int g =0;g<100;g++){
        temper[g] = INF;
    }
    for(int g =0;g<10;g++){
        new_array[g] = INF;
    }
    for(int g =0;g<10;g++){
        vexb[g] = 0;
    }
    int **MapMatrix;
    int flag1 = 0,flag2 = 0;
	int x,y,num = 0;
	int max = 0;
    int F[MAX] = {0};
    
    int asc = ID - 48;
    for(int i = 0; i < 100; i++){
        if(s[i][0] == asc){
            flag1 = i;
            break;                      //find the location of specified map ID
        }
    }


	for(int q = flag1 + 1; q < 100; q++){
		if(s[q][0] >=17 && s[q][0] <= 42){flag2 = q;break;}
        else if (s[q][0] == 0 && s[q][1] == 0){flag2 = q;break;}
	}
    for(int j = flag1 + 3; j < flag2; j++){        //skip the delay and throughput to the matrix imformation
        if(F[s[j][0]] == 0){
            max = max + 1;
            F[s[j][0]] = 1;
        }
        if(F[s[j][1]] == 0){
            max = max + 1;
            F[s[j][1]] = 1;
        }
                    //get the number of vertex of the matrix
    }
    max = max -1;
    Vexnum = max + 1;

	MapMatrix = (int **) malloc(sizeof(int *) *Vexnum);//ask for one dimension array with Vexnum
    for(int i = 0; i < Vexnum; i ++)
        MapMatrix[i] = (int *)malloc(sizeof(int) * Vexnum);  //intialize the matrix's size based on the pre-calculated max

    //memset(MapMatrix, INF, sizeof(MapMatrix)); //inistialize the matrix to be INF
    for (int k = 0; k < Vexnum; k++){
        for(int u = 0;u < Vexnum; u++){
            MapMatrix[k][u] = INF;
        }
    }
    
    
    for(int l = flag1; l < flag2-3; l++){
        temper[2*l - 2*flag1] = s[l+3][0];
        temper[2*l + 1 - 2*flag1] = s[l+3][1];
    }
    /*
    for( int i = 0; i<20; i++){
        printf("%d ",temper[i]);
    }
     */
    int lag[MAX] = {0};
    int new_array_len = 0;
    for(int u = 0; u < 50; u++){
        if(temper[u] != INF && lag[temper[u]] == 0){
            new_array[new_array_len] = temper[u];
            new_array_len = new_array_len + 1;
            lag[temper[u]] = 1;
        }
    }
    
    int temp;
    //sort
    for(int i=0; i<10-1; i++){
        for(int j=0; j<10-1-i; j++){
            if(new_array[j] > new_array[j+1]){
                temp = new_array[j];
                new_array[j] = new_array[j+1];
                new_array[j+1] = temp;
            }
        }
    }
    /*
    printf("new_array is follows");
    
    for(int i = 0 ;i<10;i++){
        printf("%d ",new_array[i]);
    }
    printf("\n");
     */
    
    for(int j = flag1 + 3; j < flag2; j++){
        for(int i = 0; i < 10; i++){
            if(s[j][0] == new_array[i]){
                s[j][0] = i;
               
            }
            if(s[j][1] == new_array[i]){
                s[j][1] = i;
               
            }
        }
    }
    for(int j = flag1 + 3; j < flag2; j++){       //skip the delay and throughput to the matrix imformation
        x = s[j][0];
        y = s[j][1];
        num = s[j][2];
        MapMatrix[x][y] = num;
        MapMatrix[y][x] = num;
    }
	

	for (int r = 0; r <= max; r++)
		MapMatrix[r][r] = 0;    //set the distance start from r to r be 0
	/*
    for(int i=0;i<Vexnum;i++){
        for(int j=0;j<Vexnum;j++){
            printf("%d ",MapMatrix[i][j]);
        }
        printf("\n");
    }
     */
	Graph* pG;

    if((pG=(Graph*)malloc(sizeof(Graph)))==NULL)
    	return NULL;
    memset(pG, 0, sizeof(Graph));

    pG->vexnum = Vexnum;
    for(int i = 0; i < pG->vexnum; i++){
        
    	pG->vexs[i] = new_array[i];
        vexb[i] = new_array[i];
    }
    /*
    for(int i =0;i<pG->vexnum;i++)
        printf("pG-Vexs is %d ",pG->vexs[i]);
    */

    for(int i = 0; i < pG->vexnum; i++)
        for(int j = 0; j < pG->vexnum; j++)
        	pG->matrix[i][j] = MapMatrix[i][j];
        
    
    
    for(int i = 0; i < pG->vexnum; i++)
        for(int j = 0; j < pG->vexnum; j++)
        	if(i!=j && pG->matrix[i][j] != INF)
        		pG->edgnum++;
        
    
    pG->edgnum /= 2;

    return pG;
}




int main(int argc, char const *argv[])
{
    FILE *in= fopen("map.txt", "r");
    int i=0,j=0,m=0,n=0;
          
    while ( fgets(buf[i], sizeof(buf[i]), in) != NULL)
    {
        
                i++;
    }
        
    fclose(in);
          
 
    for (;m<i;m++)
        {
            n=0;
            j=0;
            for (;buf[m][n]!='\0';)
            {
                if (buf[m][n] ==' ')
                {
                    n++;
                    continue;
                }
                if ( n!=0 && buf[m][n-1] != ' ')
                {
                    n++;
                    continue;
                }
                if(buf[m][n]>='A' && buf[m][n]<='z'){
                    s[m][j]=buf[m][n]-48;
                }else{
                    s[m][j]=atoi( &buf[m][n] );
                }
                    j++;
                    n++;
            }
                  
          }

    //UDP code from Beej
    int sockfd;
	int rv;
	struct addrinfo hints;  // the struct addrinfo have already been filled with relevant info
	struct addrinfo *servinfo;
	struct sockaddr_storage their_addr;
	struct addrinfo *p;
	socklen_t addr_len;


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if ((rv = getaddrinfo(HOST, APortNum, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 0;
	}

	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("serverA: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("serverA: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "serverA: failed to bind socket\n");
		return 0;
	}
	freeaddrinfo(servinfo);
    
    
    printf("The Server A is up and running using UDP on port %s.\n", APortNum);
    /*
    printf("The Server A has constructed a list of %d maps:\n",NumMaps());
    printf("-----------------------------------------------------------------------\n");
    printf("Map ID                  Num Vertices                 Num Edges\n");
    
    for(int i = 0; i < 100; i++){
        char id;
        int numV;
        int numE;
        if(s[i][0] >=17 && s[i][0] <= 42){
            id = s[i][0];
            char c = id + 48;
            numV = NumVertices(c);
            numE = NumEdges(c);
            printf("%c                          %d                            %d\n", c, numV, numE );
            
        }
        
    }
    printf("----------------------------------------------------------------------\n");

    
    
    int SourceVertex = 6;
    
    for(int b = 0; b <10; b++){
        if(SourceVertex == new_array[b]){
            SourceVertex = b;
            
        }
    }
    
    Graph *pG;

    pG = creatgraph('B');
    for(int b = 0; b <10; b++){
        if(SourceVertex == new_array[b]){
            SourceVertex = b;
            
        }
    }
    dijkstra(*pG, SourceVertex, prev, dist);
    
*/
    
	while(1){
		addr_len = sizeof their_addr;
        FILE *in= fopen("map.txt", "r");
           int i=0,j=0,m=0,n=0;
                 
           while ( fgets(buf[i], sizeof(buf[i]), in) != NULL)
           {
               
                       i++;
           }
               
           fclose(in);
                 
        
           for (;m<i;m++)
               {
                   n=0;
                   j=0;
                   for (;buf[m][n]!='\0';)
                   {
                       if (buf[m][n] ==' ')
                       {
                           n++;
                           continue;
                       }
                       if ( n!=0 && buf[m][n-1] != ' ')
                       {
                           n++;
                           continue;
                       }
                       if(buf[m][n]>='A' && buf[m][n]<='z'){
                           s[m][j]=buf[m][n]-48;
                       }else{
                           s[m][j]=atoi( &buf[m][n] );
                       }
                           j++;
                           n++;
                   }
                         
                 }




        
        
        char map_id;
        long SourceVertex;
        int propspeed;
        int transpeed;
//From AWS server
        recvfrom(sockfd, (char*)&map_id, sizeof map_id , 0,(struct sockaddr *)&their_addr, &addr_len);
        recvfrom(sockfd, (char *)&SourceVertex, sizeof SourceVertex , 0,(struct sockaddr *)&their_addr, &addr_len);

        printf("The Server A has constructed a list of %d maps:\n",NumMaps());
        printf("-----------------------------------------------------------------------\n");
        printf("Map ID                  Num Vertices                 Num Edges\n");
    
        for(int i = 0; i < 100; i++){

            char id;
            int numV;
            int numE;
            if(s[i][0] >=17 && s[i][0] <= 42){
                id = s[i][0];
                char c = id + 48;
                numV = NumVertices(c);
                numE = NumEdges(c);
                printf("%c                          %d                            %d\n", c, numV, numE );
            
            }
        
        }
        printf("----------------------------------------------------------------------\n");
        
        printf("The server A has recieved input for finding shortest paths: starting vertex %ld of map %c\n", SourceVertex, map_id);
//compute and send to AWS server
        int prev[MAX] = {0};
        int dist[MAX] = {0};
        Graph *pG;

        pG = creatgraph(map_id);
        for(int b = 0; b <10; b++){
            if(SourceVertex == new_array[b]){
                SourceVertex = b;
                
            }
        }
        sv = SourceVertex;
        dijkstra(*pG, SourceVertex, prev, dist);
        //printf("The Vexnum is %d\n", Vexnum);
        sendto(sockfd, (char*)&Vexnum, sizeof Vexnum, 0, (struct sockaddr *)&their_addr, addr_len);
        sendto(sockfd, (char*)&vexb, sizeof vexb, 0, (struct sockaddr *)&their_addr, addr_len);
        sendto(sockfd, (char*)&dist, sizeof dist, 0, (struct sockaddr *)&their_addr, addr_len);
        sendto(sockfd, (char*)&sv, sizeof sv, 0, (struct sockaddr *)&their_addr, addr_len);
        
        printf("The Server A has sent shortest paths to AWS.\n");
        
        propspeed = PropSpeed(map_id);
        transpeed = TranSpeed(map_id);
        //printf("The propspeed is %d\n", propspeed);
        sendto(sockfd, (char *)& propspeed, sizeof propspeed, 0, (struct sockaddr *)&their_addr, addr_len);
        sendto(sockfd, (char *)& transpeed, sizeof transpeed, 0, (struct sockaddr *)&their_addr, addr_len);
        
       
    }

    
}
