/*
 *  convertToInvLR.cpp
 *
 *
 *  Created by Fereydoun Hormozdiari on 23/09/08.
 *  Copyright 2008 Simon Fraser University. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int numClones;
char **listClones;
float *listProb;
int *minEditDist;
const int max_read_length=150;

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

void readClones(FILE *fp)
{

    int cloneCount=0;
    char temp[250];
    float tempI;
    int tempI2;
    char strSeq[max_read_length];
    int return_value;

  return_value = fscanf( fp,"%i\n", &numClones);
  if( return_value != 1)
  {
      perror( "Error on fscanf");
      exit( 1);
  }
  listClones=(char **) malloc(numClones*sizeof(char *));
  listProb=(float *) malloc(numClones*sizeof(float));
  minEditDist=(int *) malloc(numClones*sizeof(int));
  for (int count=0; count<numClones; count++)
    {
      listClones[count]=(char *) malloc(250*sizeof(char));
      //      sumProb[count]=0;
    }
  while (fscanf(fp,"%s\t%i\t%g\t%s\n",temp, &tempI2, &tempI, strSeq)!=EOF)
    {
		//	printf("%s\t%i\t%g\t%s\n", temp, tempI2, tempI, strSeq);
      strcpy(listClones[cloneCount], temp);
      listProb[cloneCount]=tempI;
      minEditDist[cloneCount]=tempI2;
      cloneCount++;

    }
  //  printf("L79\n");
}



int main(int argv, char *argc[])
{
	FILE *fp1, *fp2;
	fp1=fopen(argc[1],"r");
	fp2=fopen(argc[2],"r");
	readClones(fp2);

	char seq[max_read_length];
	char cloneName[100];
	char chrName[100];
	char SVtype[100];
	char temp1[200],temp2[200], temp3[200], temp4[200], temp5[200];
	int firstCloneL, firstCloneR;
	int secondCloneL, secondCloneR;
	int tempI1, tempI2, tempI3, tempI4, tempI5;
	char orienFirstClone, orienSecondClone;
	int editDist;
	float avgPhred, prob;
	//printf("TT\n");
	while (fscanf(fp1,"%s\t%s\t%i\t%c\t%i\t%g\t%g\t%s", cloneName, chrName, &firstCloneL, &orienFirstClone, &editDist, &avgPhred, &prob, seq)!=EOF)
	  {

	//     printf("Here\n");
	    if ((float)prob/(float)listProb[findCloneId(cloneName)] > 0.1)
	      if  (minEditDist[findCloneId(cloneName)]==editDist)
		{
		  printf("%s\t%s\t%i\t%c\t%i\t%g\t%g\t%s\n",cloneName, chrName,firstCloneL,  orienFirstClone, editDist, avgPhred, (float)prob/(float)listProb[findCloneId(cloneName)], seq);

	      }
	  }
	//fclose(fp2);
}

