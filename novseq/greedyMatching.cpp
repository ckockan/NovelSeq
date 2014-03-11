
// The matching part of the NovelSeq pipeline. 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct OEA_Cluster{
    int cluster_Id;
    int insideStart;
    int outsideStart;
    int insideEnd;
    int outsideEnd;
    char OEA_1[2000];
    char OEA_2[2000];
    char chrName[10];
    
}OEA_Clusters;

OEA_Cluster *list_OEA_Cluster;


typedef struct highScoreMatch{
  int OEA_Id;
  int NoHit_Id;
  int score;
  int matchOrient;
}highScoreMatch;


const int minScoreMatch=50;

highScoreMatch listHighScoreMatch[110000];
int countHighScoreMatch=0;

int *OEAIndexMarked;
int *NHIndexMarked;


int **matrixWeights;
int **matrixOrient;

int countNoHit;
int countOEA;

char **NoHitSeq;


//This function is resposible of reading the OEA reads from inpyt file. Assuming the maximum read length is"max_read_length=41".                            
//The name of the input file shoul be given as an arugumtn to the program.                                                                                   

char * reverseComp(char *str)
{
    char *RevStr;
    RevStr=(char *) malloc((strlen(str)+1)*sizeof(char));
    for (int count=0; count<strlen(str); count++)
    {
        if (str[count]=='A' || str[count]=='a')
        {
            RevStr[strlen(str)-1-count]='T';
        }
        if (str[count]=='C' || str[count]=='c')
        {
            RevStr[strlen(str)-1-count]='G';
        }
        if (str[count]=='T' || str[count]=='t')
        {
            RevStr[strlen(str)-1-count]='A';
        }
        if (str[count]=='G' || str[count]=='g')
        {
            RevStr[strlen(str)-1-count]='C';
        }
        if (str[count]=='N' || str[count]=='n')
        {
            RevStr[strlen(str)-1-count]='N';
        }
    }
    RevStr[strlen(str)]='\0';
    return (RevStr);
    //return 0;
}



int read_OEA_Cluster(FILE * readfile)
{
    //FILE *f;                                                                                                                                           
    float temp;
    int position;
    char readchr[5];
    char * readname;
    char * seq;
		char * readstr;
		char strand;
    char strand_2;
		int maxLoc=0, minLoc=1000000000;
    int clusterId;
    readname = (char*) malloc(sizeof(char) * 1000);
    seq = (char*) malloc(sizeof(char) * 250);
    readstr = (char*) malloc(sizeof(char) * 250);
	





	for (int count=0; count<2*countOEA; count++)
    {
      maxLoc=0;
      minLoc=1000000000;
 //     printf("L96\n");
			fscanf(readfile,"%i %c\n", &clusterId, &strand);

			fscanf(readfile,"%s\n", readname);
//			fprintf(stdout, "%i %c\n", clusterId, strand);
			
//			fprintf(stdout, "%s\n", readname);

  while(strcmp(readname,"END")!=0)
  {
//      fprintf(stdout, "HELLO %s \n", readname);
			fscanf(readfile, "%s\n%i\n%c\n%f\n%s\n ", readchr, &position, &strand_2, &temp, seq);

     if (maxLoc<position)
      {
    maxLoc=position;
      }
      if (minLoc>position)
      {
    minLoc=position;
      }
      
			fscanf(readfile,"%s\n", readname);

 //    fprintf(stdout, "BYE %s \n", readname);

  }
  


if (strand=='F')
	{
		//	printf("FFFFF\n");
	    list_OEA_Cluster[clusterId-1].insideStart=maxLoc;
	    list_OEA_Cluster[clusterId-1].outsideStart=minLoc;
	}
	else if (strand=='R')
	{

//			printf("RRRR");

	    list_OEA_Cluster[clusterId-1].insideEnd=minLoc;
	    list_OEA_Cluster[clusterId-1].outsideEnd=maxLoc;
	}
	strcpy(list_OEA_Cluster[clusterId-1].chrName, readchr);
		

    } // END OF FOR


}


int read_NoHit_Seq(FILE *fp)
{
    char *tempString=(char *) malloc(2000*sizeof(char));
    NoHitSeq=(char **) malloc(countNoHit*sizeof(char *));
    char *tempStr2=(char *) malloc(40000*sizeof(char));
//    char *tempStr3=(char *) malloc(40000*sizeof(char));
    int NoHitId;

   NoHitId=0;
		 //    printf("L140\n");
    for (int count=0; count<countNoHit; count++)
    {
//	fscanf(fp,"%s\n", tempStr3);
//	NoHitId++;

	fgets(tempString,2000,fp);
	fscanf(fp,"%s\n", tempStr2);
	NoHitSeq[count]=(char *) malloc((strlen(tempStr2)+2)*sizeof(char));
	//	NoHitSeq[count][strlen(tempStr2)]='\0';
	strcpy(NoHitSeq[count], tempStr2);

	//	NoHitSeq[count][strlen(tempStr2)]='\0';
	//canf(fp,"\n");
    }
    //    printf("L153\n");
    return 0;
}


int read_OEA_Seq(FILE *fp)
{
    int OEA_Id;
    char orient;
    char OEA_Seq[2000];
    while (fscanf(fp,"%i %c\n",&OEA_Id, &orient)!=EOF)
    {
      //printf("%i\n", OEA_Id);
	fscanf(fp,"%s\n", OEA_Seq);
	if (orient=='F')
	{
	    strcpy(list_OEA_Cluster[OEA_Id-1].OEA_1, OEA_Seq);
	}
	else
	{
	    strcpy(list_OEA_Cluster[OEA_Id-1].OEA_2, OEA_Seq);
	}
    }

}

int read_Matrix(FILE *fp)
{
    int OEA_Id, NoHit_Id, score, orient;
  	for (int i=0; i<countOEA; i++)
			for (int j=0; j<countNoHit; j++)
			{  
	
	fscanf(fp,"%i %i %i %i\n", &OEA_Id, &NoHit_Id, &score, &orient);
    
		//fprintf(stdout, "%i\n", OEA_Id);
	matrixWeights[OEA_Id][NoHit_Id]=score;
	matrixOrient[OEA_Id][NoHit_Id]=orient;
	if (score>minScoreMatch)
	  {
	    listHighScoreMatch[countHighScoreMatch].OEA_Id=OEA_Id;
	    listHighScoreMatch[countHighScoreMatch].NoHit_Id=NoHit_Id;
	    listHighScoreMatch[countHighScoreMatch].score=score;
	    listHighScoreMatch[countHighScoreMatch].matchOrient=orient;
	    countHighScoreMatch++;
	  }
    }


}


int greedyMatching()
{

  int maxScore=0, maxHitId;
    int maxNoHit, maxOEAId;
    int maxOrient;
    int count;
    do{
      /*maxScore=0;
	for (int count=0; count<countNoHit; count++)
	{
	    for (int count2=0; count2<countOEA; count2++)
	    {
		if (maxScore>matrixWeights[count][count2])
		{
		    maxScore=matrixWeights[count][count2];
		    maxNoHit=count;
		    maxOEAId=count2;
		}
	    }
	}
      */
      
      maxScore=0;
      for (int count=0; count<countHighScoreMatch; count++)
	{
	  if (maxScore<listHighScoreMatch[count].score && NHIndexMarked[listHighScoreMatch[count].NoHit_Id]==0 && OEAIndexMarked[listHighScoreMatch[count].OEA_Id]==0)
	    {
	      maxScore=listHighScoreMatch[count].score;
	      maxNoHit=listHighScoreMatch[count].NoHit_Id;
	      maxOEAId=listHighScoreMatch[count].OEA_Id;
	      maxOrient=listHighScoreMatch[count].matchOrient;
	      maxHitId=count;
	    }
	}
      
      if(maxScore>0)
	{
	  if (maxOrient==1)
	    {
	       //printf("%s %s %s %s %i %i\n", list_OEA_Cluster[maxOEAId].OEA_1, list_OEA_Cluster[maxOEAId].OEA_2, NoHitSeq[maxNoHit], list_OEA_Cluster[maxOEAId].chrName, list_OEA_Cluster[maxOEAId].insideStart, list_OEA_Cluster[maxOEAId].insideEnd);

	      printf(">OEA_%i.1(%s %i)\n%s\n>OEA_%i.2(%s %i)\n%s\n>NoHit_%i\n%s\n", maxOEAId, list_OEA_Cluster[maxOEAId].chrName, list_OEA_Cluster[maxOEAId].insideStart, list_OEA_Cluster[maxOEAId].OEA_1, maxOEAId, list_OEA_Cluster[maxOEAId].chrName, list_OEA_Cluster[maxOEAId].insideEnd, list_OEA_Cluster[maxOEAId].OEA_2, maxNoHit, NoHitSeq[maxNoHit]);

	    }
	  else
	    {
	     
	      printf(">OEA_%i.1(%s %i)\n%s\n>OEA_%i.2(%s %i)\n%s\n>NoHit_%i\n%s\n", maxOEAId, list_OEA_Cluster[maxOEAId].chrName, list_OEA_Cluster[maxOEAId].insideStart, list_OEA_Cluster[maxOEAId].OEA_1, maxOEAId,list_OEA_Cluster[maxOEAId].chrName, list_OEA_Cluster[maxOEAId].insideEnd, list_OEA_Cluster[maxOEAId].OEA_2, maxNoHit, reverseComp(NoHitSeq[maxNoHit]));


//	     printf("%s %s %s %s %i %i\n", list_OEA_Cluster[maxOEAId].OEA_1, list_OEA_Cluster[maxOEAId].OEA_2, reverseComp(NoHitSeq[maxNoHit]), list_OEA_Cluster[maxOEAId].chrName, list_OEA_Cluster[maxOEAId].insideStart, list_OEA_Cluster[maxOEAId].insideEnd);
	    }
	}



      //listHighScoreMatch[maxHitId].score=0;
      NHIndexMarked[maxNoHit]=1;
      OEAIndexMarked[maxOEAId]=1;

	
	/*for (int count2=0; count2<countOEA; count2++)
	{
	    matrixWeights[maxNoHit][count2]=0;
	}
	for (int count2=0; count2<countNoHit; count2++)
	{
	    matrixWeights[count2][maxOEAId]=0;
	}
	*/
	
    }while(maxScore>30);
}



int main(int argv, char **argc)
{
    char *fileName_OEA, *fileName_NoHit, *fileName_OEA_S, *fileName_Matrix;
    FILE *fp1, *fp2, *fp3, *fp4;
    fileName_OEA=(char *) malloc(150*sizeof(char));
    fileName_NoHit=(char *) malloc(150*sizeof(char));
    fileName_OEA_S=(char *) malloc(150*sizeof(char));
    fileName_Matrix=(char *) malloc(150*sizeof(char));
//    int countOEA;
//    int countNoHit;

    for (int argCount=1; argCount < argv; argCount++)
    {
	if (strcmp(argc[argCount], "-fOEA_C")==0) // the name of the file contating OEA                                                   
	    strcpy(fileName_OEA,argc[argCount+1]);
	if (strcmp(argc[argCount], "-fNH")==0) //the name of the file containing no_hitters                                             
	  strcpy(fileName_NoHit, argc[argCount+1]);
	if (strcmp(argc[argCount],"-fOEA_S")==0)
	    strcpy(fileName_OEA_S, argc[argCount+1]);
	if (strcmp(argc[argCount], "-fMatrix")==0)
	    strcpy(fileName_Matrix, argc[argCount+1]);
	
 	if (strcmp(argc[argCount], "-OEAcount")==0) //the number of OEA contigs                                                            
	    countOEA = atoi(argc[argCount+1]);
	if (strcmp(argc[argCount], "-NHcount")==0) // the number of No_Hitter contigs                                                
	    countNoHit= atoi(argc[argCount+1]);
    };
    
    
    list_OEA_Cluster=(OEA_Cluster *) malloc(countOEA*sizeof(OEA_Cluster));
    OEAIndexMarked = (int *) malloc((countOEA+1)*sizeof(int));
    NHIndexMarked= (int *) malloc((countNoHit+1)*sizeof(int));    
    
    //    printf("%s %s %s %s\n", fileName_OEA, fileName_NoHit, fileName_OEA_S, fileName_Matrix);

    fp1=fopen(fileName_OEA,"r");
   
    fp2=fopen(fileName_NoHit, "r");
    fp3=fopen(fileName_OEA_S, "r");
    fp4=fopen(fileName_Matrix, "r");
//	fprintf(stdout, "READING CLUSTER FILE ....\n"); 
 
		 read_OEA_Cluster(fp1);

//	fprintf(stdout, "READING ORPHAN FILE ....\n"); 

   read_NoHit_Seq(fp2);//    read_OEA_Seq(fp3);
//injaa TC
    
    matrixWeights=(int **) malloc(countOEA*sizeof(int *));
    matrixOrient=(int **) malloc(countOEA*sizeof(int *));
    
    for (int count=0; count<countOEA; count++)
    {
	matrixWeights[count]=(int *) malloc(countNoHit*sizeof(int));
	matrixOrient[count]=(int *) malloc(countNoHit*sizeof(int));
	OEAIndexMarked[count]=0;
    }


    for (int count=0; count<countOEA; count++)
    {
	for (int count2=0; count2<countNoHit; count2++)
	{
	    matrixWeights[count][count2]=0;
	    matrixOrient[count][count2]=0;
	}
    }

    for (int count=0; count<countNoHit+1; count++)
	{	
		NHIndexMarked[count]=0;	
	}
	
//	fprintf(stdout, "READING MATRIX FILE ....\n"); 
   read_Matrix(fp4);

//	fprintf(stdout, " MATCHING ....\n"); 
    greedyMatching();
 
}
