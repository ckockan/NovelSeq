#include <ctype.h> //For toupper

#include "optionalfield.hh"

OptionalField::OptionalField(char* fieldStr)
{
	char* strtokPtr;
	char* tagStr = strtok_r(fieldStr, ":", &strtokPtr);
	char* typeStr = strtok_r(NULL, ":", &strtokPtr);
	char* valueStr = strtok_r(NULL, "\r\n", &strtokPtr);

	tag = new char[strlen(tagStr) + 1];
	int j;
	for (j = 0; tagStr[j] != 0; j++)
		tag[j] = toupper(tagStr[j]);
	tag[j] = '\0';

	switch (typeStr[0])
	{
		case 'A':
			type = FType_A;
			break;
		case 'i':
			type = FType_i;
			break;
		case 'f':
			type = FType_f;
			break;
		case 'Z':
			type = FType_Z;
			break;
		case 'H':
			type = FType_H;
			break;
		default:
			type = FType_Unknown;
			break;
	}

	value = new char[strlen(valueStr)  + 1];
	strcpy(value, valueStr);
}

OptionalField::~OptionalField()
{
	delete[] tag;
	delete[] value;
}

void OptionalField::print()
{
	printf("%s:%c:%s", tag, type, value);
}

