#ifndef __REPEAT_HANDLER__H_
#define __REPEAT_HANDLER__H_

#include <fstream>
#include <string.h>
using namespace std;

class RepeatHandler
{
   private:
      int arrayRepeats[25][500000][2];
      int arraySize[25];

      fstream satelliteFile;

      int binarySearch(int num, int chroId);
      int findChr(char chroName[7]);

   public:
      ~RepeatHandler();
      RepeatHandler(char* satFileName);
      bool isInSattelliteRegion(int position, char* chroName);
};

#endif

