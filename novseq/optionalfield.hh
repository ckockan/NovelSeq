#ifndef __OPTIONAL_FIELD__H___
#define __OPTIONAL_FIELD__H___

/**
 *	Copyright 2011 by Reza Shahidi-Nejad
 *	Simon Fraser University
 *	Computational Biology Lab: http://compbio.cs.sfu.ca
 *
 *	personal homepage: reza.beginningat25.com
 */

#include <stdio.h>
#include <string.h>

enum FieldType
{
	FType_A = 'A', FType_i = 'i', FType_f = 'f', FType_Z = 'Z', FType_H = 'H', FType_Unknown = 'U'
};

class OptionalField 
{
private:
	char* tag;
	FieldType type;
	char* value;

public:
	OptionalField(char* fieldStr); //TODO: Add error handler
	~OptionalField();

	char* getTag() {return tag;}
	char* getValue() {return value;}

	void print();
};

class OptionalFieldNode 
{
public:
	OptionalField* field;
	OptionalFieldNode* next;

	OptionalFieldNode(char* fieldStr) 
	{
		field = new OptionalField(fieldStr);
		next = NULL;
	}

	~OptionalFieldNode()
	{
		delete field;
		field = NULL;
	}
};

#endif

