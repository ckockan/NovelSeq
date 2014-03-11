#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

using namespace std;

int compare(const void *e1, const void *e2)
{
  // const char *se1=(const void *) e1;
  //const char *se2=(const void *) e2;
  //  printf("%s\n", (const char *) e1);
  //  return 1;
  
  const char *se1 = (const char *) e1;
  printf("%s\n", se1);
  //return strcmp(*(char **)e1, *(char **)e2);
}




static int cmpr(const void *a, const void *b) { 
 return strcmp(*(char **)a, *(char **)b);
}

void sortstrarr(void *array, unsigned n) { 
 qsort(array, n, sizeof(char *), cmpr);
}



int main(int argv, char *argc[])

{
 
 char ** names;
 char ** names_str;
// FILE *fp=fopen(argc[1],"r");
ifstream fp(argc[1]);
names = (char **) malloc (70000000*sizeof(char *));
  names_str = (char **) malloc (70000000*sizeof(char *));
 
 	char *temp;
	char *temp1;
  temp = (char *) malloc(250*sizeof(char));
  for (int count=0; count<70000000; count++)
    {
      names[count] = (char *) malloc(250*sizeof(char));
		}
  int count=0;
  
	while(fp.getline(temp, 250))
    {
      strcpy(names[count],temp);
      count++;
  //       printf("%s\n", names[count-1]);
 //         printf("%d\n", count-1);
    
	}
  //qsort(names, count, 50*sizeof(char), compare);
  sortstrarr(names, count);  
//printf("L53 \n");
printf("%i\n", count);
  for (int count1=0; count1<count; count1++)
    {
      printf("%s\n", names[count1]);
			}
}
