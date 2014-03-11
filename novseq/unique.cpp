#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char str[100], tempStr[100];

int main(int agrv, char *argc[])
{
  FILE *fp=fopen(argc[1],"r");
  //char str[100], tempStr[100];
  
  while (fscanf(fp,"%s\n", str)!=EOF)
    {
      if (strcmp(str,tempStr)!=0)
	{
	  printf("%s \n", str);
	  strcpy(tempStr, str);
	}
    }

}
