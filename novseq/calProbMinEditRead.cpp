#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

char **listClones;
char **cloneSeq;
float *sumProb;
int *minEditDist;
int numClones;
int findCloneId(char*);
const int max_read_length = 150;

void readClones(FILE *fp)
{
    //  printf("L15\n");
    int cloneCount=0;
    char temp[250];
    char temp_seq[250];
    int return_value;

    return_value = fscanf( fp, "%i\n", &numClones);
    if( return_value != 1)
    {
        perror( "Error on fscanf");
        exit( 1);
    }

  listClones=(char **) malloc(numClones*sizeof(char *));
  sumProb=(float *) malloc(numClones*sizeof(float));
  minEditDist=(int *) malloc(numClones*sizeof(int));
  cloneSeq=(char **) malloc(numClones*sizeof(char *));

	for (int count=0; count<numClones; count++)
    {
      listClones[count]=(char *) malloc(50*sizeof(char));
      sumProb[count]=0;
      minEditDist[count]=5;
    	cloneSeq[count]=(char *) malloc(max_read_length*sizeof(char));
//	strcpy(cloneSeq[count], "QQ");
		}
  while (fscanf(fp,"%s\n",temp)!=EOF)
    {
      strcpy(listClones[cloneCount], temp);
     // strcpy(cloneSeq[cloneCount], temp_seq);
			cloneCount++;
    }
  //printf("L34\n");
}

void readAlignments(FILE *fp)
{
  // printf("L39\n");
  char *cloneName;
  float probDist;
  int editDist;
  int cloneId;
	char seq[max_read_length];
  cloneName=(char *) malloc(50*sizeof(char));
 long long int count=0;
  while(fscanf(fp,"%s\t%i\t%g\t%s\n", cloneName, &editDist, &probDist, seq)!=EOF)
    {

		count++;
      //  printf("%s\n", cloneName);
     cloneId=findCloneId(cloneName);
//	printf("%d\n", count%10);
	//if ((count%1000)==0)
 //     		printf("%s %i %li\n", cloneName, cloneId,count);
      sumProb[cloneId]=sumProb[cloneId]+probDist;
      if (minEditDist[cloneId]>editDist)
	  minEditDist[cloneId]=editDist;
		strcpy(cloneSeq[cloneId], seq);

	//count++;
    }
  //printf("L54\n");
}

int findCloneId(char *cloneName)
{
  //printf("L51\n");
  int minId=0;
  int maxId=numClones-1;
  int middleId=numClones/2;
  //printf("L55\n");
  if (strcmp(cloneName, listClones[minId])==0)
    return minId;
  //printf("L58\n");
  if (strcmp(cloneName, listClones[maxId])==0)
    return maxId;
  //printf("L61\n");
  while (strcmp(cloneName, listClones[middleId])!=0)
    {
      if (strcmp(cloneName, listClones[middleId])>0)
	{
	  minId=middleId;
	  middleId=(minId+maxId)/2;
	}
      else if (strcmp(cloneName, listClones[middleId])<0)
	{
	  maxId=middleId;
	  middleId=(minId+maxId)/2;
	}
      if (strcmp(cloneName, listClones[minId])==0)
	return minId;
      if (strcmp(cloneName, listClones[maxId])==0)
	return maxId;
      //printf("%s %s %s %s %i\n",listClones[minId].cloneName, listClones[maxId].cloneName, listClones[middleId].cloneName, cloneName, middleId);
    }
  return middleId;


}


void outputTheReads()
{
  printf("%i\n", numClones);
  for (int count=0; count<numClones; count++)
    {
      printf("%s\t%i\t%g\t%s\n", listClones[count], minEditDist[count],sumProb[count], cloneSeq[count]);
    }
}

int main(int argv, char *argc[])
{
	FILE *fp, *fp2;
  fp=fopen(argc[1],"r");
  fp2=fopen(argc[2],"r");
  readClones(fp);
 // printf("L94\n");
  readAlignments(fp2);
//  printf("L96\n");
 outputTheReads();
}
