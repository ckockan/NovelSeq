#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int l1, l2, l3, r, q, s, j;
int temp, n, x;
int testCount;
char names[10000];
int names_int[10000];
int sorted[10000];
char Name1[100];

int string2int(char*digit) {
    int result = 0;
   //--- get integer portion^M
    while (*digit >= '0' && *digit <='9') {
        result = (result *10) + *digit-'0';
        digit++;
    }
   //--- get decimal point and fraction, if any.^M
    if (*digit != 0) {
        return 0;
    }
    return result;
}

int convert(char *xname) {
    int found = 0;
    
    char *stemp1;
		char *stemp2;
     // set TEMP to be the SP part^M
    int l = 0;
		int l2=-1;
    int r = 0; 
    while (xname[l] != '_' && xname[l] != '\n') {
        l++;
				r++; 
		}

		while (xname[r] != '.' && xname[r] != '\n') {
			r++;
		}

	
        	stemp1 = (char*) malloc(r-l+1);
        	strncpy(stemp1, xname+l+1, r-l);
        	stemp1[r-1] = '\0';
 			//		printf("%s %d\n", stemp1, string2int(stemp1));
      	 
  	return string2int(stemp1);
}

int main(int argv, char *argc[])
{
FILE *fp;
fp = fopen(argc[1], "r");


 
        while (fscanf(fp,"%s\n", Name1)!=EOF)
        {
       					names_int[testCount] = convert(Name1); 				
				//				printf("%d \n", names_int[testCount]);				
								testCount++;
				}

			
			for (s=0; s <testCount; s++)
				for (j=0; j<s; j++)
				if (names_int[s] < names_int[j])
				{
						temp = names_int[s]; names_int[s]=names_int[j]; names_int[j]=temp;
				}
			
			for (s=0; s<testCount; s++)
			{	
				printf("g1k-sc-NA19435-A_%d.sam_OEA.clean.bz2\n", names_int[s]);
			}
}
