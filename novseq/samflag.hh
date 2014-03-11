#ifndef __SAM_FLAG__H___
#define __SAM_FLAG__H___

/**
 *	Copyright 2011 by Reza Shahidi-Nejad
 *	Simon Fraser University
 *	Computational Biology Lab: http://compbio.cs.sfu.ca
 *
 *	personal homepage: reza.beginningat25.com
 */

class SamFlag
{
public:
	bool hasMulFragments;
	bool isProperlyAligned;
	bool isUnmapped;
	bool nextIsUnmapped;
	bool isReversed;
	bool nextIsReversed;
	bool isFirst;
	bool isLast;
	bool isSecondary;
	bool isNotPassingQualityControls;
	bool isPcrOrOpticalDuplicate;

public:
	SamFlag(int bitwiseFlag);
	~SamFlag(){}

	int convertToInt();
};

#endif

