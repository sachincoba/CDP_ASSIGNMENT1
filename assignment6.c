#include <stdio.h>
#include <stdlib.h>
/*          ADJACENCY MATRIX                            */
int source,V,E,a,time,visited[20],G[20][20];
void DFS(int i)
{
    int j;
    visited[i]=1;
    printf(" %d ",i+1);
    for(j=0;j<V;j++)
    {
        if(G[i][j]==1 && visited[j]==0)
           {

           //printf("%d\n",i );
           	//printf("%d\n",j );
            DFS(j);


           }
    }
}
int main()
{
    int i,j,v1,v2,number=0,character=0;
    char* s;
    scanf("%d",&V);
    scanf("%d",&E);
    scanf("%d",&a);
    if(a==0)
    	a=1;
    else
    	a=0;
    for(i=0;i<V;i++)
    {
        for(j=0;j<V;j++)
            G[i][j]=0;
    }

    for(i=0;i<E;i++)
    {
        scanf("%d%d",&v1,&v2);
        G[v1-1][v2-1]=1;
        if(a)
        	G[v2-1][v1-1]=1;

    }

    for(i=0;i<V;i++)
    {
        for(j=0;j<V;j++)
        	{//G[i][j]=G[j][i];
            printf(" %d ",G[i][j]);}
        printf("\n");
    }

    for (int i = 0; i < V; ++i)
    {
    	printf("%d " ,i+1 );
   		for (int j = 0; j < V; ++j)
   		{
   			if(G[i][j]){
   				//printf("%d\n",i+1 );
   				printf("%d ",j+1 );}
   		}
   		printf("\n");
    }
    for (int i = 0; i < 20; ++i)
    {
    	visited[i]=0;
    }
    scanf("%d",&source);
        DFS(source-1);
    return 0;
}
