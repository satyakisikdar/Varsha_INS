
 /*The program assumes that the node label starts from 1 and the graph has no missing nodes.*/ 
#include <stdlib.h> 
#include <stdio.h> 
#include <time.h> 
#include <string.h> 
#define MAX 50

typedef struct vertex
{
    int label; 
    float score; 
    int community; 
    int parent; 
    int category;  // 1-->Broker 2-->Community Node 3--> Broker node turned community node
}node; 
typedef struct comm
{
    int value; 
    int label; 
}comm; 

typedef struct record
{
    int label; 
    int category; 
    int mark1; 
    int influenced; 
    int traversed; 
    int deg_sum; 
    int intra_edges; 
    int nodes; 
}record; 

typedef struct brok
{
    int label; 
    int parent; 
}brok; 

typedef struct linked_list
{
    int label; 
    int mark; 
    int test; 
    struct linked_list *ptr; 
}link; 

typedef struct community1
{
	int deg; 
	int iedges; 
	float m; 
	int count; 
}community1; 

community1 *profile; 
float val = 0.0; 
node *Queue, *Broker; 
int *degC, *list, *list1, *sort1, no_of_overlap = 0; 
link **Overlap; 
int abc = 0, flag1 = 1; 
int max_label, count_of_comm, *degree, **adlist, noOfComm, **arraySet; 
record *labels; 
void maxCMT(int); 
void convergence(); 
void maxLabel(FILE *); 
void degreeOfNodes(FILE *); 
void adjacencyList(FILE *); 
void community_detection(float); 
float calculate_score(int); 
int print(); 
int neighbour(int, int); 
int no_of_comm1 = 0, min_deg_node; 
void generateNodes(); 
int **arrayL; 
int cd = 0; 
char filename[50] = ""; 

int main( int argc, char **argv)
{
	FILE *fp; 
	float t; 
	int i; 
	if(argc != 3)
	{
		printf("Wrong command line args"); 
		exit(0); 
	}
	else
	{
		strcat(filename, argv[1]); 
	}
	// char *filename = "karate.txt";
	fp = fopen(filename, "r"); 
	if(fp == NULL)
	{
		printf("File does not exist !!"); 
		exit(0); 
	}
	maxLabel(fp); 
	fclose(fp); 
	fp = fopen(filename, "r"); 
	degreeOfNodes(fp); 
	fclose(fp); 
	labels = (record *)calloc(max_label + 2, sizeof(record)); 
    fp = fopen(filename, "r"); 
	adjacencyList(fp); 
	fclose(fp); 

	t = atof(argv[2]);
	// t = 0.3;
	Queue = (node *)calloc(max_label + 2, sizeof(node)); 
	Broker = (node *)calloc(max_label + 2, sizeof(node)); 
    Overlap = (link **)calloc(max_label + 2, sizeof(link *)); 
	
	node start; 
	start.label = min_deg_node; 
	start.score = 0; 
	start.parent = start.label; 
	start.community = min_deg_node; 
	start.category = 1;  // broker
	labels[start.label].influenced = 1; 

	Broker[0] = start; 
	labels[start.label].label = start.label; 
	labels[start.label].category = 1; 
	community_detection(t); 
	no_of_comm1 = print();
	convergence(); 
	free(labels); 
    
	generateNodes(); 
	free(adlist[0]); 
	for(i = 1; i <= max_label + 1; i++)
	{
	    link *temp = Overlap[i]; 
	    while(temp != NULL)
	    {
            link *p = temp; 
            temp = temp->ptr; 
            free(p); 
	    }

        free(adlist[i]); 
	}
	free(Overlap); 
   	free(adlist); 
	return 0; 
}

 /*Finds the maximum no. of nodes present in the graph*/ 

void maxLabel(FILE *f)
{
	max_label = 0; 
	int i, j; 
	while(fscanf(f, "%d %d", &i, &j) != EOF)
	{
    	if(i > max_label && i > j)
 		   max_label = i; 
    	else if(j > max_label && j > i)
			max_label = j; 
	}
}

 /*Calculates the degree of the nodes present in the graph*/ 

void degreeOfNodes(FILE *f)
{
    int n1, n2; 
    degree = (int*)calloc((max_label + 2), sizeof(int)); 
    while(fscanf(f, "%d %d", &n1, &n2) != EOF)
    {
        degree[n1] ++; 
        degree[n2] ++; 
    }
}

 /*Calculates the adjacency list of the graph and minimum degree node present*/ 
void adjacencyList(FILE *f)
{
	int n1, n2, *flag, i, min = degree[1]; 
	min_deg_node = 1; 
    
    adlist = (int**)calloc((max_label + 2), sizeof(int*)); 
	  // char name[50] = "Adjlist_"; 
	  // strcat(name, filename); 
	  // FILE *ad; 
	  // ad = fopen(name, "w+"); 
	for(i = 1; i <= max_label; i++)
	{
		if(degree[i] < min && i != 0)
		{
			min = degree[i]; 
			min_deg_node = i; 
		}
        adlist[i] = (int*)calloc((degree[i] + 2), sizeof(int)); 
		adlist[i][0] = degree[i]; 
		// printf("Adj list [%d][0] = %d\n", i, adlist[i][0]); // working
		labels[i].label = i; 
		labels[i].category = 0; 
		labels[i].traversed = 0; 
		labels[i].deg_sum = degree[i]; 
		labels[i].intra_edges = 0; 
	}
	
	// printf("\nMaxlabel = %d\n", max_label);  // working
	
	// printf("\nMin degree = %d", min_deg_node);  // working
	
	// for (i = 1; i <= max_label; i ++)
	// {
	// 	printf("%d: ", i); 
	// 	for (j = 0; j <= degree[i]; j ++)
	// 	   	printf("%d ", adlist[i][j]); 
	// 	printf("\n"); 
	// }
	
	flag = (int*)calloc((max_label + 2), sizeof(int)); 
	
	while(fscanf(f, "%d %d", &n1, &n2) != EOF)
	{
		adlist[n1][++ flag[n1]] = n2; 
    	adlist[n2][++ flag[n2]] = n1; 
    	cd ++; 
	}
	printf("The adjacency list is created :- n = %d, m = %d\n", max_label, cd); 
    free(flag); 
	free(degree); 
}



void community_detection(float t)
{
	int i, rear = -1, front = -1, top = 0; 
	brok *bro; 
	link * temp; 
	int z = 0; 
	bro = (brok *)calloc(max_label + 2, sizeof(brok)); 
	int b = 0; 
	int k, no_of_nodes; 
	int sum = 0, intra = 0, count = 0; 
	
	count_of_comm = 0; 
	node start, neighbour;

	
	while(front < rear || top >= 0)
	{
		if(front < rear)
    	{
    		// printf("\nFrom Queue\n");
       		start = Queue[++front]; 
        }
    	else
    	{
    		// printf("\nFrom Stack\n");
       		start = Broker[top --]; 
       		b++;  // Broker node
			no_of_nodes = 0; 
			sum = labels[start.label].deg_sum; 
			intra = labels[start.label].intra_edges; 
			temp = (link*)malloc(sizeof(link)); 	   			
			temp->label = 0; 
			temp->mark = 0; 
			temp->test = 0; 
	    	temp->ptr = NULL; 
    		Overlap[start.label] = temp; 
    	}

    	labels[start.label].traversed = 1; 
    	if(start.category == 1)
    	{
    		bro[z].label = start.label; 
    		bro[z].parent = start.parent; 
    		z++; 
    	}
    	for(i = 1; i <= adlist[start.label][0]; i++)
    	{
        	labels[adlist[start.label][i]].influenced = 1; 
    	}
    
    	for(i = 1; i <= adlist[start.label][0]; i++)
    	{
        	if(labels[adlist[start.label][i]].traversed == 0)
        	{
	    		neighbour.parent = start.label; 
	    		neighbour.label = adlist[start.label][i]; 
	    		neighbour.community = start.label; 
				labels[neighbour.label].traversed = 1; 
	    		neighbour.score = calculate_score(neighbour.label); 
	    		if(neighbour.score < t)
	    		{
	        		neighbour.category = 1;  // Broker
	       			labels[neighbour.label].label = neighbour.label;  // broker
	        		labels[neighbour.label].category = 1; 
	        		Broker[++top] = neighbour; 
	    		}
                else
                {
					if(labels[labels[start.label].label].mark1 == 0)
					{
						labels[labels[start.label].label].mark1 = -1; 
						count_of_comm++; 
						Overlap[start.label]->mark = count_of_comm; 
					}
					count = 0; 
					for(k = 1; k <= adlist[adlist[start.label][i]][0]; k++)
					{
						if(labels[adlist[adlist[start.label][i]][k]].label == labels[start.label].label)
							count++; 
					}
					sum += adlist[adlist[start.label][i]][0]; 
					intra += count; 
                	labels[neighbour.label].label = labels[start.label].label; 
                	labels[neighbour.label].category = 2; 
					labels[labels[start.label].label].deg_sum = sum; 
					labels[labels[start.label].label].intra_edges = intra; 
                	neighbour.category = 2;  // Community node
					no_of_nodes++; 
					labels[labels[start.label].label].nodes = no_of_nodes; 
                	Queue[++rear] = neighbour; 
					temp = (link*)malloc(sizeof(link)); 
       				temp->label = count_of_comm; 
					temp->mark = 0; 
					temp->test = 0; 
        			temp->ptr = NULL; 
        			Overlap[neighbour.label] = temp; 
                }
           	}
		}
	}
	free(Queue); 
    free(Broker); 
	printf("\nThe number of brokers are: %d", z); 
    	
	printf("\n\nNo. of COMMUNITIES detected before mod max : %d\n", count_of_comm); 
 
    
	  // printf("\n\nTime taken = %f\n", (float)(end-begin) / CLOCKS_PER_SEC); 

    profile = (community1 *)calloc(count_of_comm + 1, sizeof(community1)); 
	float dc; 
	
	for(i = 1; i <= max_label; i++)
	{
		if(labels[i].category == 1)
		{
			if(labels[i].nodes > 0)
			{
				profile[Overlap[i]->mark].count = labels[i].nodes; 
				profile[Overlap[i]->mark].iedges = labels[i].intra_edges; 
				profile[Overlap[i]->mark].deg = labels[i].deg_sum; 
				dc = labels[i].deg_sum * labels[i].deg_sum; 
				dc = dc / (float)cd; 	
				dc = dc / (float)cd; 
				dc = dc / (float)(4); 
				profile[Overlap[i]->mark].m = (labels[i].intra_edges / (float) cd) - dc; 
			}
		}
	}
	
    free(bro); 
}
 /*Calculates score*/ 

float calculate_score(int index)
{
    int i, c = 0; 
    for(i = 1; i <= adlist[index][0]; i++)
    {
        if(labels[adlist[index][i]].influenced == 1)
            c++; 
    }
    return ((float)c / adlist[index][0]); 
}


  // Calculates the number of edges of the a broker in a community


 /*Covergence Algorithm*/ 

void convergence()
{
   	int i; 
 /* Placing marked nodes in communities to which maximum neighbours belong */ 
	int check_loop = 1, vertex = 0; 
	while(check_loop == 1)
	{
		check_loop = 0; 
		for(i = 1; i <= max_label; i++)
		{
			if(labels[i].category == 1)
			{
				vertex = i; 
				maxCMT(vertex); 
				if(labels[vertex].category == 1)
					check_loop = 1; 
			}
		}
	}
}



 /*Calculates the maximum label*/ 
void maxCMT(int n)
{
	int i; 
	float max = 0.0, mod_change = 0.0; 
	float dc1; 
	int comm, flag_val = 0, maxlabel; 
	float new_mod = 0.0, max_m = 0.0; 
   	link *temp; 
   	int *flag = (int *)calloc(count_of_comm + 1, sizeof(int)); 
	int *flag1 = (int *)calloc(count_of_comm + 1, sizeof(int)); 
	for(i = 1; i <= adlist[n][0]; i++)
	{
		if(labels[adlist[n][i]].category != 1)
		{
			temp = Overlap[adlist[n][i]]; 
			while(temp != NULL)
			{
				flag[temp->label]++; 
				flag1[temp->label]++; 
				temp = temp->ptr; 
			}
			  // free(temp); 
		}
    }

    for(i = 1; i <= adlist[n][0]; i++)
    {
		if(labels[adlist[n][i]].category != 1)
		{
			temp = Overlap[adlist[n][i]]; 
			while(temp != NULL)
			{
				if(flag[temp->label] != -1)
				{
					dc1 = profile[temp->label].deg+adlist[n][0]; 
					dc1 = dc1*dc1; 
					dc1 = dc1 / (float)cd; 	
					dc1 = dc1 / (float)cd; 
					dc1 = dc1 / (float)(4); 
					new_mod = ((profile[temp->label].iedges+flag[temp->label]) / (float)cd)-dc1; 
					new_mod = new_mod-profile[temp->label].m; 
					if(new_mod > mod_change)
					{
						mod_change = new_mod; 
						comm = temp->label; 
						flag_val = 1; 
					}
					if((flag[temp->label]) > max)
					{
						max = flag[temp->label]; 
						maxlabel = temp->label; 
						max_m = new_mod; 						
					}
					flag[temp->label] = -1; 
				}
				temp = temp->ptr; 
			}
		  // free(temp); 
		}
	}
	free(flag); 			

	if(flag_val == 1)
	{
		profile[comm].m += mod_change; 
		profile[comm].iedges += flag1[comm]; 
		profile[comm].count += 1; 
		profile[comm].deg += adlist[n][0]; 
		labels[n].category = 3; 
		Overlap[n]->label = comm; 
	}
	else
	{
		profile[maxlabel].m += max_m; 
		profile[maxlabel].iedges += flag1[maxlabel]; 
		profile[maxlabel].count += 1; 
		profile[maxlabel].deg += adlist[n][0]; 
		labels[n].category = 3; 
		Overlap[n]->label = maxlabel; 
	}
	free(flag1); 
}

 /*Prints the communities*/ 
int print()
{
    int i, j; 
    link *temp, *temp1, *temp2; 
	int cVal = 1; 
	FILE *fp; 
	list1 = (int *)calloc(count_of_comm + 2, sizeof(int)); 
	int *arr = (int *)calloc(count_of_comm + 2, sizeof(int)); 
	char mapping[50] = "Mapping"; 
	char membership[50] = "Membership_"; 
	strcat(mapping, filename); 
	strcat(membership, filename); 
	FILE *fp123 = fopen(mapping, "w+"); 
	fp = fopen(membership, "w+"); 
  	for(i = 1; i <= max_label; i++)
   	{
		fprintf(fp, "%d", i); 
       	temp = Overlap[i]; 
		  //  if(temp->test != 0)
		  //  	index = temp->test; 
       	while(temp != NULL)
		{
			if(arr[temp->label] == 0)
			{
      			arr[temp->label] = cVal; 
				fprintf(fp123, "%d  %d  \n", temp->label, cVal); 
				fprintf(fp, " %d", arr[temp->label]); 
	  			temp->label = cVal; 
				list1[temp->label]++; 
				cVal++; 
			}
			else
       		{
				fprintf(fp, " %d", arr[temp->label]); 
           		temp->label = arr[temp->label]; 
				list1[temp->label]++; 
       		}
       		temp = temp->ptr; 
		}
		fprintf(fp, " %d\n", -1); 
    }
	
	fclose(fp); 
	  // char filename3[50], c1, c2; 
	free(arr); 
	int *arr1 = (int *)calloc(count_of_comm + 2, sizeof(int)); 
	char filename3[50] = ""; 

	if(1)
	{
		strcat(filename3, filename); 
		strcat(filename3, "_cover_i.txt"); 
		fp = fopen(filename3, "w+"); 
		fprintf(fp, "%d\n", count_of_comm); 
    	for(i = 1; i <= max_label; i++)
		{
    		temp2 = Overlap[i]; 
			while(temp2 != NULL)
			{
    			while(arr1[temp2->label] != -1)
				{
					arr1[temp2->label] = -1; 
    	    		for(j = i; j <= max_label; j++)
    	    		{
   	        			temp1 = Overlap[j]; 
   	        			while(temp1 != NULL)
	        			{
            				if(temp2->label == temp1->label)
            				{
                				fprintf(fp, "%d ", j); 
            				}
            				temp1 = temp1->ptr; 
	       				}
       	   			}
					fprintf(fp, "-1\n"); 
				}
       	   		temp2 = temp2->ptr; 
			}
		}
		fclose(fp); 
	}

	free(arr1); 
    return (cVal-1); 
}

void generateNodes()
{
	link * temp, *temp1; 
	int i, j; 
	FILE *fpl; 
	char name[50] = "louvain_input_"; 
	strcat(name, filename); 
	fpl = fopen(name, "w+"); 
	
    arrayL = (int**)calloc(no_of_comm1 + 2, sizeof(int*)); 
    for(i = 1; i <= no_of_comm1 + 1; i ++)
    {
        arrayL[i] = (int*)calloc((i+2), sizeof(int)); 
    }
    for(i = 1; i <= max_label; i++)
  	{
		Overlap[i]->mark = -999; 
        for(j = 1; j <= adlist[i][0]; j++)
      	{
			if(Overlap[adlist[i][j]]->mark != -999)
			{
               	temp = Overlap[i]; 
				temp1 = Overlap[adlist[i][j]]; 
				while(temp != NULL)
				{
					while(temp1 != NULL)
               		{
						if(temp->label != temp1->label)
						{
                   			  // if((temp->label) > (temp1->label) && arrayL[temp->label][temp1->label] == 0)
							if((temp->label) > (temp1->label))
                   			{
								if(arrayL[temp->label][temp1->label] == 0)
								{
									printf("%d %d\n", temp->label, temp1->label);
									fprintf(fpl, "%d %d\n", temp->label, temp1->label); 
								}
                       			arrayL[temp->label][temp1->label]++; 
                   			}
                                        		  // else if((temp->label) < (temp1->label) && arrayL[temp1->label][temp->label] == 0)
							else if((temp->label) < (temp1->label))
                      		{
								if(arrayL[temp1->label][temp->label] == 0)
								{
									fprintf(fpl, "%d %d\n", temp->label, temp1->label); 
									printf("%d %d\n", temp->label, temp1->label); 
	
								}
               	       			arrayL[temp1->label][temp->label]++; 
                   	 		}
                   		}
						temp1 = temp1->ptr; 	
					}
					temp = temp->ptr; 
				}
			}
		}
	}
				
    printf("\n\n"); 
 	
	fclose(fpl); 

	for(i = 0; i <= no_of_comm1 + 1; i++)
		free(arrayL[i]); 
	free(arrayL); 
	free(list1); 
}
