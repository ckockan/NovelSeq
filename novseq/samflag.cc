#include "samflag.hh"

SamFlag::SamFlag(int bitwiseFlag)
{
	hasMulFragments = bitwiseFlag & 0x1;
	isProperlyAligned = bitwiseFlag & 0x2;
	isUnmapped = bitwiseFlag & 0x4;
	nextIsUnmapped = bitwiseFlag & 0x8;
	isReversed  = bitwiseFlag & 0x10;
	nextIsReversed  = bitwiseFlag & 0x20;
	isFirst  = bitwiseFlag & 0x40;
	isLast = bitwiseFlag & 0x80;
	isSecondary  = bitwiseFlag & 0x100;
	isNotPassingQualityControls  = bitwiseFlag & 0x200;
	isPcrOrOpticalDuplicate  = bitwiseFlag & 0x400;
}

int SamFlag::convertToInt()
{
	int ret = 
		((hasMulFragments) ? 1 : 0) * 0x1 +
		((isProperlyAligned) ? 1 : 0) * 0x2 +
		((isUnmapped) ? 1 : 0) * 0x4 +
		((nextIsUnmapped) ? 1 : 0) * 0x8 +
		((isReversed) ? 1 : 0) * 0x10 +
		((nextIsReversed) ? 1 : 0) * 0x20 +
		((isFirst) ? 1 : 0) * 0x40 +
		((isLast) ? 1 : 0) * 0x80 +
		((isSecondary) ? 1 : 0) * 0x100 +
		((isNotPassingQualityControls) ? 1 : 0) * 0x200 +
		((isPcrOrOpticalDuplicate) ? 1 : 0) * 0x400;

	return ret;
}

