#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int maxSizeInst;
const int max_read_length=150;

const int maxNumAlignsInClusters=100000;

typedef struct aligns{
  int hitPos;
  char orient;
  char chroName[7];
  char cloneName[50];
	char cloneSeq[max_read_length];
  float prob;
  aligns *next;
}alignType;


typedef struct alignCluster{
  int hitPos;
  float prob;
  char orient;
  char cloneName[50];
	char cloneSeq[max_read_length];
}alignClusterType;


aligns** listOfOEA;
int chroSize;
int *listOfOEA_R;//has a R OEA that possition
int *listOfOEA_F;//has a F OEA that possition
char chroName[7];
//int chroSize;
int EndClusterR;
int EndClusterF;
alignCluster listAlignClusters[maxNumAlignsInClusters];
int numAlignsInCluster;

bool check_R(int, int);
bool check_F(int, int);

int readTheAligns(FILE *fp2)
{
  int pos;
  char orient;
  char chroName_T[7];
  aligns *newAlignEl;
  char cloneName[50];
	char cloneSeq[max_read_length];
  float prob;
  float qualAvg;
  int editDist;
  while(fscanf(fp2, "%s\t%s\t%i\t%c\t%i\t%g\t%g\t%s\n",cloneName, chroName_T, &pos, &orient, &editDist, &qualAvg, &prob, cloneSeq)!=EOF)
    {
      if (strcmp(chroName, chroName_T)==0)
	{
	  //	  p%s %s %i %c \n",cloneName, chroName_T, pos, orient);
		//fprintf(stdout,  "%s\t%s\t%i\t%c\t%i\t%g\t%g\n",cloneName, chroName_T, pos, orient, editDist, qualAvg, prob);

		newAlignEl=(aligns *) malloc(sizeof(aligns));
	  newAlignEl->hitPos=pos;
	  newAlignEl->orient=orient;
	  newAlignEl->prob=prob;
	  strcpy(newAlignEl->chroName, chroName_T);
	  strcpy(newAlignEl->cloneName, cloneName);
	  strcpy(newAlignEl->cloneSeq, cloneSeq);
		if (orient=='F')
	    listOfOEA_F[pos]=1;
	  if (orient=='R')
	    listOfOEA_R[pos]=1;
	  // printf("L44\n");
	  newAlignEl->next=listOfOEA[pos];
	  // printf("L46\n");
	  listOfOEA[pos]=newAlignEl;
	  // printf("L47\n");
	}
    } //in ro bezar dobare!!!!!!!!!!!!!!!!!!!!!!!!!!!1
  //printf("L44\n");
}


int addToListOfAlign(aligns *elementAlign)
{
  //printf("Hello\n");
  bool check=false;
  for (int count=0; count<numAlignsInCluster; count++)
    {
      if (strcmp(listAlignClusters[count].cloneName, elementAlign->cloneName)==0)
	{
	  check=true;
	}
    }
  if (check==false)
    {
      listAlignClusters[numAlignsInCluster].hitPos=elementAlign->hitPos;
      listAlignClusters[numAlignsInCluster].orient=elementAlign->orient;
      listAlignClusters[numAlignsInCluster].prob=elementAlign->prob;
      strcpy(listAlignClusters[numAlignsInCluster].cloneName, elementAlign->cloneName);
      strcpy(listAlignClusters[numAlignsInCluster].cloneSeq, elementAlign->cloneSeq);
			numAlignsInCluster++;
      //      printf("%i\n", numAlignsInCluster);
    }

}

int printClusterList(FILE *fp3)
{
  for (int count=0; count<numAlignsInCluster; count++)
    {
   if (listAlignClusters[count].orient=='R')
	fprintf(fp3, "%s %s %d 2 %f %s ", listAlignClusters[count].cloneName, chroName, listAlignClusters[count].hitPos, listAlignClusters[count].prob, listAlignClusters[count].cloneSeq);
    if (listAlignClusters[count].orient=='F')
	fprintf(fp3, "%s %s %d 1 %f %s ", listAlignClusters[count].cloneName, chroName, listAlignClusters[count].hitPos, listAlignClusters[count].prob, listAlignClusters[count].cloneSeq);
    }
  fprintf(fp3, "END\n");
}


int printCluster(int clusterBegin_F, int clusterEnd_F, int clusterBegin_R, int clusterEnd_R, FILE *fp3)
{
  //printf("L107\n");
  bool  check=false;
  int count=clusterBegin_R;
  int posClusterFMin=3000000000U;
  int posClusterFMax=0;
  int posClusterRMin=3000000000U;
  int posClusterRMax=0;
  aligns* elementAlign;
  // printf("L59\n");
  while (check==false && count<=clusterEnd_R)
    {
      if (listOfOEA_R[count]==1)
	check=true;
      count++;
    }
  //  printf("L65 %i %i %i %i\n", clusterBegin_F, clusterEnd_F, clusterBegin_R, clusterEnd_R);
  if (check==true)
    {
      for (count=clusterBegin_F; count<=clusterEnd_F; count++)
	{
	  if(listOfOEA_F[count]==1)
	    {
	      elementAlign=listOfOEA[count];
	      while(elementAlign!=NULL)
		{
		  if (elementAlign->orient=='F')
		    {
		      // printf("%s %s %i %i ", elementAlign->cloneName, elementAlign->chroName,elementAlign->hitPos, 1);
		      if (posClusterFMin > elementAlign->hitPos)
			posClusterFMin =elementAlign->hitPos;
		      if (posClusterFMax < elementAlign->hitPos)
			posClusterFMax = elementAlign->hitPos;
		      addToListOfAlign(elementAlign);
		    }
		  elementAlign=elementAlign->next;
		}
	    }
	}
      for (count=clusterBegin_R; count<=clusterEnd_R; count++)
	{
	  if(listOfOEA_R[count]==1)
	    {
	      elementAlign=listOfOEA[count];
	      while(elementAlign!=NULL)
		{
		  if (elementAlign->orient=='R')
		    {

		      if (posClusterRMin >elementAlign->hitPos)
			posClusterRMin=elementAlign->hitPos;
		      if (posClusterRMax < elementAlign->hitPos)
			posClusterRMax= elementAlign->hitPos;


		      addToListOfAlign(elementAlign);
		      //printf("%s %s %i %i ", elementAlign->cloneName, elementAlign->chroName,elementAlign->hitPos, 2);
		    }
		  elementAlign=elementAlign->next;
		}
	    }
	}
      //printf("END\n");
      if (posClusterRMax-posClusterRMin > maxSizeInst/2 && posClusterFMax-posClusterFMin > maxSizeInst/2)
	printClusterList(fp3);
      numAlignsInCluster=0;
    }
  //  printf("L95\n");
}


int compute_F_R_Clusters(int posStart, int posEnd, FILE *fp3)
{

  int startOfClusterR;
  int startOfClusterF;
  int posPrevREnd=posStart, posPrevFEnd=posStart;
  for (int count=posStart; count<posStart+maxSizeInst; count++)
    {
      if (listOfOEA_R[count]==1 &&(check_F(posPrevFEnd, count)||check_R(posPrevREnd, count+maxSizeInst)))
	{
 // printf("L110\n");
	  //  startOfClusterR=count;
	  //if (check_F(posPrevR, count)==true)
	  printCluster(posStart, count, count, count+maxSizeInst, fp3);
	  posPrevFEnd=count;
	  posPrevREnd=count+maxSizeInst;
	}
    }

  int count=posStart+maxSizeInst;
  bool endFound=false;
  //while (count<posStart+2*maxSizeInst && endFound==true)
  //{
  //  if (listOfOEA_R[count]==1)
  //{
  printCluster(posStart, posStart+maxSizeInst, count, posEnd, fp3);

  //  endFound=true;
  //}
  //count++;
  //}

}

bool check_R(int start, int end)
{
  int count=0;
  for (count=start+1; count<=end; count++)
    {
      if (listOfOEA_R[count]==1)
	return true;
    }
  return false;
}

bool check_F(int start, int end)
{
  for (int count=start+1; count<=end; count++)
    {
      if (listOfOEA_F[count]==1)
	return true;
    }
  return false;
}

int createClusters(FILE *fp3)
{
  //  printf("L218\n");
  for (int count=0; count<chroSize; count++)
    {
      //printf("L222\n");
      if (listOfOEA_F[count]==1 && (check_R(EndClusterR, count+2*maxSizeInst)||check_F(EndClusterF, count+maxSizeInst)))
	{
	//    printf("L223\n");
	  compute_F_R_Clusters(count, count+2*maxSizeInst, fp3);
	  EndClusterR=count+2*maxSizeInst;
	  EndClusterF=count+maxSizeInst;
	}
    }
}

int main(int argv, char *argc[])
{
  int return_value;
  numAlignsInCluster=0;
  FILE *fp1, *fp2, *fp3;
  fp1=fopen(argc[1],"r");
  fp2=fopen(argc[2],"r");
  fp3=fopen(argc[3],"w");

	maxSizeInst = 330;
	if (argv == 5) maxSizeInst=atoi(argc[4]);

	//fprintf(stdout, "max Insert size = %d\n", maxSizeInst);

	EndClusterR=0;
  EndClusterF=0;
  return_value = fscanf( fp1,"%s\t%i\n", chroName, &chroSize);
  if( return_value != 2)
  {
      perror( "Error on fscanf");
      exit( 1);
  }

  listOfOEA=(aligns **) malloc(chroSize*sizeof(aligns*));
  listOfOEA_R=(int *) malloc(chroSize*sizeof(int));
  listOfOEA_F=(int *) malloc(chroSize*sizeof(int));
  for (int count=0; count< chroSize; count++)
    {
      listOfOEA[count]=NULL;
      listOfOEA_R[count]=0;
      listOfOEA_F[count]=0;

    }
  //  printf("%s\t%i\n", chroName, chroSize);

  readTheAligns(fp2);
  createClusters(fp3);
}
