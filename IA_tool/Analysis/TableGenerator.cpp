//
// Created by nishant on 6/4/16.
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

//string AnalysisALLFilename;
//string AnalysisGreenFilename;
//string AnalysisOtherFilename;
string outFilename;
string outDepthFilename;

//Node AnalysisALL;
//Node AnalysisGreen;
//Node AnalysisOther;

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
LogDB AnalysisOtherLogDB;

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

	for (LogDB::iterator allIT = NoStopDB.begin(); allIT != NoStopDB.end() ; allIT++)
	{
		//print affected components
		for (string s : allIT->second.affectedComponents)
		{
			outFile << s << "   ";
		}
		outFile << ",";
		//print normal details
		outFile << allIT->second.impactedTopics << " ," << allIT->second.impactedTopicsPercentage << ",";
		outFile << allIT->second.impactedNodes << " ," << allIT->second.impactedNodesPercentage << ",";
		outFile << allIT->second.impactedDepth << ",";

		//print Green details
		outFile << GreenStopDB.at(allIT->second.affectedComponents).impactedTopics << " ," << GreenStopDB.at(allIT->second.affectedComponents).impactedTopicsPercentage << ",";
		outFile << GreenStopDB.at(allIT->second.affectedComponents).impactedNodes << " ," << GreenStopDB.at(allIT->second.affectedComponents).impactedNodesPercentage << ",";
		outFile << GreenStopDB[allIT->second.affectedComponents].impactedDepth << ",";

		//print other details
		outFile << AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopics << " ," << AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopicsPercentage << ",";
		outFile << AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodes << " ," << AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodesPercentage << ",";
		outFile << AnalysisOtherLogDB[allIT->second.affectedComponents].impactedDepth << ",";

		//print normal to Green reduction details
		outFile << allIT->second.impactedTopics - GreenStopDB.at(allIT->second.affectedComponents).impactedTopics << " ," << allIT->second.impactedTopicsPercentage - GreenStopDB.at(allIT->second.affectedComponents).impactedTopicsPercentage << ",";
		outFile << allIT->second.impactedNodes - GreenStopDB.at(allIT->second.affectedComponents).impactedNodes << " ," << allIT->second.impactedNodesPercentage - GreenStopDB.at(allIT->second.affectedComponents).impactedNodesPercentage << ",";
		outFile << allIT->second.impactedDepth - GreenStopDB[allIT->second.affectedComponents].impactedDepth << ",";

		//print normal to other reduction details
		outFile << allIT->second.impactedTopics - AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopics << " ," << allIT->second.impactedTopicsPercentage - AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopicsPercentage << ",";
		outFile << allIT->second.impactedNodes - AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodes << " ," << allIT->second.impactedNodesPercentage - AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodesPercentage << ",";
		outFile << allIT->second.impactedDepth - AnalysisOtherLogDB[allIT->second.affectedComponents].impactedDepth;

		outFile << endl ;

		//populating depth results
		//if depth entry exists
		if( depthResults.find(allIT->second.impactedDepth) != depthResults.end())
		{
			depthResults[allIT->second.impactedDepth].count++;
			if(allIT->second.impactedDepth != 0)
			{
				//revision 7446 had everything as percentage
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgDepthReductionGreen += (GreenStopDB[allIT->second.affectedComponents].impactedDepth);
				depthResults[allIT->second.impactedDepth].avgDepthReductionOther += (AnalysisOtherLogDB[allIT->second.affectedComponents].impactedDepth);
			}
			if(allIT->second.impactedNodes != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgNodeReductionGreen += (GreenStopDB.at(allIT->second.affectedComponents).impactedNodes);
				depthResults[allIT->second.impactedDepth].avgNodeReductionOther += (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodes);
				depthResults[allIT->second.impactedDepth].avgImpactedNodesNormal += allIT->second.impactedNodes;
			}
			if(allIT->second.impactedNodesPercentage != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgNodeReductionPercentageGreen += (GreenStopDB.at(allIT->second.affectedComponents).impactedNodesPercentage / allIT->second.impactedNodesPercentage) * 100;
				depthResults[allIT->second.impactedDepth].avgNodeReductionPercentageOther += (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodesPercentage / allIT->second.impactedNodesPercentage) * 100;
			}
			if (allIT->second.impactedTopics != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgTopicReductionGreen += (GreenStopDB.at(allIT->second.affectedComponents).impactedTopics);
				depthResults[allIT->second.impactedDepth].avgTopicReductionOther += (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopics);
				depthResults[allIT->second.impactedDepth].avgImpactedTopicsNormal += allIT->second.impactedTopics;
			}
			if(allIT->second.impactedTopicsPercentage != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgTopicReductionPercentageGreen += (GreenStopDB.at(allIT->second.affectedComponents).impactedTopicsPercentage / allIT->second.impactedTopicsPercentage) * 100;
				depthResults[allIT->second.impactedDepth].avgTopicReductionPercentageOther += (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopicsPercentage / allIT->second.impactedTopicsPercentage) * 100;
			}
		}
		else
		{
			depthResults[allIT->second.impactedDepth].depth = allIT->second.impactedDepth;
			depthResults[allIT->second.impactedDepth].count = 1;

			if(allIT->second.impactedDepth != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgDepthReductionGreen = (GreenStopDB[allIT->second.affectedComponents].impactedDepth);
				depthResults[allIT->second.impactedDepth].avgDepthReductionOther = (AnalysisOtherLogDB[allIT->second.affectedComponents].impactedDepth);
			}
			else
			{
				depthResults[allIT->second.impactedDepth].avgDepthReductionGreen = 0;
				depthResults[allIT->second.impactedDepth].avgDepthReductionOther = 0;
			}

			if(allIT->second.impactedNodes != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgNodeReductionGreen = (GreenStopDB.at(allIT->second.affectedComponents).impactedNodes);
				depthResults[allIT->second.impactedDepth].avgNodeReductionOther = (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodes);
				depthResults[allIT->second.impactedDepth].avgImpactedNodesNormal = allIT->second.impactedNodes;
			}
			else
			{
				depthResults[allIT->second.impactedDepth].avgNodeReductionGreen = 0;
				depthResults[allIT->second.impactedDepth].avgNodeReductionOther = 0;
				depthResults[allIT->second.impactedDepth].avgImpactedNodesNormal = 0;
			}

			if(allIT->second.impactedNodesPercentage != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgNodeReductionPercentageGreen = (GreenStopDB.at(allIT->second.affectedComponents).impactedNodesPercentage / allIT->second.impactedNodesPercentage) * 100;
				depthResults[allIT->second.impactedDepth].avgNodeReductionPercentageOther = (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedNodesPercentage / allIT->second.impactedNodesPercentage) * 100;
			}
			else
			{
				depthResults[allIT->second.impactedDepth].avgNodeReductionPercentageGreen = 0;
				depthResults[allIT->second.impactedDepth].avgNodeReductionPercentageOther = 0;
			}

			if (allIT->second.impactedTopics != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgTopicReductionGreen = (GreenStopDB.at(allIT->second.affectedComponents).impactedTopics);
				depthResults[allIT->second.impactedDepth].avgTopicReductionOther = (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopics);
				depthResults[allIT->second.impactedDepth].avgImpactedTopicsNormal = allIT->second.impactedTopics;
			}
			else
			{
				depthResults[allIT->second.impactedDepth].avgTopicReductionGreen = 0;
				depthResults[allIT->second.impactedDepth].avgTopicReductionOther = 0;
				depthResults[allIT->second.impactedDepth].avgImpactedTopicsNormal = 0;
			}

			if(allIT->second.impactedTopicsPercentage != 0)
			{
				//reduced to formula and not reduced by
				depthResults[allIT->second.impactedDepth].avgTopicReductionPercentageGreen = (GreenStopDB.at(allIT->second.affectedComponents).impactedTopicsPercentage / allIT->second.impactedTopicsPercentage) * 100;
				depthResults[allIT->second.impactedDepth].avgTopicReductionPercentageOther = (AnalysisOtherLogDB.at(allIT->second.affectedComponents).impactedTopicsPercentage / allIT->second.impactedTopicsPercentage) * 100;
			}
			else
			{
				depthResults[allIT->second.impactedDepth].avgTopicReductionPercentageGreen = 0;
				depthResults[allIT->second.impactedDepth].avgTopicReductionPercentageOther = 0;
			}
		}

	}


	outFile << endl << endl << endl;

	//dump depth wise analysis
	ofstream depthFile(outDepthFilename);
	//depthFile << "depth , avg Depth reduction green, avg node reduction green, avg topic reduction green,  avg Depth reduction other, avg node reduction other, avg topic reduction other " <<
	//			 endl;

	for (auto d : depthResults)
	{
		depthFile << d.second.depth << ", ";
		depthFile << d.second.count << ", ";
		depthFile << d.second.avgImpactedNodesNormal / d.second.count << ", ";
		depthFile << d.second.avgImpactedTopicsNormal / d.second.count << ", ";
		depthFile << d.second.avgDepthReductionGreen / d.second.count <<", ";
		depthFile << d.second.avgNodeReductionGreen / d.second.count << " ,"<< d.second.avgNodeReductionPercentageGreen / d.second.count <<", ";
		depthFile << d.second.avgTopicReductionGreen / d.second.count << " ,"<< d.second.avgTopicReductionPercentageGreen / d.second.count <<", ";
		depthFile << d.second.avgDepthReductionOther / d.second.count <<", ";
		depthFile << d.second.avgNodeReductionOther / d.second.count << " ,"<< d.second.avgNodeReductionPercentageOther / d.second.count <<", ";
		depthFile << d.second.avgTopicReductionOther / d.second.count << " ,"<< d.second.avgTopicReductionPercentageOther / d.second.count;
		depthFile << endl;
	}


	depthFile.close();

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

void readLaunchFile(char* filename)
{
	YAML::Node node = LoadFile(filename);
	string AnalysisALLFilename = node["AnalysisALL"].as<string>();
	string AnalysisGreenFilename = node["AnalysisGreen"].as<string>();
	string AnalysisOtherFilename = node["AnalysisOther"].as<string>();
	outFilename = node["outFilename"].as<string>();
	outDepthFilename = node["outDepthFilename"].as<string>();

	Node AnalysisALL = LoadFile(AnalysisALLFilename);
	Node AnalysisGreen = LoadFile(AnalysisGreenFilename);
	Node AnalysisOther = LoadFile(AnalysisOtherFilename);

	decodeAndAdd(NoStopDB, AnalysisALL);
	decodeAndAdd(GreenStopDB, AnalysisGreen);
	decodeAndAdd(AnalysisOtherLogDB, AnalysisOther);
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		cout << "usage TableGenerator <launch file name>" << endl;
		exit(0);
	}

	readLaunchFile(argv[1]);
	analyzeAndWrite();
	return 0;
}