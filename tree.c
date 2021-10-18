#include <stdio.h>
#include <stdlib.h>

int queue[20];
int front =0;
int capacity=20;
int rear=20-1;
int visited[20];
int adj[20][20];
void enqueue(int value)
{
	rear=(rear+1)%capacity;
	queue[rear]=value;
}

int dequeue()
{
	int value=queue[front];
	front=(front+1)%capacity;
	return value;
}

int empty()
{
	if(front>rear)
		return 0;
	else
		return 1;
}
void bfs(int s)
{
	enqueue(s);
	while(empty())
	{
		int m;
		m=dequeue();
		if(visited[m]==0)
		{
			printf("%d ",m );
			visited[m]=1;
		}
		for (int i = 0; i < 20; ++i)
		{
			if(adj[m][i]==1&&visited[i]==0)
			{
				enqueue(i);
			}
		}
	}
}

int main(int argc, char const *argv[])
{

	for (int i = 0; i < 20; ++i)
	{
		visited[i]=0;
	}

	for (int i = 0; i < 20; ++i)
	{
		for (int j = 0; j < 20; ++j)
		{
			adj[i][j]=0;
		}
	}

	int v,e,a,q,b,z;
	scanf("%d%d%d",&v,&e,&q);
	for (int i = 0; i < e; ++i)
	{
		scanf("%d%d",&a,&b);
		adj[a][b]=1;
		//adj[b][a]=1;
	}
	scanf("%d",&z);
	bfs(z);
	return 0;
}
