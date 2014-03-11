/*

   1) The mismatchPenalty, indelPenalty and matchScore should be tested such that a best one is picked
   2) Check the greedy method to see how good it does
   3) Check the maximum spaning tree vcersion and see which one is working better

 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> 
#include <sys/param.h>

#define mismatchPenalty 2
#define indelPenalty 4
#define matchScore 1

using namespace std;

int clusterId_Min, clusterId_Max;
const int scoreVariance=3;

int deltaMin, deltaMax;
int OEASeqMaxLen=10000;
//int delta_max=200;
//int delta_min=100;
const int max_read_length=250;
int read_number;
//float *alignScore;
int is_Forward;
int histrogram[1000];


typedef struct oea_reads {
   char name[100];
   char seq[max_read_length];
   int pos;
   int cluster_id;
   double qual;
   int is_F;
   int mark; //0 if it is not yet picked. 1 if it is picked (it should not be picked again);
} OEA_read;


typedef struct EdgeEls{
   int headNode;
   int tailNode;
   float score;
   EdgeEls *next;
} EdgeEl;


EdgeEls *listEdgeEl;

OEA_read *read;
int maxNumReads;


float **probOverlapBasedPos;
float *len_hist;
float **graph_weights_matrix;
float *suffixPrefixAlignments(char *, char *);

char *addSeq(char *seq1, char *seq2, int overlapCount)
{
   char *resultSeq=(char *) malloc((strlen(seq1)+strlen(seq2)+1-overlapCount)*sizeof(char));
   for (int count=0; count<strlen(seq1); count++)
   {
      resultSeq[count]=seq1[count];
   }	
   for (int count=overlapCount; count<strlen(seq2); count++)
   {
      resultSeq[count+strlen(seq1)-overlapCount]=seq2[count];
   }
   resultSeq[(strlen(seq1)+strlen(seq2)-overlapCount)]='\0';
   return resultSeq;
}		

int addNewEl(int nodeStart, int nodeStop)
{
   EdgeEls *newEl;
   newEl= (EdgeEls *) malloc(sizeof(EdgeEls));
   newEl->headNode=nodeStart;
   newEl->tailNode=nodeStop;
   newEl->score=graph_weights_matrix[nodeStart][nodeStop];

   newEl->next=listEdgeEl;
   listEdgeEl=newEl;
}




int addNewMaxEl(int nodeStart, int nodeStop)
{
   EdgeEls *ptList;
   EdgeEls *headList;
   float maxValue;
   maxValue=graph_weights_matrix[nodeStart][nodeStop];  
   if (listEdgeEl!=NULL)
   {
      while (listEdgeEl!=NULL && listEdgeEl->score < maxValue-scoreVariance)
      {

         ptList=listEdgeEl->next;
         listEdgeEl=ptList;
      }

      while(listEdgeEl!=NULL && listEdgeEl->next!=NULL)
      {
         if (listEdgeEl->next->score < maxValue-scoreVariance)
         {
            ptList=listEdgeEl->next;
            listEdgeEl->next=ptList->next;
         }
         else 
            listEdgeEl=listEdgeEl->next;
      }
      addNewEl(nodeStart, nodeStop);

   }
   else
   {
      addNewEl(nodeStart, nodeStop);
   }
}


int bestEdgeToPick(int *source, int *dest)
{
   EdgeEls *ptrEdge;
   ptrEdge=listEdgeEl;
   float minValue=1000;
   int minValueSource=-1;
   int minValueDest=-1;
   while(ptrEdge!=NULL)
   {
      if (abs(read[ptrEdge->headNode].pos - read[ptrEdge->tailNode].pos)<minValue)
      {
         minValue=abs(read[ptrEdge->headNode].pos - read[ptrEdge->tailNode].pos);
         minValueSource=ptrEdge->headNode;
         minValueDest=ptrEdge->tailNode;
      }
      ptrEdge=ptrEdge->next;
   }
   *source=minValueSource;
   *dest=minValueDest;
}


void freeList(){
   EdgeEls *tmp;
   while (listEdgeEl != NULL){
      tmp = listEdgeEl->next;
      free(listEdgeEl);
      listEdgeEl = tmp;
   }
   listEdgeEl=NULL;
}


char *greedyGraphTraversal(float **graph_weights_matrix, int numNodes)
{
   char *tempResultSeq;
   float *alignScore;
   int overlapCount;
   float overlapScore;
   int **edgesPicked;
   char *resultSeq;

   float minScoreForAdding=15;
   int nodeChoosen;
   float maxValue=-1000;
   float minValueInList=-1000;
   int startNode;
   int stopNode;
   int prevStartNode=stopNode;	
   int prevStopNode=startNode;
   bool firstAdded, secondAdded;
   tempResultSeq=(char *) malloc(OEASeqMaxLen * sizeof(char));

   listEdgeEl=NULL;

   for (int count=0; count<numNodes; count++)
   {
      for (int count2=0; count2<numNodes; count2++)
      {
         if (graph_weights_matrix[count][count2]>maxValue && count!=count2)
         {
            addNewMaxEl(count, count2);
            maxValue=graph_weights_matrix[count][count2];

         }
         else if (graph_weights_matrix[count][count2] > maxValue - scoreVariance && count!=count2)
         {
            addNewEl(count, count2);
         }
      }
   }

   bestEdgeToPick(&startNode, &stopNode);

   read[startNode].mark=1;
   read[stopNode].mark=1;	
   alignScore=suffixPrefixAlignments(read[startNode].seq, read[stopNode].seq);
   overlapCount=-1;
   overlapScore=-1;
   for (int count=0; count<strlen(read[stopNode].seq); count++)
   {
      if (alignScore[count]>overlapScore)
      {
         overlapScore=alignScore[count];
         overlapCount=count;	
      }
   }

   free(alignScore);

   prevStartNode=startNode;
   prevStopNode=stopNode;



   resultSeq=addSeq(read[startNode].seq, read[stopNode].seq, overlapCount);

   freeList();
   listEdgeEl=NULL;

   do
   {
      maxValue=-1000;
      nodeChoosen=0;
      for (int count=0; count<numNodes; count++)
      {

         firstAdded=false;
         secondAdded=false;
         if (read[count].mark==0)
         {	      
            if (maxValue < graph_weights_matrix[count][prevStartNode])
            {  
               maxValue=graph_weights_matrix[count][prevStartNode];
               addNewMaxEl(count, prevStartNode);
               firstAdded=true;
            } 
            if (maxValue < graph_weights_matrix[prevStopNode][count])
            {	  
               maxValue=graph_weights_matrix[prevStopNode][count];
               addNewMaxEl(prevStopNode, count);
               secondAdded=true; 
            }
            if (firstAdded==false && maxValue-scoreVariance < graph_weights_matrix[count][prevStartNode])
            {

               addNewEl(count, prevStartNode);
            }
            if (secondAdded==false &&  maxValue - scoreVariance < graph_weights_matrix[prevStopNode][count])
            {

               addNewEl(prevStopNode, count);

            }
         }

      }
      bestEdgeToPick(&startNode, &stopNode);

      alignScore=suffixPrefixAlignments(read[startNode].seq, read[stopNode].seq);
      if (maxValue>minScoreForAdding)
      {
         if (startNode==prevStopNode)
         {
            free(alignScore);
            alignScore=suffixPrefixAlignments(read[prevStopNode].seq, read[stopNode].seq);
            overlapCount=-1;
            overlapScore=-100;
            for (int count=0; count<strlen(read[stopNode].seq)+1; count++)
            {
               if (alignScore[count]>overlapScore)
               {
                  overlapScore=alignScore[count];
                  overlapCount=count;
               }
            }
            strcpy(tempResultSeq, resultSeq);
            resultSeq=addSeq(resultSeq, read[stopNode].seq, overlapCount);

            if (strlen(resultSeq)>OEASeqMaxLen)
               return tempResultSeq;
            read[stopNode].mark=1;
            prevStopNode=stopNode;
            nodeChoosen=1;
            free(alignScore);

         }else if (stopNode==prevStartNode)
         {
            free(alignScore);
            alignScore=suffixPrefixAlignments(read[startNode].seq, read[prevStartNode].seq);
            overlapCount=-1;
            overlapScore=-1;
            for (int count=0; count<strlen(read[prevStartNode].seq)+1; count++)
            {
               if (alignScore[count]>overlapScore)
               {
                  overlapScore=alignScore[count];
                  overlapCount=count;
               }
            }
            strcpy(tempResultSeq, resultSeq);
            resultSeq=addSeq(read[startNode].seq,resultSeq, overlapCount);                                                       
            if (strlen(resultSeq)>OEASeqMaxLen)
               return tempResultSeq;
            read[startNode].mark=1;
            prevStartNode=startNode;
            nodeChoosen=1;
            free(alignScore);
         }
      }else free(alignScore);
      freeList();
      listEdgeEl=NULL;
   }while(maxValue>minScoreForAdding && strlen(resultSeq)<400);

   return(resultSeq);
}

void calculate_prob_hist()
{

   for (int dist1= (-1)*deltaMax; dist1<deltaMax; dist1++) //dist1 is the distance between the ends of the mapped paired-ends if Forward strand.
   {
      for (int dist2=0; dist2<max_read_length; dist2++) // dist2 is the distance between the ends of the unmapped paired-end if Reverse strand
      {
         for (int paired_end_len=deltaMin; paired_end_len<deltaMax; paired_end_len++) // the distance between the paired-ends of the first paired-end
         {
            if (paired_end_len+dist2-dist1 > 0 && paired_end_len-dist1+dist2<deltaMax) probOverlapBasedPos[dist1+deltaMax][dist2] += len_hist[paired_end_len] * len_hist[paired_end_len -dist1 +dist2]; 
         }
      }

   }
}

float max3way(float a, float b, float c)
{
   if ((a>=b)&&(a>=c))
      return a;
   if ((b>=a)&&(b>=c))
      return b;
   if ((c>=b)&&(c>=a))
      return c;
}

float max (float a, float b)
{
   if (a<b)
      return b;
   else return a;
}
float min(float a, float b)
{

   if (a<b)
      return a;
   else return b;
}

float *suffixPrefixAlignments(char *str1, char *str2) // aligns suffix of str1 with prefix of str2
{
   int rowSize, colSize; // rowSize: is the strlen(str2) and colSize: is strlen(str1). The last column should be returned.
   float *returnCol;
   float **alignMatrix; // alignMatrix[x][y] is aigning str2[x] and str1[y]
   rowSize=strlen(str2);
   colSize=strlen(str1);
   returnCol=(float *) malloc((rowSize+1)*sizeof(float));
   alignMatrix=(float **) malloc((rowSize+1)*sizeof(float *));
   for (int countI=0; countI<rowSize+1; countI++)
   {
      alignMatrix[countI]=(float *) malloc((colSize+1)*sizeof(float));
   }
   for (int countI=0; countI<(rowSize+1); countI++)
   {
      for (int countI2=0; countI2<colSize+1; countI2++)
         alignMatrix[countI][countI2]=0;
   }
   for (int countRow=0; countRow<rowSize+1; countRow++)
      alignMatrix[countRow][0]=(-1)*indelPenalty*countRow;

   for (int countRow=1; countRow<rowSize+1; countRow++)
   {
      for (int countCol=1; countCol<colSize+1; countCol++)
      {
         if (str1[countCol-1]!=str2[countRow-1])
         {
            alignMatrix[countRow][countCol]=max3way(alignMatrix[countRow-1][countCol-1]-mismatchPenalty, alignMatrix[countRow-1][countCol]-indelPenalty, alignMatrix[countRow][countCol-1]-indelPenalty);
         }
         else
         {
            alignMatrix[countRow][countCol]=max3way(alignMatrix[countRow-1][countCol-1]+matchScore, alignMatrix[countRow-1][countCol]-indelPenalty, alignMatrix[countRow][countCol-1]-indelPenalty);
         }
      }	
   }

   for (int countRow=0; countRow<rowSize+1; countRow++)
   {
      returnCol[countRow]=alignMatrix[countRow][colSize];
   }


   for (int countI=0; countI<rowSize+1; countI++)
   {
      free(alignMatrix[countI]);
   }
   free(alignMatrix);

   return returnCol; // returns an array in which returnCol[i] is 

}

void get_len_hist(FILE *fp)
{
   fscanf(fp,"%i %i\n", &deltaMin, &deltaMax);
   len_hist=(float *) malloc((deltaMax+1)*sizeof(float));
   probOverlapBasedPos=(float **) malloc((2*deltaMax+1)*sizeof(float*));

   for (int count=0; count<deltaMax; count++)
      len_hist[count]=0;

   for (int count=0; count<2*deltaMax+1; count++)
      probOverlapBasedPos[count]=(float *)malloc(max_read_length*sizeof(float));

   for (int count=0; count<2*deltaMax+1; count++)
      for (int count1=0; count1<max_read_length; count1++)
         probOverlapBasedPos[count][count1]=0;

   int lenBase;
   float prob;
   while(fscanf(fp,"%i %f\n", &lenBase, &prob)!=EOF){
      len_hist[lenBase]=prob;
   } 
   calculate_prob_hist();
}

//This function is resposible of reading the OEA reads from inpyt file. Assuming the maximum read length is "max_read_length=41".
//The name of the input file shoul be given as an arugumtn to the program.

/*
int read_OEA_reads(FILE * readfile, int cluster_number)
{
   float temp;
   int position;
   char * readstr;
   char readchr[5];
   char * readname;
   char strand;
   readstr = (char*) malloc(sizeof(char) * max_read_length);
   readname = (char*) malloc(sizeof(char) * 100);
   read_number=0;
   fscanf(readfile,"%s\n", readname);
   while(strcmp(readname,"END")!=0)
   {
      strcpy(read[read_number].name, readname); // read-name 	  

      fscanf(readfile,"%s\n", read[read_number].seq); 
     


			 if (strlen(read[read_number].seq)==36)
         read[read_number].seq[36]='\0';
      else if (strlen(read[read_number].seq)==37)
         read[read_number].seq[37]='\0';
      else  if (strlen(read[read_number].seq)==41)
         read[read_number].seq[41]='\0';
      read[read_number].mark=0;
      fscanf(readfile, "%s\n%i\n%c\n%f\n%i\n", &readchr, & (read[read_number].pos), &strand, &temp);
      read[read_number].cluster_id = cluster_number;
			if (strand=='F') read[read_number].is_F=1;

      else read[read_number].is_F=0;


      if (strcmp(readstr,"(null)")!=0 && read[read_number].cluster_id>clusterId_Min && read[read_number].cluster_id<clusterId_Max)    
      {
         read_number++;
      }

      fscanf(readfile,"%s\n", readname);
   }

}


*/

//This function is resposible of reading the OEA reads from inpyt file. Assuming the maximum read length is "max_read_length=41".
//The name of the input file shoul be given as an arugumtn to the program.


int read_OEA_reads(FILE * readfile)
{
   float temp;
   int position;
   char * readstr;
   char readchr[5];
   char * readname;
   char strand;
   readstr = (char*) malloc(sizeof(char) * max_read_length);
   readname = (char*) malloc(sizeof(char) * 100);
   read_number=0;
   while (fscanf(readfile,"%s\n", readname)!=EOF)
	{
   strcpy(read[read_number].name, readname); // read-name 	  
   fscanf(readfile,"%s\n", read[read_number].seq); 
   //read[read_number].seq[strlen(read[read_number])]='\0';
   read[read_number].is_F=is_Forward; 
	 read_number++;	
	}

  // read[read_number].mark=0;
  //    fscanf(readfile, "%s\n%i\n%c\n%f\n%i\n", &readchr, & (read[read_number].pos), &strand, &temp);
  //    read[read_number].cluster_id = cluster_number;
//			if (strand=='F') read[read_number].is_F=1;
 //     else read[read_number].is_F=0;


  //    if (strcmp(readstr,"(null)")!=0 && read[read_number].cluster_id>clusterId_Min && read[read_number].cluster_id<clusterId_Max)    
   //   {
 //        read_number++;
 //     }

   //   fscanf(readfile,"%s\n", readname);
//   }

}


float prob(OEA_read x, OEA_read y)
{

   int dist1;
   dist1=y.pos-x.pos;
   float *overlapScoreBasedAlignment = (float *) malloc(sizeof(float) * (strlen(y.seq)+1));
   overlapScoreBasedAlignment= suffixPrefixAlignments(x.seq, y.seq);
   float result;
   result=0;
   for (int overlap=0; overlap<min(strlen(y.seq)+1, strlen(x.seq)+1); overlap++) //overlap is the length of the overlap between the end of suffix-preffix
   {
      if (strlen(x.seq)-overlap >=0 ) 
         if (result<overlapScoreBasedAlignment[overlap])
            result=overlapScoreBasedAlignment[overlap];

   }
   free(overlapScoreBasedAlignment);
   return result;
}


int createTheGraph()
{
   graph_weights_matrix=(float **) malloc(read_number * sizeof(float *));
   for (int count=0; count<read_number; count++)
   {
      graph_weights_matrix[count]=(float *)malloc(read_number * sizeof(float));
   }

   for(int nodeA=0; nodeA<read_number; nodeA++)
   {
      for (int nodeB=0; nodeB<read_number; nodeB++)
      {

         graph_weights_matrix[nodeA][nodeB]=prob(read[nodeA], read[nodeB]);
      }
   }


}

int main(int argc, char *argv[])
{

  // fprintf(stdout, "USAGE: <histogram> <cluster file> <maxNumReads> <forward/reverse>\n\n");

   int clusterId;
   char clusterOrient;
   int clusterIdToCreate; 
	FILE *fpLenHist, *fpOEAReads;
   fpLenHist=fopen(argv[1],"r");// The first input argument is the name of the file which has the histrogram of insert sizes pf paired-end reads
   fpOEAReads=fopen(argv[2],"r");// The second input argument is the name of the file which has the OEA reads, position mapping of the one end, the strand and the cluster id (integer).
   maxNumReads=atoi(argv[3]);
	 is_Forward=atoi(argv[4]);
 
   bool readAllClusters = false;

   read=(OEA_read *) malloc(maxNumReads*sizeof(OEA_read));
   get_len_hist(fpLenHist);
   FILE *outF;
//   outF=fopen("outputF3.txt","w");
   char *resultStr;


   listEdgeEl=NULL;
   read_OEA_reads(fpOEAReads);

   createTheGraph();
   resultStr=greedyGraphTraversal(graph_weights_matrix,read_number);
   printf(">%s\n", argv[2]);
	 printf("%s\n", resultStr);
         for (int count=0; count<read_number; count++)
         {
            free(graph_weights_matrix[count]);
         }
         free(graph_weights_matrix);

  // fclose(outF);

}
