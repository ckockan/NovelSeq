#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

const int minSpan=0;
const int strSize=250;
const int max_read_length=150;

struct clusEl{
  int clusterId;
  float probAlign;
  clusEl *next;
};

//typedef struct clusEl clusterEl;

struct cloneHead{
  char cloneName[strSize];
  char cloneSeq[max_read_length];
	int cloneId;
  //  float totalProbClone;//summation of prob of all posible alignments of the clone
  int cloneCovered; //0: Not yet covered
  clusEl *next;
};


struct cloneEl{
  int cloneId;
  //  int startPos;
  // int stopPos;
  int hitPos;
  float probAlign;//prob of this particular alignments
  char chroName[strSize];
  //  int SVtype;
  int orient;
  cloneEl *next;
	char cloneSeq[max_read_length];

};


struct clusHead{
  int clustreId;
  int clusterChoosen;//0: Not yet covered
  int cloneSup;
  double clustProb; //the summation of prob of alignnment in this cluster
  cloneEl *next;
};


FILE *fp2;


char cloneName[strSize];
char cloneSeq[max_read_length];
cloneHead *listClones;
clusHead *listClusters;
const int maxNumCluster=40000000;
int numClusters;
int numClones;
int total=0;
int minLeftSupport = 3;
int minRightSupport = 3;

void readClones(FILE *fp)
{
  int cloneCount=0;
  char temp[strSize];
//	char temp2[max_read_length];
  //float totalSumClones;
  while (fscanf(fp,"%s\n",temp)!=EOF)
    {
      listClones[cloneCount].cloneCovered=0;
      strcpy(listClones[cloneCount].cloneName, temp);
    // 	strcpy(listClones[cloneCount].cloneSeq, temp2);
			 //      listClones[cloneCount].totalProbClone=totalSumClones;
      //if (totalSumClones==0) printf("ERRORRRR\n");
      listClones[cloneCount].next = NULL;
      cloneCount++;
      //      printf("%i\n", cloneCount);
      // printf("%s\n", temp);
    }
}


int findCloneId(char *cloneName)
{
  //  printf("In\n");
  int minId=0;
  int maxId=numClones;
  int middleId=numClones/2;
  if (strcmp(cloneName, listClones[minId].cloneName)==0)
    return minId;
  if (strcmp(cloneName, listClones[maxId].cloneName)==0)
    return maxId;

  while (strcmp(cloneName, listClones[middleId].cloneName)!=0)
    {
      if (strcmp(cloneName, listClones[middleId].cloneName)>0)
	{
	  minId=middleId;
	  middleId=(minId+maxId)/2;
	}
      else if (strcmp(cloneName, listClones[middleId].cloneName)<0)
	{
	  maxId=middleId;
	  middleId=(minId+maxId)/2;
	}
      if (strcmp(cloneName, listClones[minId].cloneName)==0)
	return minId;
      if (strcmp(cloneName, listClones[maxId].cloneName)==0)
	return maxId;
      //printf("%s %s %s %s %i\n",listClones[minId].cloneName, listClones[maxId].cloneName, listClones[middleId].cloneName, cloneName, middleId);
    }
  //  printf("Out\n");
  return middleId;


}

void readClusters(FILE *fp)
{
  int return_value;
  char temp[strSize];
  cloneEl *tempCloneEl;
  clusEl *tempClusEl;
  //int clusterNum=0;
  int cloneId;
  //int startPos,stopPos, SVtype;
  int hitPos, orient;
  float editProbDist;
  char chroName[strSize];
  char cloneSeq[max_read_length];
	int count=0, count1=0;
  listClusters[0].next=NULL;
  numClusters=0;
  while (fscanf(fp,"%s ",temp)!=EOF)
    {
      count++;
      //if (count-(count/1000000)*1000000 ==0)
      //printf("%i %i %s\n", count,count1, temp);
      if (strcmp(temp,"END")==0)
	{
	  count1++;
	  //fscanf(fp,"\n");
	  //  clusterNum++;
	  //	  printf("END\n");
	  /////////////////////////////////////THIS PART CHANGED////////////////////////

	  //	  if (listClusters[numClusters].cloneSup>1)
	    numClusters++;
	    //else
	    //{
	      //free(listClusters[numClusters].next->chroName);
	      //free(listClusters[numClusters].next);
	      listClusters[numClusters].next=NULL;
	      listClusters[numClusters].cloneSup=0;
	      listClusters[numClusters].clustProb=0;
	    //}
	}
      else
	{
	  //	  printf("%s\n", temp);
	  //printf("%s\n", temp);
	  return_value = fscanf(fp,"%s %i %i %f %s",chroName, &hitPos, &orient, &editProbDist, cloneSeq);
	  if( return_value != 5)
	  {
	      perror( "Error on fscanf");
	      exit( 1);
	  }
	  //	  editProbDist=0.1;
	  cloneId=findCloneId(temp);
	  //	  printf("HERE %i \n", cloneId);
	  tempCloneEl=(cloneEl *)malloc(sizeof(cloneEl));
	  tempCloneEl->cloneId=cloneId;
	  strcpy(tempCloneEl->chroName, chroName);
	  strcpy(tempCloneEl->cloneSeq, cloneSeq);
		tempCloneEl->hitPos=hitPos;
	  // tempCloneEl->stopPos=stopPos;
	  tempCloneEl->orient=orient;
	  tempCloneEl->probAlign=editProbDist;
	  tempCloneEl->next=listClusters[numClusters].next;
	  listClusters[numClusters].next=tempCloneEl;
	  listClusters[numClusters].clustProb=listClusters[numClusters].clustProb+editProbDist;
	  //  printf("%s %i %i %f %f %i %f\n",listClones[cloneId].cloneName,cloneId,numClusters, listClusters[numClusters].clustProb, (float)listClones[cloneId].totalProbClone, editDist, (float)1/(float)pow(100,editDist));
	  listClusters[numClusters].cloneSup++;

	  tempClusEl=(clusEl *)malloc(sizeof(clusEl));
	  tempClusEl->clusterId=numClusters;
	  tempClusEl->probAlign=editProbDist;
	  tempClusEl->next = listClones[cloneId].next;
	  if (listClones[cloneId].next==NULL)
	    {
	      //  printf("%s\n", temp);
	      total++;
	    }
	  listClones[cloneId].next=tempClusEl;
	  //	  printf("LL\n");
	}

    }

}


int findBigCluster()
{
  int maxId=-1;
  float maxClone=-1;

  for (int count=0; count<numClusters; count++)
    {
      //      printf("%i %i\n",count, listClusters[count].cloneSup);
      //  if (listClusters[count].next->SVtype!=3)
      //{
	  if (listClusters[count].clustProb>maxClone)
	    {
	      maxClone=listClusters[count].clustProb;
	      maxId=count;

	    }
	  //}
	  //else
	  //{
	  //if (1.7*listClusters[count].clustProb > maxClone)
	  //{
	  //  maxClone=1.7*listClusters[count].clustProb;
	  //  maxId=count;
	  //}

	  //}
    }
  if (maxClone < 0) return -1;
  else
    return maxId;

}

void removeClones(int clusterId)
{
  cloneEl *ptrClone, *ptrClone2;
  clusEl *ptrCluster;
  int cloneId;

  ptrClone = listClusters[clusterId].next;
  while (ptrClone!=NULL)
    {
      cloneId = ptrClone -> cloneId;


      if (listClones[cloneId].cloneCovered==0)
	{
	  ptrCluster = listClones[cloneId].next;
	  listClones[cloneId].cloneCovered=1;
	  while(ptrCluster!=NULL)
	    {

	      //     if (ptrCluster->clusterId==470) printf("CLUSTER 470: %i %s\n", listClusters[ptrCluster->clusterId].cloneSup, listClones[cloneId].cloneName);
	      //	      if (ptrCluster->clusterId==291263)
	      //printf("removing %s\n", listClones[cloneId].cloneName);
	      //ptrClone2=listClusters[ptrCluster->clusterId].next;
	      //	      while (ptrClone2->cloneId!=cloneId)
	      //	{
	      //  ptrClone2=ptrClone2->next;
	      //}
	      //	      if (ptrCluster->clusterId==301484)
	      //printf("This Was Takes %i %0.20f %0.20f\n",listClusters[ptrCluster->clusterId].cloneSup, listClusters[ptrCluster->clusterId].clustProb, ptrCluster->probAlign);
	      listClusters[ptrCluster->clusterId].cloneSup--;
	      listClusters[ptrCluster->clusterId].clustProb=listClusters[ptrCluster->clusterId].clustProb - ptrCluster->probAlign;
	      ptrCluster = ptrCluster->next;
	    }
	  //	  if (ptrCluster->clusterId==291263)
	  //printf("OUt While\n");
	}
      ptrClone=ptrClone->next;

    }

}


void outputCluster(int clusterId)
{
  //printf("%i %f %i\n", clusterId, listClusters[clusterId].clustProb, listClusters[clusterId].cloneSup);
  cloneEl *ptr;
  ptr=listClusters[clusterId].next;
  //total=total+listClusters[clusterId].cloneSup;
  //printf("%i\n", ptr->SVtype);
  // int smallStart=300000000;
  // int bigStart=0;
  // int smallEnd=300000000;
  //nt bigEnd=0;
  int RClusterMin=400000000;
  int RClusterMax=0;
  int FClusterMin=400000000;
  int FClusterMax=0;
  bool checkF=false;
  bool checkR=false;
  int countF=0, countR=0;
  long int AvgLeng = 0;
  //if (listClusters[clusterId].cloneSup==0)
    //printf("%i Error\n", listClusters[clusterId].cloneSup);
  //printf("L254\n");
  while (ptr!=NULL)
    {
      //      printf("Here\n");
      if (listClones[ptr->cloneId].cloneCovered==0)
	{

	  if (ptr->orient==1)
	    {
	      //	      printf("GotF\n");
	      if (ptr->hitPos < FClusterMin)
		FClusterMin=ptr->hitPos;
	      if (ptr->hitPos > FClusterMax)
		FClusterMax=ptr->hitPos;
	      checkF=true;
	      countF++;
	    }
	  if (ptr->orient==2)
	    {
	      // printf("GotR\n");
	      if (ptr->hitPos < RClusterMin)
		RClusterMin=ptr->hitPos;
	      if (ptr->hitPos > RClusterMax)
		RClusterMax=ptr->hitPos;
	      checkR=true;
	      countR++;
	    }


	  /*
	  if (ptr->startPos < smallStart)
	    smallStart=ptr->startPos;
	  if (ptr->startPos > bigStart)
	    bigStart=ptr->startPos;
	  if (ptr->stopPos < smallEnd)
	    smallEnd=ptr->stopPos;
	  if (ptr->stopPos > bigEnd)
	    bigEnd = ptr->stopPos;
	  //	  printf("L267\n");
	  AvgLeng= AvgLeng + ptr->stopPos - ptr->startPos;
	  */
	  //if((ptr->stopPos - ptr->startPos)<0)
	  //{
	  //printf("ERROORRRRRRRRRRRRRRRRRRRRRR\n");
	  //printf("ERRRORORROROROROROROOROR\n");
	  //	  printf("%s %i %i\n", listClones[ptr->cloneId].cloneName,ptr->startPos, ptr->stopPos);

	  //total=total+listClusters[clusterId].cloneSup;
	}
      ptr=ptr->next;
      //      printf("L278\n");
    }
  //printf("L280\n");
  //  if (bigStart-smallStart >= minSpan || bigEnd-smallEnd >= minSpan)
  // {
  ptr=listClusters[clusterId].next;
  //while (ptr!=NULL)
  //{

  //printf("%i\n", ptr->cloneId);
  /*	  if (listClones[ptr->cloneId].cloneCovered==0)
    {
    printf("%s %i %i\n", listClones[ptr->cloneId].cloneName,ptr->startPos, ptr->stopPos);

    //total=total+listClusters[clusterId].cloneSup;
    }*/

  //  ptr=ptr->next;
  //}
  //ptr=listClusters[clusterId].next;
  //if (checkF && checkR )
  //printf("Got F and R %i \n", countF+countR);
  //printf("%i \n", countF+countR);
  if (checkF && checkR && countF>=minLeftSupport && countR>=minRightSupport)
    {
      printf("%i %i %i %i %s %f %i\n",FClusterMin, FClusterMax, RClusterMin, RClusterMax, ptr->chroName, listClusters[clusterId].clustProb, clusterId);
      while (ptr!=NULL)
	{
	  if(listClones[ptr->cloneId].cloneCovered==0)
	    {
	      if (ptr->orient==1)
		printf("%s %s %i F %f %s ", listClones[ptr->cloneId].cloneName, ptr->chroName, ptr->hitPos, ptr->probAlign, ptr->cloneSeq);
	      else if (ptr->orient==2)
		printf("%s %s %i R %f %s ", listClones[ptr->cloneId].cloneName, ptr->chroName, ptr->hitPos, ptr->probAlign, ptr->cloneSeq);
	      //  ptr=ptr->next;
	    }
	  ptr=ptr->next;
	}
      printf("END\n");
    }

  //      if (listClusters[clusterId].cloneSup==0)
  //printf("ERORRRRRRRRRRRRRRRRRRR1 %i %f\n", clusterId, listClusters[clusterId].clustProb);
  //printf("Inside_Start:%i Inside_End:%i OutSide_Start:%i Oustide_End:%i chro:%s SVtype:%i sup:%i Avg_Span:%i sumProb:%f\n",
  //     smallStart, bigStart, smallEnd, bigEnd, ptr->chroName, ptr->SVtype, listClusters[clusterId].cloneSup, AvgLeng/listClusters[clusterId].cloneSup, listClusters[clusterId].clustProb);
  //      printf("******************************************\n");
  //}

  //ptr=listClusters[clusterId].next;


}

void setCover()
{

  int bigCluster;
  bigCluster = findBigCluster();
  //printf("L396   %i %i\n", bigCluster, listClusters[bigCluster].cloneSup);


  while((bigCluster>-1) && (listClusters[bigCluster].clustProb >4)){
    //    printf("%i %i\n", bigCluster, listClusters[bigCluster].cloneSup);
    //printf("1 %i %i %f\n", bigCluster, listClusters[bigCluster].cloneSup, listClusters[bigCluster].clustProb);
    outputCluster(bigCluster);
    //    printf("2\n");
    removeClones(bigCluster);
    //printf("3\n");
    bigCluster=findBigCluster();
    //printf("4\n");
    //printf("%i\n", bigCluster);
  }

}
//int total=0;

int main(int argv, char *argc[])
{
  FILE *fp;//name of clones (sorted)
  FILE  *fp1;//clusters

  int return_value;

  fp = fopen(argc[1],"r");
  fp1=fopen(argc[2],"r");
  //printf("HERE\n");
  //fp2=fopen(arg[c],"w");

	if (argv == 4) {
		minLeftSupport = atoi(argc[3]);
		minRightSupport = minLeftSupport;
		}
	else if (argv == 5) {
		minLeftSupport = atoi(argc[3]);
		minRightSupport = atoi(argc[4]);
	}
	return_value = fscanf(fp,"%i\n",&numClones);
	if( return_value != 1)
	{
	    perror( "Error on fscanf");
	    exit( 1);
	}
  //printf("%i\n", numClones);
  listClones = (cloneHead*) malloc((numClones+1)*sizeof(cloneHead));
  listClusters = (clusHead *) malloc(maxNumCluster*sizeof(clusHead));
  //printf("%i\n", strcmp("149","14:"));
  //printf("%i\n",strcmp("HWI-EAS90__20B1UAAXX:6:17:580:368.1","HWI-EAS90__20B1UAAXX:6:175:802:480.1"));
  for (int count=0; count<=maxNumCluster; count++)
    {
      listClusters[count].cloneSup=0;
      listClusters[count].clustProb=0;
    }

  readClones(fp);
 // printf("JJ\n");
  //char *test;
  //test = (char *)malloc(50*sizeof(char));
  //strcpy(test, "HWI-EAS90__20B1UAAXX:6:175:841:975.1");
  //printf("%i\n", findCloneId(test));
  readClusters(fp1);
 // printf("HH\n");
  //  int total=0;
  /*for (int count=0; count<numClusters; count++)
    {
      if (listClusters[count].cloneSup==1) fprintf(fp2,"%i %i\n", count, listClusters[count].cloneSup);
      //total=total+listClusters[count].cloneSup;
      }*/
  //  printf("TOTAL:%i\n", total);
  /*  for (int count=0; count<numClusters; count++)
  {
    //    printf("%i %f\n",count, listClusters[count].clustProb);
    }*/
  setCover();
  //printf("TOTAL %i\n", total);
  //fclose(fp2);
}
