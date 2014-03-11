#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <complex>
#include <algorithm>
#include <vector>


using namespace std;

void addToType2(fstream& out, char* name, char* chro, int location, char orientation, float weight, int clusterId, char* sequence)
{
				out << name << endl;
				out << sequence << endl;
				out << chro << endl;
				out << location << endl;
				out << weight << endl;
				out << clusterId << endl;
				out << "END" << endl;
}



void createReadNames(fstream& type1, char* nameStr)
{
				fstream out(nameStr, fstream::out);

				char line[1024];
				char name[100];
				char token[120];
				char temp_token[5][120];
				bool ignoreFirstLine = true;
				while(!type1.eof())
				{
								if (ignoreFirstLine)
								{
												type1.getline(line, 1024);
												ignoreFirstLine = false;
								}

								type1 >> name;
								if (!strcmp(name, "END"))
								{
												type1.getline(line, 1024); //ignoring the end of line enter
												ignoreFirstLine = true;
												continue;
								}
								type1 >> token;
								type1 >> token;
								type1 >> token;
								type1 >> token;
								type1 >> token;
								out << name << endl;
				}
				out.close();
}

void itoa(int value, char* result) {

				char* out = result;
				int quotient = value;
				do {
								*out = "0123456789abcdef"[ abs( quotient % 10) ];
								++out;
								quotient /= 10;
				} while ( quotient );

				reverse( result, out );
				*out = 0;
				//	return result;
}


void mergeToType2(char* type1Name, char* type2Name, char* fasta_file)
{
				fstream type1(type1Name, fstream::in);
				fstream type2(type2Name, fstream::out);
			fstream* cluster_fasta = NULL;

				bool ignoreFirstLine = true;
				char fsName[100];
				char token[120];
				char seq[112];
				char line[1024];
				char name[100];
				char temp_token[5][120];
				char current_file[100];
				char temp_str[10];

				bool isF = false;
				int clusterId = 0;

				while(!type1.eof())
				{
								if (ignoreFirstLine)
								{
												type1.getline(line, 1024);
												ignoreFirstLine = false;

												if (type1.fail() || type1.bad()) //if end of file, return;
												break;

												if (!isF)
																clusterId++;
												type2 << clusterId << ((isF) ? " F" : " R") << endl;

												sprintf(current_file, "%s%d.%c", fasta_file, clusterId, ((isF) ? 'F' : 'R'));

											//	fprintf(stdout, "I AM %s", current_file);
												if (cluster_fasta != NULL) {
																cluster_fasta->close();
																delete cluster_fasta;
												}
												cluster_fasta = new fstream(current_file, fstream::out);

											isF = !isF;
								}


								type1 >> name;

								if (!strcmp(name, "END"))
								{
												type2 << "END" << endl;


												type1.getline(line, 1024); //ignoring the end of line enter
												ignoreFirstLine = true;
												continue;
								}


								//farz mikonim felan ke yaroo name-e tooye fs hast
								char tmp;

								type2 << name << endl;
								//type2 << seq << endl;
								//or (int i = 0; i < 5; i ++)
								//
								type1 >> temp_token[0] >> temp_token[1] >> temp_token[2] >> temp_token[3] >> temp_token[4];


								if (isF && !strcmp("F", temp_token[2]))
								{


												type2 << "END" << endl;
												type2 << clusterId << ((isF) ? " F" : " R") << endl;


												sprintf(current_file, "%s%d.%c", fasta_file, clusterId, ((isF) ? 'F' : 'R'));

											if (cluster_fasta != NULL){
															cluster_fasta->close();
																delete cluster_fasta;
											}
											cluster_fasta = new fstream(current_file, fstream::out);

												isF = !isF;

								}
							if (!isF && !strcmp("R", token)) cout << token;

								//	cout << temp_token[0] << temp_token[1] << temp_token[2] << temp_token[3] << temp_token[4] <<  endl;
								type2 << temp_token[0] << endl << temp_token[1] << endl << temp_token[2] << endl << temp_token[3] << endl << temp_token[4] <<  endl;
								(*cluster_fasta) << ">" << name << endl;
							  (*cluster_fasta) << temp_token[4] << endl;


				}

				type1.close();
				type2.close();
//				if (cluster_fasta != NULL){
//								cluster_fasta->close();
//								delete cluster_fasta;
//				}
}

int main(int argc, char** argv)
{
				if (argc != 4)
				{
								cout << "Error in inputs! Usage: <cluster_type_1> <cluster_type_2> <clusters_fasta_files>"  << endl;
								return 0;
				}


				//char* queryFileName = "names.queries";
				char* type1Name = argv[1];
				char* fasta_file = argv[3];
				char* type2Name = argv[2];

				fstream nameseqfile(type1Name, fstream::in);
	//			createReadNames(nameseqfile, fasta_file);

				char command[500];

				mergeToType2(type1Name, type2Name, fasta_file);

				return 0;
}








