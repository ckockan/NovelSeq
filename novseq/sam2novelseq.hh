#ifndef _SAM__2_DIVET__H__
#define _SAM__2_DIVET__H__

/**
 *	Copyright 2011 by Reza Shahidi-Nejad
 *	Simon Fraser University
 *	Computational Biology Lab: http://compbio.cs.sfu.ca
 *
 *	personal homepage: reza.beginningat25.com
 */

#include <fstream>
#include <iostream>
#include <list>
using namespace std;

#include "samalignment.hh"
#include "repeathandler.hh"

class Sam2NovelSeqConverter
{
private:
	int QUALITY_BASE;
	
	bool errorInFiles;
	fstream samfile;
	fstream divetfile;
  // fstream allFile;
   RepeatHandler* repeatHandler;

	list<SamAlignment*> currentClassAlignments;

	void clearCurrentClass();
	void writeCurrentClassToDivet();
	bool isInTheSameClass(SamAlignment* newAlignment);
	void writeSamAlignment(SamAlignment* first, bool isFirst);
  //void writeSamAlignmentToAll(SamAlignment* align);

	double calcScore(SamAlignment* align);
	double calcPhred(char* qual1, char* qual2);
	int calcSizeOnRef(char* cigar);
	int calcError(SamAlignment* first);

public:
	Sam2NovelSeqConverter(char* divetFileName, char* satFileName, int QUALITY_BASE );
	~Sam2NovelSeqConverter();

	SamAlignment* getNextAlignment();
	void readHeader();
   void appendFile(char* samfilename);
	void addSamAlignment(SamAlignment* toBeAdded);

	bool hasError() {return errorInFiles;}
};

#endif

