#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
using namespace std;

void addToType2(fstream& out, char* name, char* chro, int location, char orientation, float weight, int clusterId, char* sequence)
{
   out << name << endl;
   out << sequence << endl;
   out << chro << endl;
   out << location << endl;
   out << weight << endl;
   out << clusterId << endl;
   out << "END" << endl;
}


void createReadNames(fstream& type1, char* nameStr)
{
   fstream out(nameStr, fstream::out);

   char line[1024];
   char name[100];
   char token[100];

   bool ignoreFirstLine = true;
   while(!type1.eof())
   {
      if (ignoreFirstLine)
      {
         type1.getline(line, 1024);
         ignoreFirstLine = false;
      }

      type1 >> name;
			if (!strcmp(name, "END"))
      {
         type1.getline(line, 1024); //ignoring the end of line enter
         ignoreFirstLine = true;
         continue;
      }
      type1 >> token;
      type1 >> token;
      type1 >> token;
      type1 >> token;

      out << name << endl;
   		cout << name << endl;
		}
   out.close();
}


void mergeToType2(char* fsFileName, char* type1Name, char* type2Name)
{
   fstream fs(fsFileName, fstream::in);
   fstream type1(type1Name, fstream::in);
   fstream type2(type2Name, fstream::out);

   bool ignoreFirstLine = true;
   char fsName[100];
   char token[100];
   char seq[200];
   char line[1024];
   char name[100];

   bool isF = true;
   int clusterId = 0;
   while(!type1.eof())
   {
      if (ignoreFirstLine)
      {
         type1.getline(line, 1024);
         ignoreFirstLine = false;

         if (type1.fail() || type1.bad()) //if end of file, return;
            break;

         if (isF)
            clusterId++;
         type2 << clusterId << ((isF) ? " F" : " R") << endl;
         isF = !isF;
      }


      type1 >> name;

      if (!strcmp(name, "END"))
      {
         type2 << "END" << endl;
         type1.getline(line, 1024); //ignoring the end of line enter
         ignoreFirstLine = true;
         continue;
      }


      //farz mikonim felan ke yaroo name-e tooye fs hast
      char tmp;

      fs >> tmp >> fsName >> seq;
      if (strcmp(fsName, name)){
         cerr << "Error in file. Names do not match-fsName: " << fsName << " Type1Name: " << name << endl;
         return;
      }

      type2 << name << endl;
      type2 << seq << endl;
      for (int i = 0; i < 4; i ++)
      {
         type1 >> token;
         type2 << token << endl;
      }

   } 

   fs.close();
   type1.close();
   type2.close();
}

int main(int argc, char** argv)
{
   if (argc != 4)
   {
      cout << "Error in inputs! Usage: <cluster_type_1> <all> <prime number>" << endl;
      return 0;
   }


   char* queryFileName = "names.queries";
   char* type1Name = argv[1];
   char* allFileName = argv[2];
   char fsOutName[100];
   char type2Name[100];
	 int myprime = atoi(argv[3]);

   fstream nameseqfile(type1Name, fstream::in);
   createReadNames(nameseqfile, queryFileName);

/*
   char command[500];

   strcpy(fsOutName, type1Name);
   strcat(fsOutName, ".cluster_readseq");

   strcpy(type2Name, type1Name);
   strcat(type2Name, ".cluster2");

   sprintf(command, "./fs-fa.out -r %s -i %s -o %s -p %s", queryFileName, allFileName, fsOutName, myprime);
   system(command);

   mergeToType2(fsOutName, type1Name, type2Name);
*/
   return 0;
}








