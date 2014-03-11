#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <stdio.h>


using namespace std;

const int max_contig_size = 20000;

int contigId, len, dummy, longest_len, longest_id, longest_dummy;
char result[max_contig_size];
char line[100];
char line2[max_contig_size];

void get_longest(char* type1Name)
{
				fstream type1(type1Name, fstream::in);

				char line[2000];
				longest_len = 0;
			  longest_dummy = 0;
				longest_id = 0;
	

 while(!type1.eof()) {


				
					
	type1.getline(line, 100);
	type1.getline(line2, max_contig_size);

	if (strlen(line2) >	longest_len)
	{
		strcpy(result, line2); 			
		longest_len = strlen(line2);
	}
				 
	}
}

int main(int argc, char** argv)
{
				if (argc != 2)
				{
								cout << "Error in inputs! Usage: <OEA_contig_fasta_file>"  << endl;
								return 0;
				}

				longest_len=0;
				//char* queryFileName = "names.queries";
				char* type1Name = argv[1];
				char* type2Name = argv[2];

				fstream nameseqfile(type1Name, fstream::in);


				get_longest(type1Name);
				cout << ">"<<type1Name << endl;
				cout << result << endl;
		return 0;
}








