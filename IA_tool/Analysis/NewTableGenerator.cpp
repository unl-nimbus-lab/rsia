//
// Created by nishant on 8/8/16.
//

#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

using namespace std;
using namespace YAML;

string outFilename = "table.txt";

struct Log
{
	set<string> affectedComponents;
	vector<string> stoppingConditions;
	double impactedTopics;
	double impactedTopicsPercentage;
	double impactedNodes;
	double impactedNodesPercentage;
	int impactedDepth;
};

typedef map<set<string>, Log> LogDB;

LogDB NoStopDB;
LogDB GreenStopDB;
LogDB AutoGreenStopDB;

struct Depth
{
	double avgTopicReductionGreen ;
	double avgNodeReductionGreen;
	double avgDepthReductionGreen;
	double avgTopicReductionPercentageGreen;
	double avgNodeReductionPercentageGreen;
	double avgTopicReductionOther;
	double avgNodeReductionOther;
	double avgDepthReductionOther;
	double avgTopicReductionPercentageOther;
	double avgNodeReductionPercentageOther;
	double avgImpactedTopicsNormal;
	double avgImpactedNodesNormal;
	int count;
	int depth;
};

map<int, Depth> depthResults;

void analyzeAndWrite()
{
	ofstream outFile(outFilename);

	for (LogDB::iterator allIT = NoStopDB.begin(); allIT != NoStopDB.end() ; allIT++) {
		//print affected components
		for (string s : allIT->second.affectedComponents) {
			outFile << s << "   ";
		}
		outFile << ",";
		//print normal details
		outFile << allIT->second.impactedNodes << ", ";

		//print Green details
		outFile << GreenStopDB.at(allIT->second.affectedComponents).impactedNodes << ", ";

		//print AutoGreen details
		outFile << AutoGreenStopDB.at(allIT->second.affectedComponents).impactedNodes << ", ";

		//print normal to Green reduction details
		if(allIT->second.impactedNodes != 0)
			outFile << ((GreenStopDB.at(allIT->second.affectedComponents).impactedNodes) / allIT->second.impactedNodes ) * 100 << ", ";
		else
			outFile << 0 << ", ";

		//print normal to AutoGreen reduction details
		if(allIT->second.impactedNodes != 0)
			outFile << ((AutoGreenStopDB.at(allIT->second.affectedComponents).impactedNodes) / allIT->second.impactedNodes ) * 100 << ", ";
		else
			outFile << 0 << ", ";

		outFile << allIT->second.impactedDepth;


		outFile << endl;
	}


	outFile << endl << endl << endl;
	outFile.close();
}


void decodeAndAdd(LogDB &logDB, Node node)
{
	for (int i = 0; i < node.size(); ++i)
	{
		set <string> key;
		Log log;
		//cout << node[i];
		for (int j = 0; j < node[i]["AffectedComponents"].size(); ++j) {
			log.affectedComponents.insert(node[i]["AffectedComponents"][j].as<string>());
			key.insert(node[i]["AffectedComponents"][j].as<string>());
		}
		log.impactedDepth = node[i]["ImpactedDepth"].as<int>();
		log.impactedNodes = node[i]["ImpactedNodesN"].as<double>();
		log.impactedNodesPercentage = node[i]["ImpactedNodesP"].as<double>();
		log.impactedTopics = node[i]["ImpactedTopicsN"].as<double>();
		log.impactedTopicsPercentage = node[i]["ImpactedTopicsP"].as<double>();
		for (int j = 0; j < node[i]["StoppingConditions"].size(); ++j) {
			log.stoppingConditions.push_back(node[i]["StoppingConditions"][j].as<string>());
		}
		logDB[key] = log;
	}
}


int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "usage TableGenerator <nostop.log> <stop.log> <stopLogForAutomatic.log>" << endl;
		exit(0);
	}

	Node noStop = LoadFile(argv[1]);
	Node greenStop = LoadFile(argv[2]);
	Node autoGreenStop = LoadFile(argv[3]);

	decodeAndAdd(NoStopDB, noStop);
	decodeAndAdd(GreenStopDB, greenStop);
	decodeAndAdd(AutoGreenStopDB, autoGreenStop);

	analyzeAndWrite();

	return 0;
}