

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> 
#include <math.h>
#include <sys/param.h>
#include <time.h>

clock_t start, end;
double cpu_time_used;

#ifndef max
    #define max(a,b) ((a)>(b) ? (a) : (b))
#endif

const int max_OEA_Length=3000;
const int max_NoHit_Length=100000;
int countOEA_Added;
int countNH_Added;

int **matrixOEA_NoHit;//matrixOEA_NoHit[x][y] : is the OEA(x) and NoHit(y)
int **OEA_NoHit_RorF; //OEA_NoHit_RorF: is the matrix which shows if the 

int *r;//
//NoHit is best matched with the OEA clsuer in its
                        //Forward (F) or Reverse (R) .. F=1, R=2;
int maxLen_Btw_Pairs;
char *OEA_S_R, *OEA_S_F, *No_Hit_S_L, *No_Hit_S_R;

int **alignMatrix;// the row is OEA and the column is no-hitters

const int gap_penalty = 16;
const int mismatch_penalty = 4;


int alignType; //1 : local, 2:global
typedef struct OEA_Strings{ //Both the OEA_F and OEA_R are transfered in a way that are mapping to the Forward(+) strand
  char *OEA_F;
  char *OEA_R;
  int OEA_F_len;
  int OEA_R_len;
  int id;
};
int OEAtoCoverStart=0;
int OEAtoCoverStop=0;


typedef struct NoHit_Strings{
  int id;
  char *NoHit;
  int NoHit_Len;
};

NoHit_Strings *NoHitArray;

OEA_Strings *OEAArray;

int max3Way(int a, int b, int c)
{
  if (a >= b && a >= c )
    return a;
  if (b >=a && b >= c)
    return b;
  if (c >= a && c>= b)
    return c;
}

/*int max(int a, int b)
{
  if (a<=b)
    return b;
  else return a;
}*/

int reverseComp_2(char *str, char *str2)
{
    //char *RevStr;
	//RevStr=(char *) malloc((strlen(str)+1)*sizeof(char));
	for (int count=0; count<strlen(str); count++)
	{
		if (str[count]=='A' || str[count]=='a')
		{
			str2[strlen(str)-1-count]='T';
		}
		if (str[count]=='C' || str[count]=='c')
		{
			str2[strlen(str)-1-count]='G';
		}
		if (str[count]=='T' || str[count]=='t')
		{
			str2[strlen(str)-1-count]='A';
		}
		if (str[count]=='G' || str[count]=='g')
		{
			str2[strlen(str)-1-count]='C';
		}
		if (str[count]=='N' || str[count]=='n')
		{
			str2[strlen(str)-1-count]='N';
		}
	}
	str2[strlen(str)]='\0';
//	return (RevStr);
	return 0;
}

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

int squareMatrix(int rowCount, int colCount)
{
  //This function gets a two dimentional matrix of size rowCount*colCount and return a 1-dimentional 
  // matrix of size max(rowCount,colCount)*max(rowCount, colCount).

  int maxRow_Col=max(rowCount, colCount);
  r=(int *) malloc(maxRow_Col*maxRow_Col*sizeof(int));
  
  for (int count=0; count<maxRow_Col; count++)
    {
      for (int count2=0; count2<maxRow_Col; count2++)
	{
	  if (count<rowCount && count2<colCount)
	    {
	      r[count*maxRow_Col+count2]=matrixOEA_NoHit[count][count2];
	    }
	  else
	    {
	      r[count*maxRow_Col+count2]=0;
	    }

	}
    }
  /*r=(int **) malloc(maxRow_Col*sizeof(int *));
  for (int count=0; count<maxRow_Col; count++)
    {
      r[count]=(int *) malloc(maxRow_Col*sizeof(int));
      
    }
  for (int count=0; count<maxRow_Col; count++)
    {
      for (int count2=0; count2<maxRow_Col; count2++)
	{
	  if (count<rowCount && count2<colCount)
	    {
	      r[count][count2]=matrixOEA_NoHit[count][count2];
	    }
	  else
	    {
	      r[count][count2]=0;
	    }
	}
	}*/
  return maxRow_Col;
}


int localAlignLeft()
{
  ///int **alignMatrix;
  for (int count=0; count<maxLen_Btw_Pairs; count++)
    {
      if (alignType==1)
	{
	  alignMatrix[count][0]=0;
	  alignMatrix[0][count]=0;
	}
      else if (alignType==2)
	{
	  alignMatrix[count][0]=0;
          alignMatrix[0][count]=0;
	}
    }

//		printf("%s %s\n", OEA_S_R, No_Hit_S_L);

  int str_len_OEA_S_R=strlen(OEA_S_R);
  int str_len_No_Hit_S_L=strlen(No_Hit_S_L);
//printf("%s %s\n", OEA_S_R, No_Hit_S_L);
  for (int row=1;row < str_len_OEA_S_R; row++) 
    {
      for (int col=1; col < str_len_No_Hit_S_L ; col++)
	{
	  if (OEA_S_R[row]==No_Hit_S_L[col])
	    {
	      alignMatrix[row][col]=max3Way(alignMatrix[row-1][col]-gap_penalty, alignMatrix[row][col-1]-gap_penalty, 
					   alignMatrix[row-1][col-1]+1);
	      if (alignType==1)
			{
		  		alignMatrix[row][col]=max(0, alignMatrix[row][col]);
			}
	    }
	  else
	    {
	      alignMatrix[row][col]=max3Way(alignMatrix[row-1][col]-gap_penalty,alignMatrix[row][col-1]-gap_penalty, 
					   alignMatrix[row-1][col-1]-mismatch_penalty);
	      if (alignType==1)
			{	
                  alignMatrix[row][col]=max(0,alignMatrix[row][col]);
            }
	    }
//		fprintf(stdout, "align[%d][%d] = %d \n", row, col,alignMatrix[row][col]);
	}
    }

  int maxValueRet=0;
  /*
  for (int row=1; row<strlen(OEA_S_R); row++)
    {
      if (alignMatrix[row][strlen(No_Hit_S_L)-1] > maxValueRet)
	maxValueRet=alignMatrix[row][strlen(No_Hit_S_L)-1];
    }
  for (int col=1; col<strlen(No_Hit_S_L); col++)
    {
      if (alignMatrix[strlen(OEA_S_R)-1][col] > maxValueRet)
	maxValueRet=alignMatrix[strlen(OEA_S_R)-1][col];
	}*/
//  int str_len_OEA_S_R=strlen(OEA_S_R);
//  int str_len_No_Hit_S_L=strlen(No_Hit_S_L);
  for (int row=1; row<str_len_OEA_S_R; row++)
    {
      for (int col=1; col< str_len_No_Hit_S_L; col++)
	{
	  if (alignMatrix[row][col] > maxValueRet)
	    maxValueRet=alignMatrix[row][col];
	//	printf("%i ", alignMatrix[row][col]);
	}
	//	printf("\n");
    }
//	printf("final : %i\n", maxValueRet);
//printf("%i\n", maxValueRet);
  return maxValueRet;
}


int localAlignRight()
{
  
  for (int count=0; count<maxLen_Btw_Pairs; count++)
    {
      if (alignType==1)
	{
	  alignMatrix[count][0]=0;
	  alignMatrix[0][count]=0;
	}
      else if (alignType==2)
	{
	  alignMatrix[count][0]=0;
	  alignMatrix[0][count]=0;
	}
    }
 // printf("%s %s\n", OEA_S_F, No_Hit_S_R);
  int str_len_OEA_S_F = strlen(OEA_S_F);
  int str_len_No_Hit_S_R = strlen(No_Hit_S_R);
  //  printf("%s %s\n", OEA_S_F, No_Hit_S_R);
  for (int row=1;row < str_len_OEA_S_F; row++)
//  for (int row=1;row < strlen(OEA_S_F); row++)
    {
	for (int col=1; col < str_len_No_Hit_S_R ; col++)
	//for (int col=1; col < strlen(No_Hit_S_R) ; col++)
	{
	  if (OEA_S_F[row]==No_Hit_S_R[col])
	    {
	      //alignMarix[row][col]=min3Way(alignMatrix[row-1][col], alignMatrix[row][col-1], alignMatrix[row-1][col-1]);
	      alignMatrix[row][col]=max3Way(alignMatrix[row-1][col]-gap_penalty, alignMatrix[row][col-1]-gap_penalty, 
					   alignMatrix[row-1][col-1]+1);
	      if (alignType==1)
		{
		  alignMatrix[row][col]=max(0, alignMatrix[row][col]);
		}
	    }
	  else
	   {
	     //alignMarix[row][col]=min3Way(alignMatrix[row-1][col],alignMatrix[row][col-1], alignMatrix[row-1][col-1]+1);
	     alignMatrix[row][col]=max3Way(alignMatrix[row-1][col]-gap_penalty,alignMatrix[row][col-1]-gap_penalty, 
					  alignMatrix[row-1][col-1]-mismatch_penalty);
	     if (alignType==1)
	       {
		 alignMatrix[row][col]=max(0,alignMatrix[row][col]);
	       }
	   }
	  
	
//		fprintf(stdout, "align[%d][%d] = %d \n", row, col,alignMatrix[row][col]);
}
    }

  int maxValueRet=0;

  /*
  for (int row=1; row<strlen(OEA_S_F); row++)
    {
      if (alignMatrix[row][strlen(No_Hit_S_R)-1] > maxValueRet)
	maxValueRet=alignMatrix[row][strlen(No_Hit_S_R)-1];
    }
  for (int col=1; col<strlen(No_Hit_S_R); col++)
    {
      if (alignMatrix[strlen(OEA_S_F)-1][col] > maxValueRet)
	maxValueRet=alignMatrix[strlen(OEA_S_F)-1][col];
    }
  */
  //int str_len_OEA_S_F=strlen(OEA_S_F);
  //int str_len_No_Hit_S_R=strlen(No_Hit_S_R);
  for (int row=1; row < str_len_OEA_S_F; row++)
    {
      for (int col=1; col < str_len_No_Hit_S_R; col++)
	{
	  if (alignMatrix[row][col]>maxValueRet)
	    maxValueRet=alignMatrix[row][col];
	//	printf("%i ", alignMatrix[row][col]);
	}
//	printf("\n");
    }
  //printf("final: %i\n", maxValueRet);
  return maxValueRet;
  
}


int readOEAs(int countOEA, char *fileName_OEA)
{

	FILE *fp;
  int OEA_id;
  char *strOEA_F, *strOEA_R;
  strOEA_F=(char *) malloc(max_OEA_Length * sizeof(char));
  strOEA_R=(char *) malloc(max_OEA_Length * sizeof(char));
  fp=fopen(fileName_OEA, "r");
  countOEA_Added=0;
	OEA_id=0;

//	 fprintf(stdout, "I AM HERE 3");
  
	while (countOEA>0)
    {
  
      fscanf(fp,">%i R\n", &OEA_id);
      fscanf(fp,"%s\n", strOEA_R);// The sequence itself is from forward strand
	    fscanf(fp,">%i F\n", &OEA_id);
      fscanf(fp,"%s\n", strOEA_F);//The sequence it self is from reverse trand
    	 
		 if (strlen(strOEA_F) <= maxLen_Btw_Pairs && strlen(strOEA_R) <= maxLen_Btw_Pairs)
	{
	
		OEAArray[countOEA_Added].OEA_F=(char *) malloc((strlen(strOEA_F)+1)*sizeof(char));
	  OEAArray[countOEA_Added].OEA_R=(char *) malloc((strlen(strOEA_R)+1)*sizeof(char));
	  OEAArray[countOEA_Added].OEA_F_len = strlen(strOEA_F);
	  OEAArray[countOEA_Added].OEA_R_len = strlen(strOEA_R);
	  strcpy(OEAArray[countOEA_Added].OEA_R, reverseComp(strOEA_R));
	  strcpy(OEAArray[countOEA_Added].OEA_F, strOEA_F);
	  OEAArray[countOEA_Added].id=OEA_id;
	  countOEA_Added++;
	} 
      countOEA--;
    }


	//printf("%s %s\n", OEAArray[181].OEA_F, OEAArray[181].OEA_R);

}


int readNo_Hits(int countNo_Hit, char *fileName_NoHit)
{
  FILE *fp;
  int NoHit_id;
  int countNo_HitAdded=0;
  char *NoHit_Str;
  NoHit_Str=(char *) malloc(max_NoHit_Length*sizeof(char));
  char *tempString=(char *) malloc(10000*sizeof(char));
  NoHit_id=0;
  int temp1, temp2;
  fp=fopen(fileName_NoHit,"r");
  while (countNo_Hit>0)
    {
      
//	 fscanf(fp,">a%i", &NoHit_id);
	// NoHit_id++;	
	 fgets(tempString,2000,fp);
     // printf("Complete string %s\n", tempString);
	  fscanf(fp,"%s\n", NoHit_Str);
	  fscanf(fp,"\n");
	  //printf("%i %s %s\n", NoHit_id, tempString, NoHit_Str);
      strcpy(NoHitArray[countNo_HitAdded].NoHit, NoHit_Str);
      NoHitArray[countNo_HitAdded].NoHit_Len=strlen(NoHit_Str);
      NoHitArray[countNo_HitAdded].id=NoHit_id++;
      countNo_HitAdded++;
      countNo_Hit--;
	   
    }
    
}


int my_StrCopy(char *str2, char *str1)
{
    for (int count=0;count<maxLen_Btw_Pairs; count++)
    {
	if (str1[count]!='\0')
	{
	    str2[count]=str1[count];
	}else 
	{
	    str2[count]='\0';
	    return 0;
	}
    }
}



int createTheMatrix(int countOEA, int countNoHit)
{
//int bestMatch=0;
//int bestMatchOEA=0;
//int bestMatchNoHit=0;
  /*The matrixOEA_NOHit holds the bipartite graph of size OEA*NoHit*/
	
  matrixOEA_NoHit=(int **)malloc(countOEA*sizeof(int *));
  OEA_NoHit_RorF=(int **) malloc(countOEA*sizeof(int *));
  for (int count=0; count<countOEA; count++)
    {
      matrixOEA_NoHit[count]=(int *)malloc(countNoHit*sizeof(int));
      OEA_NoHit_RorF[count]=(int *) malloc(countNoHit*sizeof(int));
    }
  
  char *NoHit_S_L_Temp=(char *) malloc((maxLen_Btw_Pairs+1)*sizeof(char));
  char *NoHit_S_R_Temp=(char *) malloc((maxLen_Btw_Pairs+1)*sizeof(char));

  for (int count=0; count<countNoHit; count++)
  {
      //printf("More\n");
      // printf("%i\n", count);
      if (NoHitArray[count].NoHit_Len>maxLen_Btw_Pairs)
      {
	  for (int count2=0; count2<maxLen_Btw_Pairs-1; count2++)
	  {
	      //No_Hit_S_L[count2]=NoHitArray[count].NoHit[count2];	
	      //printf("%c", No_Hit_S_L[count2]);
	      No_Hit_S_R[count2]=NoHitArray[count].NoHit[NoHitArray[count].NoHit_Len - maxLen_Btw_Pairs + count2];
	      //No_Hit_S_R[count2]='c';
	      No_Hit_S_L[count2]=NoHitArray[count].NoHit[count2];
	      //No_Hit_S_L[count2]='a';
	      
	  }
	  No_Hit_S_L[maxLen_Btw_Pairs]='\0';
	  No_Hit_S_R[maxLen_Btw_Pairs]='\0';
      }
      else
      {
	  strcpy(No_Hit_S_L, NoHitArray[count].NoHit);
	  No_Hit_S_L[strlen(NoHitArray[count].NoHit)]='\0';
	  strcpy(No_Hit_S_R, NoHitArray[count].NoHit);
	  No_Hit_S_L[strlen(NoHitArray[count].NoHit)]='\0';
      }
      //    printf("%i\n", count);
      //printf("MyMy %s %s %i %s\n", No_Hit_S_L, No_Hit_S_R, maxLen_Btw_Pairs, NoHitArray[count].NoHit); 
      int NoHitForwardStringWeight=0;
      int NoHitReverseStringWeight=0;
      
      for (int count2=OEAtoCoverStart; count2<OEAtoCoverStop; count2++)
      {
	  //printf("%i\n", count2);
//	    printf("Copy Time\n");
//	    start=clock();
	  NoHitForwardStringWeight=0;
	  my_StrCopy(OEA_S_R, OEAArray[count2].OEA_R);
	  //strcpy(OEA_S_R, OEAArray[count2].OEA_R);
	  my_StrCopy(OEA_S_F, OEAArray[count2].OEA_F);
//	  end=clock();
	  //strcpy(OEA_S_F, OEAArray[count2].OEA_F);
	  //printf("OEA %i\n", count2);
//	  cpu_time_used=((double) (end-start));
//	  printf("Run TIme First %f\n", cpu_time_used);
//	  start=clock();
//	  printf("%s %s\n", OEA_S_R, OEA_S_F);
	  NoHitForwardStringWeight=localAlignLeft();
	  //printf("\n%i\n",NoHitForwardStringWeight);
	  NoHitForwardStringWeight+=localAlignRight();
	//printf("\n%i\n", NoHitForwardStringWeight);
	  matrixOEA_NoHit[count2][count]=NoHitForwardStringWeight;
//	  end=clock();
//	  cpu_time_used=((double) (end-start));
//	  printf("Run Time Second %f\n", cpu_time_used);
	//printf("%s %s %s %s %i\n",OEA_S_R, OEA_S_F, No_Hit_S_L, No_Hit_S_R, matrixOEA_NoHit[count2][count]);
	 
	}
      strcpy(NoHit_S_R_Temp, No_Hit_S_R);
      reverseComp_2(No_Hit_S_L, No_Hit_S_R);
      reverseComp_2(NoHit_S_R_Temp, No_Hit_S_L);
      //printf("%i %i %i\n", OEAtoCoverStart, OEAtoCoverStop, count);
      for (int count2=OEAtoCoverStart; count2<OEAtoCoverStop; count2++)
	{
	//	printf("%i\n", count2);
	  NoHitReverseStringWeight=0;
	  my_StrCopy(OEA_S_R, OEAArray[count2].OEA_R);
	  //strcpy(OEA_S_R, OEAArray[count2].OEA_R);
	  my_StrCopy(OEA_S_F, OEAArray[count2].OEA_F);
	  //strcpy(OEA_S_F, OEAArray[count2].OEA_F);
	  NoHitReverseStringWeight=localAlignLeft();
	//printf("\n %i\n", NoHitReverseStringWeight);
	  NoHitReverseStringWeight+=localAlignRight();
	//printf("\n %i\n", NoHitReverseStringWeight);
	  if (matrixOEA_NoHit[count2][count] < NoHitReverseStringWeight)
	    {
	      matrixOEA_NoHit[count2][count]=NoHitReverseStringWeight;
	      OEA_NoHit_RorF[count2][count]=2;
	    }
	  else
	    {
	      OEA_NoHit_RorF[count2][count]=1;
	    }
	}
      
    }
}

int **r2;

int main( int argv, char **argc)
{
	int editDist;
  char fileName_OEA[200];
  char fileName_NoHit[200];
  int countOEA, countNoHit;
  FILE *fp1, *fp2;
	int bestMatch=0;
	int bestMatchOEA=0;
	int bestMatchNH=0;


  if (argv < 8) {
      printf("Not enough arguments passed. Usage: createScoreMatrix  <maxLen> <OEA contigs> <orphan contigs> <OEA count> <orphan count> <OEAStartCover> <OEAStopCover> \n");
     	return -1;
	}

	maxLen_Btw_Pairs=atoi(argc[1]);
	strcpy(fileName_OEA,argc[2]); 
	strcpy(fileName_NoHit, argc[3]);
	countOEA = atoi(argc[4]);
	countNoHit= atoi(argc[5]);
	OEAtoCoverStart=atoi(argc[6]);
	OEAtoCoverStop=atoi(argc[7]);

      
	alignMatrix=(int **) malloc(maxLen_Btw_Pairs*sizeof(int *));
	
	for(int count=0; count<maxLen_Btw_Pairs; count++)
	{
		alignMatrix[count]=(int *) malloc(maxLen_Btw_Pairs*sizeof(int));
	}
  
  OEAArray = (OEA_Strings *) malloc(countOEA * sizeof(OEA_Strings));
  NoHitArray = (NoHit_Strings *) malloc(countNoHit * sizeof(OEA_Strings));
  
  for (int count=0; count<countOEA; count++)
    {
      OEAArray[count].OEA_F = (char *) malloc(maxLen_Btw_Pairs*sizeof(char));
      OEAArray[count].OEA_R = (char *) malloc(maxLen_Btw_Pairs*sizeof(char));
    }
  for (int count=0; count<countNoHit; count++)
    {
      NoHitArray[count].NoHit=(char *) malloc(max_NoHit_Length*sizeof(char));
    }

  fp1=fopen(fileName_NoHit,"r");
  fp2=fopen(fileName_OEA,"r");
  
  OEA_S_R = (char *) malloc((maxLen_Btw_Pairs+1)*sizeof(char));
  OEA_S_F = (char *) malloc((maxLen_Btw_Pairs+1)*sizeof(char));
  No_Hit_S_L = (char *) malloc((maxLen_Btw_Pairs+1)*sizeof(char));
  No_Hit_S_R = (char *) malloc((maxLen_Btw_Pairs+1)*sizeof(char));
 





//	for (int count=0; count<countOEA_Added; count++)
//	{
//		printf("%i %s %s\n", OEAArray[count].id, OEAArray[count].OEA_F, OEAArray[count].OEA_R);
//	}
	

//fprintf(stdout, "I AM HERE 3\n");
	readNo_Hits(countNoHit, fileName_NoHit);
	
//fprintf(stdout, "I AM HERE 3\n");
  
	readOEAs(countOEA, fileName_OEA);
	alignType=1;
  
	createTheMatrix(countOEA_Added, countNoHit);
  
//fprintf(stdout, "SALAAM");
	//fprintf(stdout, "I AM HERE mismatch_penalty\n");
	for (int count=OEAtoCoverStart; count<OEAtoCoverStop; count++)
	{
	  		for (int count2=0; count2<countNoHit; count2++)
		{

			if (OEA_NoHit_RorF[count][count2]==1)
			 // printf("%i %i %i %i %s %s %s\n", count, count2, matrixOEA_NoHit[count][count2], 2,NoHitArray[count2].NoHit, OEAArray[count].OEA_R, OEAArray[count].OEA_F );
			printf("%i %i %i %i\n", count, count2, matrixOEA_NoHit[count][count2], 2);
			else printf("%i %i %i %i\n", count, count2, matrixOEA_NoHit[count][count2], 1);
//	if (bestMatch<matrixOEA_NoHit[count][count2])
//				bestMatch=matrixOEA_NoHit[count][count2];
		}

}

}
