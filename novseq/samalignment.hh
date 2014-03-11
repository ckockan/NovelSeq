#ifndef __SAM_ALIGNMENT___H_
#define __SAM_ALIGNMENT___H_

/**
 *	Copyright 2011 by Reza Shahidi-Nejad
 *	Simon Fraser University
 *	Computational Biology Lab: http://compbio.cs.sfu.ca
 *
 *	personal homepage: reza.beginningat25.com
 */

#include <stdio.h>

#include "samflag.hh"
#include "optionalfield.hh"

class SamAlignment
{
public:
	static char* SAM_DELIMITERS;

private:
	bool _hasError;	//Is true, if the alignment has errors -> For error checking

	char* qname;	//Query Template Name
	SamFlag* flag;	//Bitwise Flag
	char*  rname;	//Reference Sequence Name
	int pos;		//1-based leftmost mapping position
	int mapq;		//mapping quality
	char* cigar;	//cigar -> Should be changed to a class
	char* rnext;	//reference name of the mate/next fragment
	int pnext;		//Position of the mate/next fragment
	int tlen;		//observed template length
	char* seq;		//fragment sequence
	char* qual;		//ASCII of Phred-scale base quality
	OptionalFieldNode*	headField;

public: 
	SamAlignment(char* line);
	~SamAlignment();

	SamFlag* getFlag(){return flag;}
	char* getQname() {return qname;}
	int getPnext() {return pnext;}
	int getPos() {return pos;}
	char* getCigar() {return cigar;}
	char* getRname() {return rname;}
	char* getSeq() {return seq;}
	char* getQual() {return qual;}

	char* allocateString(char* notAllocatedStr);
	int convertToInt(char* intStr);
	char* getOptionalFieldValue(char* field);

	bool isMate(SamAlignment* second);

	bool hasError() { return _hasError; }

	void print();

};

#endif

