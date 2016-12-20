/* Program to find the modularity of a graph, conisidering two text files as input, 
   one that contains the graph data, other contains the clusters/Communities */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX 50


typedef struct adjacencylist
{
	int label;
	struct adjacencylist *next;
}node;

typedef struct vertexlist
{
	int degree;
	node *head,*end;
}vertexset;

typedef struct community
{
	node *head,*end;
}module;

module *module_set;	/*Contains the list of nodes in each community */
vertexset *vertex;	/*Holds the head,end and degree of each vertex */
int no_of_nodes;	/*Takes an input from user, the number of nodes */
int no_of_clusters;	/* Keeps track of the no of communities entered */
int **edge_list,no_of_edges;



void insert(int,int);
node *create_new_node(int);
void display_adjacencylist();
void read_clusters(FILE *);
void push_into_cluster(int,int);
int in_cluster(int,int,int);
int calculate_cluster_degree(int);
void calculate_modularity(void);
void get_edges_and_nodes(FILE *);

int main(int argc, char **argv)
{
	char filename[MAX];
	int node1,node2,i;

	
	FILE *fp;
	
	//printf("\nEnter the filename which contains the edge list (with extension) : ");
	//gets(filename);
strcat(filename,argv[1]);
	fp=fopen(filename,"r");
	
	/* Call the get_edges_and_nodes() function that calculates the no of edges and nodes in the graph and puts it into the
		variables, no_of_edges and no_of_nodes respectively. */
	
	get_edges_and_nodes(fp);
	
	/* Creating the room for the adjacency List */

	vertex=(vertexset *)calloc(no_of_nodes,sizeof(vertexset));

	/* Create Space for the Edge list */
	edge_list=(int **)malloc(sizeof(int *)*no_of_edges);
	for(i=0;i<no_of_edges;i++)
		edge_list[i]=(int *)malloc(sizeof(int)*2);


	/* Read the edge list from the file and make an equivalent adjacency list */
	
	i=0;
	rewind(fp);
	while(fscanf(fp,"%d",&node1)>0)
	{
		fscanf(fp,"%d",&node2);
		
		insert(node1,node2);
		insert(node2,node1);
		edge_list[i][0]=node1;
		edge_list[i++][1]=node2;
		
	}
	fclose(fp);

	//printf("\n\nThe corresponding adjacency List : \n\n");
	//display_adjacencylist();

	//printf("\n\nEnter the filename that contains the communities(Clusters) : ");
	//gets(filename);
char filename1[MAX] = "";
strcat(filename1,argv[2]);
	fp=fopen(filename1,"r");
	read_clusters(fp);	/* Function to read the file that contains the cluster data */

	calculate_modularity();


	return 0;
}

void insert(int target,int label)
{
	node *newNODE;
	newNODE=create_new_node(label);
	
	if(vertex[target].head==NULL)
	{
		vertex[target].head=newNODE;
		vertex[target].end=newNODE;
	}

	else
	{
		(vertex[target].end)->next=newNODE;
		(vertex[target].end)=newNODE;
	}

	(vertex[target].degree)++;
}

node *create_new_node(int key)
{
	node *temp;

	temp=(node *)malloc(sizeof(node));
	temp->label=key;
	temp->next=NULL;

	return temp;
}


void display_adjacencylist()
{
	int i=0;
	node *save;

	while(i<no_of_nodes)
	{
		save=vertex[i].head;
		printf(" %d  %d ",i,vertex[i].degree);
		while(save!=NULL)
		{
			printf(" %d ",save->label);
			save=save->next;
		}
		i++;
		printf("\n");
	}
}

void read_clusters(FILE *fp)
{
	fscanf(fp,"%d",&no_of_clusters);
	
	/* Create room for the module set */
	module_set=(module *)calloc(no_of_clusters,sizeof(module));

	int i=0,lb;

	for(i=0;i<no_of_clusters;i++)
	{
		
		while(fscanf(fp,"%d",&lb)>0)
		{
			if(lb==-1)	break;
			else
				push_into_cluster(i,lb);
		}
	}
}

void push_into_cluster(int target,int label)
{
	node *newNODE;
	newNODE=create_new_node(label);

	if(module_set[target].head==NULL)
	{
		module_set[target].head=newNODE;
		module_set[target].end=newNODE;
	}
	else
	{
		module_set[target].end->next=newNODE;
		module_set[target].end=newNODE;
	}
}
void calculate_modularity()
{
	
	int i,j;
	float modularity=0.0,lc=0,dc;
float part1=0.0,part2=0.0;
	node *save,*check_node,*temp;
	
	for(i=0;i<no_of_clusters;i++)
	{
		for(j=0;j<no_of_edges;j++)
			if(in_cluster(i,edge_list[j][0],edge_list[j][1]))
				lc++;
		dc=(float)calculate_cluster_degree(i);
//printf("Part2 %f\n",(dc*dc));
dc=dc*dc;
dc=dc/(double)no_of_edges;
dc=dc/(double)no_of_edges;
//printf("Part1 %f\t",lc);
//printf("Part2 %f\n",(dc*dc));
		modularity+= (lc/(float)no_of_edges)-((dc)/(float)(4));
	//printf("comm %d %f \n",i,modularity);
		lc=0;
	}
	printf("\nThe Modularity of this specific partioning is : %0.3f \n",modularity);
}

int in_cluster(int target,int node1,int node2)
{
	node *save=module_set[target].head;
	int flag=0;

	while(save!=NULL)
	{
		if(save->label==node1 || save->label==node2)
			flag++;
		save=save->next;
	}
	
	if(flag==2)	return 1;
	else		return 0;
}
int calculate_cluster_degree(int target)
{
	node *save=module_set[target].head;
	int deg_sum=0;
	
	while(save!=NULL)
	{
		deg_sum+=vertex[save->label].degree;
		save=save->next;
	}
	return deg_sum;
}

void get_edges_and_nodes(FILE *fp)
{	
	int temp_node;

	no_of_nodes=-1;
	no_of_edges=0;
	
	while(fscanf(fp,"%d",&temp_node)>0)
	{
		if(temp_node>no_of_nodes)
			no_of_nodes=temp_node;
		no_of_edges++;
	}	
	//no_of_nodes++;		/* labels start from zero */
	no_of_edges/=2;		/* As per the while loop condition, the loop runs 2E times */
printf("no of nodes : %d\n",no_of_nodes);
printf("no of edges : %d",no_of_edges);
}
