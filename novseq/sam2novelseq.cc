#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <string>

#include "sam2novelseq.hh"
#include "math.h"

using namespace std;

string INVERSION = "inversion";
string DELETION = "deletion";
string INSERTION = "insertion";
string DONTKNOW = "dontknow";

Sam2NovelSeqConverter::Sam2NovelSeqConverter(char* divetFileName, char* satFileName, int qualBase = 33) : divetfile(divetFileName, fstream::out)
{
   if (!divetfile.is_open() )
   {
      errorInFiles = true;
      cerr << "Error in opening files: " << endl << "\tDivet: " << divetFileName <<  endl;
   }
   else
      errorInFiles = false;

	QUALITY_BASE = qualBase;

   if (satFileName != NULL)
      repeatHandler = new RepeatHandler(satFileName);
   else
      repeatHandler = NULL;
}

Sam2NovelSeqConverter::~Sam2NovelSeqConverter()
{
	if (divetfile.is_open())
		divetfile.close();
}

SamAlignment* Sam2NovelSeqConverter::getNextAlignment()
{
	if (hasError())
   {
		return NULL;
   }

	char line[1024];
	samfile.getline(line, 1024);
	SamAlignment* alignment = new SamAlignment(line);
	if (samfile.eof())
		return NULL;

	return alignment;
}

void Sam2NovelSeqConverter::readHeader()
{
	if (hasError())
		return;

	//TODO: READ the header -> lines which start with '@'
}

void Sam2NovelSeqConverter::addSamAlignment(SamAlignment* newSam)
{
	SamFlag* flag = newSam->getFlag();
	if (!flag->hasMulFragments)
	{
		cerr << "Multiple Fragments not supported! Ignoring." << endl;
		return;
	}

	if (!flag->isUnmapped && !flag->nextIsUnmapped)
	{
  	 newSam->print();
		cerr << "Unmapped alignments are not supported! Ignoring: " << " FlagMap? " << !flag->isUnmapped << " NextMap? " << !flag->nextIsUnmapped << endl;
		return;
	}

	if (flag->isUnmapped && flag->nextIsUnmapped) //different from sam2divet
	{
      newSam->print();
		cerr << "Unmapped alignments are not supported! Ignoring: " << " FlagMap? " << !flag->isUnmapped << " NextMap? " << !flag->nextIsUnmapped << endl;
		return;
	}
/*
   if (!(flag->isFirst ^ flag->isLast))
	{
		cerr << "Unsupported! isFirst and isLast should be different. Ignoring." << endl;
		return;
	}
*/
	currentClassAlignments.push_back(newSam);
}

bool Sam2NovelSeqConverter::isInTheSameClass(SamAlignment* newAlignment)
{
	if (currentClassAlignments.size() == 0)
		return true;

	return !strcmp(newAlignment->getQname(), currentClassAlignments.front()->getQname());
}

void Sam2NovelSeqConverter::writeSamAlignment(SamAlignment* samAlign, bool isFirst)
{
	divetfile << samAlign->getQname();


   if (isFirst)
      divetfile << "/1";
   else
      divetfile << "/2";



   divetfile
      << "\t"
		<< samAlign->getRname() << "\t";

      if (samAlign->getFlag()->isFirst)
         divetfile << samAlign->getPos() << "\t";
      else
         divetfile << samAlign->getPnext() << "\t";

		divetfile << ((samAlign->getFlag()->isReversed) ? "R" : "F") << "\t"
		<< calcError(samAlign) << "\t"
		<< calcPhred(samAlign->getQual(), samAlign->getOptionalFieldValue("NQ"))  << "\t"
		<< calcScore(samAlign) << "\t";	//get the quals of location of MD
   if (samAlign->getFlag()->isUnmapped)
   {
			divetfile << samAlign->getOptionalFieldValue("NS");
	}
	else
	{
			divetfile << samAlign->getSeq();
	}
		divetfile << endl;

}

/**
 * score *= 0.001 + 1/pow(10, ((qual[i]-33)/10.0));
 */
double Sam2NovelSeqConverter::calcScore(SamAlignment* align)
{

	double score = 1;
	char* qual = align->getQual();

	char* md = align->getOptionalFieldValue("MD");
	if (md == NULL)
	{
		cerr << "Alignment does not have MD field: " << align->getQual() << endl;
		return -1;
	}

	int len = strlen(md);
	int curNumber = 0;
	bool isDeleting = false;
	int currentIndex = 0;
	for (int i = 0; i < len; i++)
	{
		char c = md[i];
		if (c <= '9' && c >= '0')
		{
			curNumber = curNumber * 10 + (c - '0');
			isDeleting = false;
		}
		else if (c == '^')
		{
			isDeleting = true;
			currentIndex += curNumber;
			curNumber = 0;
		}
		else if (strchr("ACTGN", c) != NULL) //if the read character is either of the A C T G N characters
		{
			currentIndex += curNumber;
			curNumber = 0;
	//	cout << ">>>> index: " << currentIndex << " qual[index]: " << qual[currentIndex] << " qual-base: " << qual[currentIndex] - QUALITY_BASE << endl;
			score *= 0.001 + 1 / pow (10, ((qual[currentIndex] - QUALITY_BASE) / 10.0));
			currentIndex++; //For each mismatch, index increaments by one
		}
		else
		{
			cerr << "Ignoring the unsupported character found in MD: '" << c << "'. Returning -1." << endl;
			return -1;
		}
	}

	return score;
}

double Sam2NovelSeqConverter::calcPhred(char* qual1, char* qual2)
{
	int sum = 0;
	int len1 = strlen(qual1);
	int len2 = strlen(qual2);
	for (int i = 0; i < len1; i++)
		sum += qual1[i];
	for (int i = 0; i < len2; i++)
		sum += qual2[i];

	return (sum / (double)(len1 + len2)) - QUALITY_BASE;
}

int Sam2NovelSeqConverter::calcSizeOnRef(char* cigar)
{
	if (cigar[0] == '*')
	{
		cerr << "Cannot handle * in cigar: " << cigar << endl;
		return -1; //TODO: Handle this one
	}

	int size = 0;

	int curNumber = 0;
	int len = strlen(cigar);
	for (int i = 0; i < len; i++)
	{
		char c = cigar[i];
		if (c <= '9' && c >= '0')
		{
			curNumber = curNumber * 10 + (c - '0');
		}
		else
		{
			switch (c)
			{
				case 'M':
				case 'D':
				case 'N':
				case 'S':
				case '=': //TODO: Check with Faraz:
					size += curNumber;
					break;

				case 'H':
				case 'I':
				case 'P':
				case 'X':
					break;

				default:
					cerr << "Ignoring Unsupported Cigar character: " << c << endl;
					break;
			}

			curNumber = 0;
		}
	}

	return size;
}

/**
 * Returns -1 if NM field is not available
 *
 */
int Sam2NovelSeqConverter::calcError(SamAlignment* first)
{
	char* firstNM = first->getOptionalFieldValue("NM");

	if ( firstNM == NULL)
		return -1;

	//TODO: Error handling

	int firstE = atoi(firstNM);

	return firstE;
}

/*
void Sam2NovelSeqConverter::writeSamAlignmentToAll(SamAlignment* align)
{
   allFile << ">" << align->getQname();
   if (align->getFlag()->isUnmapped)
   {
			if (align->getFlag()->isFirst) allFile << "/2" << endl << align->getOptionalFieldValue("NS") << endl;
   		else allFile << "/1" << endl << align->getOptionalFieldValue("NS") << endl;
	}
	else
	{
			if (align->getFlag()->isFirst) allFile << "/1" << endl << align->getSeq() << endl;
   		else allFile << "/2" << endl <<  align->getSeq() << endl;
	}
}

*/

void Sam2NovelSeqConverter::writeCurrentClassToDivet()
{
	while (currentClassAlignments.size() > 0)
	{
		list<SamAlignment*>::iterator iter;
		SamAlignment* samAlign = currentClassAlignments.front();
		currentClassAlignments.pop_front();

      if (repeatHandler != NULL && repeatHandler->isInSattelliteRegion(samAlign->getPos(), samAlign->getRname()))
         continue;

      if (samAlign->getFlag()->isFirst ^ samAlign->getFlag()->isUnmapped) // DOUBLE CHECK PLEA$E
         writeSamAlignment(samAlign, false);
      else
         writeSamAlignment(samAlign, true);
	}
}

void Sam2NovelSeqConverter::clearCurrentClass()
{
	SamAlignment* toBeDeleted = NULL;
	list<SamAlignment*>::iterator iter;
	for (iter = currentClassAlignments.begin(); iter != currentClassAlignments.end(); iter++)
	{
		delete toBeDeleted;
	}
	currentClassAlignments.clear();
}

void Sam2NovelSeqConverter::appendFile(char* samFileName)
{
   samfile.open(samFileName, fstream::in);
   if (!samfile.is_open())
   {
      errorInFiles = true;
      cerr << "Cannot open sam file: " << samFileName << endl;
   }

	if (hasError())
	{
		cerr << "Cannot convert due to errors in files!" << endl;
		return;
	}

	SamAlignment* curAlignment = NULL;
	while ((curAlignment = getNextAlignment()) != NULL)
	{
		if (curAlignment->hasError()) {
			cerr << "Ignored due to error in input file!" << endl;
         cerr << curAlignment->getQname() << endl;
      }
		else
		{
			//If the alignment is in the same class of the previous ones
			if (!isInTheSameClass(curAlignment))
			{
				//First convert the previous ones to divet
				writeCurrentClassToDivet();
				clearCurrentClass();
			}
			addSamAlignment(curAlignment);
		}
	}
	writeCurrentClassToDivet();
	clearCurrentClass();
   samfile.close();
}

int main(int argc, char** argv)
{
	cout << endl << endl;
	cout << "NOTE: This program only works with sorted SAM files." << endl;
	cout << "To sort you files, you may use our other tool, FileSorter." << endl;
	cout << endl << endl;

	if (argc < 4)
	{
		cerr << "Error in passing parameters! " << endl << "Usage: sam2novelseq <-d|-f> <inputfile> <outputfile_type1> [<SatFileName>]" << endl;
		cout << endl << endl;
		return -1;
	}
   char* satFileAdrs;
   if (argc == 4)
      satFileAdrs = NULL;
   else
      satFileAdrs = argv[4];

   bool isDir;
   if (!strcmp(argv[1], "-d"))
      isDir = true;
   else if (!strcmp(argv[1], "-f"))
      isDir = false;
   else
   {
      cerr << "The first argument should either be '-d' for dir or '-f' for files. Exiting. " << endl;
      return -1;
   }

	cout << "Going to convert " << argv[2] << " to " << argv[3];
   if (satFileAdrs != NULL)
      cout << " Sat: " << argv[4];
   cout << "\tISDIR? " << isDir << endl;

	Sam2NovelSeqConverter sam2divet(argv[3], satFileAdrs);
   if (!isDir)
   {
	   sam2divet.readHeader();
      sam2divet.appendFile(argv[2]);
   }
   else
   {
      cout << "Opening directory: " << argv[2] << endl;

      DIR* dir;
      struct dirent* ent;
      dir = opendir(argv[2]);
      if (dir != NULL)
      {
         while ((ent = readdir (dir)) != NULL)
         {
            if (ent->d_name[0] == '.')
            {
               cout << "Ignoring " << ent->d_name << endl;
               continue;
            }

            char filePath [1024];
            strcpy(filePath, argv[2]);
            strcat(filePath, "/");
            strcat(filePath, ent->d_name);
            cout << "APPENDING FILE: " << filePath << endl;
            sam2divet.appendFile(filePath);

         }
         closedir(dir);
      }
      else
      {
         cerr << "Error in reading directories contents: " << argv[2] << endl;
         return -1;
      }
   }


	cout << "Conversion completed! " << endl;

	return 0;
}




