#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "samalignment.hh"

#include "iostream"
using namespace std;

char* SamAlignment::SAM_DELIMITERS = " \t\r\n";

SamAlignment::SamAlignment(char* line)
{
   if (!strcmp(line, ""))
      return;

   char lineBackup[1024];
   strcpy(lineBackup, line);

	_hasError = false; //Sets the default. If an error occurs, the methods will change this one to true

	char* strtokPtr;
	char* qnameStr = strtok_r(line, SAM_DELIMITERS, &strtokPtr);
	char* flagStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* rnameStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* posStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* mapqStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* cigarStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* rnextStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* pnextStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* tlenStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* seqStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);
	char* qualStr = strtok_r(NULL, SAM_DELIMITERS, &strtokPtr);

	qname = allocateString(qnameStr);
	int flagInt = convertToInt(flagStr);
	flag = new SamFlag(flagInt);
	rname = allocateString(rnameStr);
	pos = convertToInt(posStr);
	mapq = convertToInt(mapqStr);
	cigar = allocateString(cigarStr);
	rnext = allocateString(rnextStr);
	pnext = convertToInt(pnextStr);
	tlen = convertToInt(tlenStr);
	seq = allocateString(seqStr);
	qual = allocateString(qualStr);

   if (hasError())
   {
      cout << "HERE: Error on this line: '" << lineBackup << "'" << endl;
   }

	headField = NULL;

	//TODO: Should test this part to see if it works for values which have space(' ') in them.
	char* opField = NULL;
	while ((opField = strtok_r(NULL, " \t\r\n", &strtokPtr)) != NULL)
	{
		OptionalFieldNode* field = new OptionalFieldNode(opField);
		if (headField == NULL)
		{
			headField = field;
		}
		else
		{
			OptionalFieldNode* cur = headField;
			while (cur->next != NULL)
				cur = cur->next;
			cur->next = field;
		}
	}
}

char* SamAlignment::allocateString(char* notAllocatedStr)
{
	if (notAllocatedStr == NULL) //Error in sam file
	{
      cerr << "Token is NULL: " << endl;
		_hasError = true;
		return NULL;
	}

	char* toRet = new char[strlen(notAllocatedStr) + 1];
	if (toRet == NULL) //Memory error
	{
      cerr << "Could not allocate memory" << endl;
		_hasError = true;
		return NULL;
	}
	strcpy(toRet, notAllocatedStr);
	return toRet;
}

int SamAlignment::convertToInt(char* intStr)
{
	if (intStr == NULL)
	{
		_hasError = true;
		return -1;
	}
	int ret = atoi(intStr);
	if (ret == INT_MIN || ret == INT_MAX)
	{
		_hasError = true;
		return -1;
	}

	return ret;
}

SamAlignment::~SamAlignment()
{
	delete flag;
	delete[] qname;
	delete[] rname;
	delete[] cigar;
	delete[] rnext;
	delete[] seq;
	delete[] qual;
}

void SamAlignment::print()
{
	if (hasError())
		printf("Has Error.\r\n");
	else
		printf(
			"%s\t%d\t%s\t%d\t%d\t%s\t%s\t%d\t%d\t%s\t%s",
			qname, flag->convertToInt(),
			rname, pos, mapq, cigar, rnext,
			pnext, tlen, seq, qual);

	OptionalFieldNode* cur = headField;
	while (cur != NULL)
	{
		printf("\t");
		cur->field->print();
		cur = cur->next;
	}
	printf("\r\n");
}

char* SamAlignment::getOptionalFieldValue(char* field)
{
	OptionalFieldNode* f = headField;
	while (f != NULL)
	{
		if (!strcmp(f->field->getTag(), field))
			return f->field->getValue();

		f = f->next;
	}

	return NULL;
}

bool SamAlignment::isMate(SamAlignment* second)
{
	if (second == NULL)
		return false;
	else
	{
		if (strcmp(this->qname, second->qname))
			return false;
		else
			return (this->pos == second->pnext) && (this->pnext == second->pos);
	}
}

