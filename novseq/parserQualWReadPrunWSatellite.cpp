#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
FILE *errf;
int arrayRepeats[25][500000][2];
int arraySize[25];
const int max_read_length=150;

int findChr(char chrName[7])
{
				if (strcmp(chrName, "chr1")==0)
								return 1;
				if (strcmp(chrName, "chr2")==0)
								return 2;
				if (strcmp(chrName, "chr3")==0)
								return 3;
				if (strcmp(chrName, "chr4")==0)
								return 4;
				if (strcmp(chrName, "chr5")==0)
								return 5;
				if (strcmp(chrName, "chr6")==0)
								return 6;
				if (strcmp(chrName, "chr7")==0)
								return 7;
				if (strcmp(chrName, "chr8")==0)
								return 8;
				if (strcmp(chrName, "chr9")==0)
								return 9;
				if (strcmp(chrName, "chr10")==0)
								return 10;
				if (strcmp(chrName, "chr11")==0)
								return 11;
				if (strcmp(chrName, "chr12")==0)
								return 12;
				if (strcmp(chrName, "chr13")==0)
								return 13;
				if (strcmp(chrName, "chr14")==0)
								return 14;
				if (strcmp(chrName, "chr15")==0)
								return 15;
				if (strcmp(chrName, "chr16")==0)
								return 16;
				if (strcmp(chrName, "chr17")==0)
								return 17;
				if (strcmp(chrName, "chr18")==0)
								return 18;
				if (strcmp(chrName, "chr19")==0)
								return 19;
				if (strcmp(chrName, "chr20")==0)
								return 20;
				if (strcmp(chrName, "chr21")==0)
								return 21;
				if (strcmp(chrName, "chr22")==0)
								return 22;
				if (strcmp(chrName, "chrX")==0)
								return 23;
				if (strcmp(chrName, "chrY")==0)
								return 24;
				return 0;
}

int createTheRepeatsArray(FILE *fp2)
{
				char chroName[7];
				int startSat, endSat;
				int chrId;
				int prevEnd=-1;
				while (fscanf(fp2,"%s\t%i\t%i\n", chroName, &startSat, &endSat)!=EOF)
				{
								//      if (startSat <  prevEnd )
								//      printf("error %s %i %i\n", chroName, startSat, endSat);
								//      prevEnd=endSat;
								chrId=findChr(chroName);
								//printf("error %i %i %i %i\n", chrId, arraySize[chrId], startSat, endSat);
								arrayRepeats[chrId][arraySize[chrId]][0]=startSat;
								//      printf("L78 %i\n", arrayRepeats[1][0][0]);

								arrayRepeats[chrId][arraySize[chrId]][1]=endSat;
								arraySize[chrId]++;
				}
}


int binarySearch(int num, int chroId)
{
				//  printf("Inside binary\n");
				int maxId, minId, midId;
				maxId=arraySize[chroId]-1;
				minId=0;
				midId=(minId+maxId)/2;

				while (midId!=maxId && midId!=minId && arrayRepeats[chroId][midId][0]!=num)
				{
								//printf("%i %i %i %i %i %i %i\n", minId, maxId, midId,arrayRepeats[chroId][minId][0], arrayRepeats[chroId][maxId][0], arrayRepeats[chroId][midId][0], num);
								if (arrayRepeats[chroId][midId][0]>num)
								{
												//printf("Hello\n");
												maxId=midId;
								}
								if (arrayRepeats[chroId][midId][0]<num)
								{
												//printf("Hello\n");
												minId=midId;
								}
								midId=(maxId+minId)/2;
				}
				//  printf("Outside binary %i\n", midId);
				return midId;
}

int NotInRepeat(int f1, char chroName[7])
{
				int chrId;
				int firstId, secondId;
				chrId = findChr(chroName);
				firstId=binarySearch(f1, chrId);
				// secondId=binarySearch(f2, chrId);
				//printf("%i\n", firstId);
				//printf("Test %i %i %i %i\n", chrId, arrayRepeats[chrId][firstId][0], f1, firstId);


				while (arrayRepeats[chrId][firstId][1] > f1)
								firstId--;
				//while (arrayRepeats[chrId][secondId][1] > f2)
				//secondId--;
				//printf("Test %i %i %i\n", arrayRepeats[chrId][firstId][0], f1, firstId);
				while (arrayRepeats[chrId][firstId][0] <= f1 && firstId < arraySize[chrId])
				{
								//  printf("L123\n");
								if (firstId >= 0 && arrayRepeats[chrId][firstId][0]<= f1 && arrayRepeats[chrId][firstId][1] >= f1)
												return 0;
								firstId++;
				}
				/* while(arrayRepeats[chrId][secondId][0] <=f2 && secondId < arraySize[chrId])
					 {
					 if (secondId >= 0 && arrayRepeats[chrId][secondId][0]<= f2 && arrayRepeats[chrId][secondId][1] >= e2)
					 return 0;
					 secondId++;
					 }
				 */

				return 1;
}


int readAndTransfer(FILE *fp)
{
				char Name1[150], Name2[150], Name3[150], chrName[150], SVtype[20],qualString1[150],qualString2[150], str[max_read_length];
				char orientOne, orientTwo, qualChar1, qualChar2, qualChar3, qualChar4, temp;
				int editDist1, editDist2, totalEditDist, Error;
				float  Phred, Score;
				int firstPosL, firstPosR, secondPosL, secondPosR, finalPosL, finalPosR, insertSize;
				int testCount=0;

				//fscanf(fp, "%s\t", Name1);



				float prob=1;
				float averageQualOfReads=0;
				int numPBless5=0;
				char temp1, temp2;
				char snpType, snpQual;
				int qualPhred, snpPos;
				int count=1;
				char insDelStr[5];
				bool insDelTwo;

				while (fscanf(fp,"%s\t%s\t%i\t%c\t%i\t%f\t%g\t%s\n", Name1, chrName, &firstPosL, &orientOne, &Error, &Phred, &Score, str)!=EOF)
				{
								testCount++;

								if (NotInRepeat(firstPosL, chrName))
												printf("%s\t%s\t%d\t%c\t%d\t%f\t%g\t%s\n", Name1, chrName,firstPosL, orientOne, Error, Phred, Score, str);
				}

}

int main(int argv, char *argc[])
{
				FILE *fp, *fp2;
				fp=fopen(argc[1],"r");
				fp2=fopen(argc[2],"r");
				errf=fopen("eFile.txt","w");
				//printf("L278\n");
				for (int count=0; count<25; count++)
								arraySize[count]=0;
				createTheRepeatsArray(fp2);
				readAndTransfer(fp);
				fclose(errf);
}
