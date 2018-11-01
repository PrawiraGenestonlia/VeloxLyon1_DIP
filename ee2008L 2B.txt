#include <stdio.h>                                      // Include header file for printf
#define maxV 100                                        // Define maximum vertices to 100
 
int x, y, V, count=0, component=0;                      // Declare Variables
int g[maxV][maxV];                                      // Declare two dimensional array for the graph
int visit[maxV],start[maxV],end[maxV],sequence[maxV],check_cycle[maxV];   // Declare array
int parent[maxV];
 
void read_graph(void);                                  // Function Prototype
void print_graph(void);
void dfs(int);
void Sequence(void);
void Component(void);
void Parent(void);
void Edges(void);
void Cycle(void);
 
void main(void)                                         // Main program
{
    read_graph();                                       // call function to input graph adjacency matrix
    print_graph();                                      // call function to output graph adjacency matrix
    Sequence();                                         // call function to output sequence
    Component();                                        // call function to output connected vertices
    Parent();                                           // call function to output parent of vertex
   //Edges();                                            // call function to output tree-edges and back-edges
    //Cycle();
}
 
void read_graph(void)                                   // Fuction to read graph adjacency matrix
{
    int x, y, edge;
    printf("Input total number of vertices : ");
    scanf("%d", &V);
 
    while (V > maxV)
    {
        printf("Exceeded the maximum number of vertices permitted\n");
        printf("\nInput total number of vertices : ");
        scanf("%d", &V);
    }
 
    for(x=1;x<=V;x++)
        for(y=1;y<=V;y++)
            g[x][y]=0;
 
    for (y=1;y<=V; y++)
    {
        for (x=y+1; x<=V; x++)
        {
            printf("Coordinate [%d][%d]=",y,x);
            scanf("%d",&edge);
            g[x][y]=edge;
            g[y][x]=edge;
        }
    }
}
 
void print_graph(void)                                  // Funtion to print graph adjacency matrix
{
    int x, y;
    printf("\nAdjacency Matrix of Graph: \n");
    for (y=1; y<=V; y++)
    {
        printf("\n\n");
            for (x=1; x<=V; x++)
                printf(" %d",g[x][y]);
 
        printf("\n\n");
    }
}
 
void dfs(int x)                                         // Function to perform depth-first search
{
    int i;
    count=count+1;
    sequence[count]=x;
    visit[x] = 1;
 
    for (i=1; i<=V; i++)
    {
        if (g[x][i] == 1 && visit[i]==0)
        {
        parent[i]=x;
        dfs(i);
        }
    }
}
 
void Sequence(void)                                     // Function to acquire and print the sequence of vertices visited
{
    for(x=1;x<=V;x++)
    {
        if(visit[x]==0)
        {
            component = component + 1;
            start[component] = count+1;
            dfs(x);
            end[component]=count;
        }
    }
 
    printf("\nSequence Visited: ");
 
        for(x=1; x<=V; x++)
         printf("%d ",sequence[x]);
 
    printf("\n");
 
}
 
void Component(void)                                     // Function to acquire and print the vertices of connected components
{
    if(component == 1)
    {
        printf("\nVertices of Connected Components: ");
 
        for(y=start[1]; y<=end[1]; y++)
            printf("%d ",sequence[y]);
    }
 
    else
    {
        for(x=1; x<=component; x++)
        {
          printf("\ncomponnent %d is(vertex) ",x);
 
          for(y=start[x]; y<=end[x]; y++)
            printf("%d ",sequence[y]);
        }
    }
 
    printf("\n\n");
}
 
void Parent(void)                                         // Function to acquire and print the parent of each vertex
{
    int i;
    for(i=1;i<=V;i++)
    {
        if(parent[i]==0)
            printf("\nVertex %d has no parent\n",i);
        else
            printf("\nParent of vertex %d is %d\n",i,parent[i]);
    }
 
    printf("\n");
}
 
/*void Edges(void)                                           // Additional implementation,
{                                                          // Function to print tree edges and back edges of graph
 
    int k;
 
    printf("\nThe tree edges are (vertex to vertex): ");
    for(k=1;k<=V;k++)
    {
        if(parent[k]!=0)
        printf("(%d to %d), ",parent[k],k);
    }
 
    printf("\n");
    printf("\nThe back edges are (vertex to vertex): ");
    for(x=1;x<=V;x++)
        for(y=x;y<=V;y++)
            if(g[x][y]==1)
                if(x!=parent[y] && y!=parent[x])
                printf("(%d to %d), ",x,y);
                printf("\n");
}
void Cycle(void)
{
   int k,t,i;
 
    check_cycle[x]=x;
 
    for(i=1; i<=component; i++)
    {
    for(k=start[i]; k<=end[i]; k++)
        for(t=1;t<=V;t++)
        {
 
            if(check_cycle[t]==sequence[k])
            {
 
                printf("\nComponent %d has a back edge, hence it contains cycle ",i);
 
                goto loop;
            }
        }
 
    printf("\nComponent %d had no back edge, therefore there is no cycle ",i);
 
    loop: ;
    }
}*/