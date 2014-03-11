#include "repeathandler.hh"

int RepeatHandler::binarySearch(int num, int chroId)
{
   int maxId;
   int minId;
   int midId;

   maxId = arraySize[chroId] - 1;
   minId = 0;
   midId = (minId + maxId) / 2;

   while (midId != maxId && midId != minId && arrayRepeats[chroId][midId][0] != num)
   {
      if (arrayRepeats[chroId][midId][0]>num)
      {
         maxId = midId;
      } 
      else if (arrayRepeats[chroId][midId][0]<num)
      {
         minId = midId;
      } else 
      { 
         midId = (maxId + minId) / 2;
      }
   }
   return midId;
}

bool RepeatHandler::isInSattelliteRegion(int pos, char* chroName)
{
   int chrId;
   int firstId;
   int secondId;

   chrId = findChr(chroName);
   firstId = binarySearch(pos, chrId);

   while (arrayRepeats[chrId][firstId][1] > pos) 
      firstId--;
   while (arrayRepeats[chrId][firstId][0] <= pos && firstId < arraySize[chrId])
   {
      if (firstId >= 0 && arrayRepeats[chrId][firstId][0]<= pos && arrayRepeats[chrId][firstId][1] >= pos )
         return true;
      firstId++;
   }

   return false;
}

int RepeatHandler::findChr(char chrName[7])
{
  if (strcmp(chrName, "chr1") == 0) return 1;
  if (strcmp(chrName, "chr2") == 0) return 2;
  if (strcmp(chrName, "chr3") ==0) return 3;
  if (strcmp(chrName, "chr4") ==0) return 4;
  if (strcmp(chrName, "chr5") ==0) return 5;
  if (strcmp(chrName, "chr6") ==0) return 6;
  if (strcmp(chrName, "chr7") ==0) return 7;
  if (strcmp(chrName, "chr8") ==0) return 8;
  if (strcmp(chrName, "chr9") ==0) return 9;
  if (strcmp(chrName, "chr10") ==0) return 10;
  if (strcmp(chrName, "chr11") ==0) return 11;
  if (strcmp(chrName, "chr12") ==0) return 12;
  if (strcmp(chrName, "chr13") ==0) return 13;
  if (strcmp(chrName, "chr14") ==0) return 14;
  if (strcmp(chrName, "chr15") ==0) return 15;
  if (strcmp(chrName, "chr16") ==0) return 16;
  if (strcmp(chrName, "chr17") ==0) return 17;
  if (strcmp(chrName, "chr18") ==0) return 18;
  if (strcmp(chrName, "chr19") ==0) return 19;
  if (strcmp(chrName, "chr20") ==0) return 20;
  if (strcmp(chrName, "chr21") ==0) return 21;
  if (strcmp(chrName, "chr22") ==0) return 22;
  if (strcmp(chrName, "chrX") ==0) return 23;
  if (strcmp(chrName, "chrY") ==0) return 24;
  return 0;
}

RepeatHandler::RepeatHandler(char* satFileName) : satelliteFile(satFileName, fstream::in)
{
   char chroName[7];
   int startSat;
   int endSat;
   int chrId;
   int prevEnd = -1;

   while (!satelliteFile.eof())
   {
      satelliteFile >> chroName >> startSat >> endSat;
      {
         chrId=findChr(chroName);
         arrayRepeats[chrId][arraySize[chrId]][0] = startSat;

         arrayRepeats[chrId][arraySize[chrId]][1] = endSat;
         arraySize[chrId]++;
      }
   }
}

RepeatHandler::~RepeatHandler()
{
   satelliteFile.close();
}

