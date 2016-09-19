//
// Created by nishant on 14/4/16.
//
#include <iostream>
#include <fstream>
#include <map>
#include <string>

using namespace std;

typedef map< string, int > Counter;

void UpdateCounter(Counter &counter, string val)
{
	if(counter.find(val) != counter.end())
	{
		counter[val]++;
	}
	else
	{
		counter[val] = 1;
	}
}

void DumpCounterDetails(Counter &counter, string filename)
{
	ofstream outfile(filename);

	for (Counter::iterator i = counter.begin(); i != counter.end(); ++i)
	{
		outfile << i->first << "," << i->second << endl;
	}
	outfile.close();
}



int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout<<"usage: ListGenerator <inputFilename> <outputfilename>" << endl;
		return 0;
	}

	string inputFile = argv[1];
	string outputFile = argv[2];

	ifstream inFile(inputFile);
	if(!inFile.is_open())
	{
		cout << "Unable to read input file" << endl;
		return 0;
	}
	Counter counter;
	string currentLine;
	while(getline(inFile,currentLine))
	{
		UpdateCounter(counter,currentLine);
	}
	inFile.close();

	DumpCounterDetails(counter,outputFile);
	return 0;
}