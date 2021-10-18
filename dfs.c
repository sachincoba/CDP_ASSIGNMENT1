#include <stdio.h>
#include <stdlib.h>

int stack[20];
int top=-1;
int visited[20];
int n=20;
int adj[20][20];
void push(int value)
{	
	stack[++top]=value;
}

int pop()
{
	return stack[top--];
}

int empty()
{
	if(top==-1)
		return 0;
	else
		return 1;
}



void dfs(int s)
{
	int m;
	push(s);
	while(empty())
	{
		m=pop();
		if(visited[m]==0)
		{
			printf("%d ",m );
			visited[m]=1;
		}
		for (int i = n; i >= 0; i--)
		{
			if(adj[m][i]==1&&visited[i]==0)
				push(i);
				//break;
		}
	}

}

int main(int argc, char const *argv[])
{

	for (int i = 0; i < 20; ++i)
{
	visited[i]=0;
}

for (int i = 0; i < n; i++)
{
	for (int j = 0; j < n; j++)
	{
		adj[i][j]=0;
		//adj[j][i]=1;
	}
}

	int v,e,a,b,q,z;
	scanf("%d%d%d",&v,&e,&q);
	for (int i = 0; i < e; i++)
	{
		scanf("%d%d",&a,&b);
		adj[a][b]=1;
		//adj[b][a]=1;
	}

	/*for (int i = 0; i < v; i++)
	{
		for (int j = 0; j <v; j++)
		{
			printf("%d ",adj[i][j] );
		}
		printf("\n");
	}*/
	scanf("%d",&z);
	dfs(z);
	return 0;
}
