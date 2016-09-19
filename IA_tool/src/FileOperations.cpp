//
// Created by nishant on 3/4/16.
//

#include "FileOperations.h"
#include <assert.h>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <Common.h>
#include <DFS.h>
#include <iostream>

using namespace std;
using namespace YAML;

bool ReadGraph(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph)
{
	vector<string> filenames = ReadFilenames(filename);
	for (int i = 0; i < filenames.size(); ++i) {
		ReadYAMLFile(filenames[i], rosNodeList, rosTopicList, rosGraph);
	}
	return true;
}

vector<string> ReadFilenames(string filename)
{
	ifstream inFile(filename);
	string currentLine;
	vector<string> filenames;
	if(inFile.is_open())
	{
		while(getline (inFile, currentLine))
		{
			filenames.push_back(currentLine);
		}
		inFile.close();
	}
	else
	{
		assert(!"Unable to open input file of filenames!!!");
	}
	return filenames;
}

void ReadYAMLFile(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList,
				  ROSGraph &rosGraph)
{
	Node node = LoadFile(filename);
	if(node.IsSequence()) //most expected
	{
		//parse sequence
		for (int i = 0; i < node.size(); ++i)
		{
			if(node[i].IsMap())
			{
				CheckAndParse(node[i], rosNodeList, rosTopicList, rosGraph);
			}
			else
			{
				cout << "Filename is " << filename << endl;
				assert(!"Unidentified Node type found in file!!!");
			}
		}
	}
	else if(node.IsMap())
	{
		CheckAndParse(node, rosNodeList, rosTopicList, rosGraph);
	}
	else
	{
		cout << "Filename is " << filename << endl;
		assert(!"Unidentified Node type found in file!!!");
	}
}

void WriteYAMLFile(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList,
				  ROSGraph &rosGraph)
{
	assert(!"WRITE TO YAML FILE IS NOT IMPLEMENTED YET!!! Contact developer @ nsharma@cse.unl.edu!!!");
}

void CheckAndParse(YAML::Node node, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList,
				   ROSGraph &rosGraph)
{
	assert(node.IsMap() && "CheckAndParse Expects a Map YAML Node");
	assert(node["type"] && "Node should have a type key member");

	if(node["type"].as<string>() == "ROSNode")
	{
		ParseROSNode(node, rosNodeList);
	}
	else if(node["type"].as<string>() == "ROSTopic")
	{
		ParseROSTopic(node, rosTopicList);
	}
	else if(node["type"].as<string>() == "ROSEdge")
	{
		ParseROSEdge(node, rosGraph);
	}
	else
	{
		assert(!"Node has an unrecognizable TYPE value");
	}
}

void ParseROSNode(YAML::Node node, ROSNodeList &rosNodeList)
{
	ROSNode rosNode;
	rosNode.decodeYAML(node);
	//to see if the node already exists
	bool isPresent = rosNodeList.find(rosNode.getNodeName()) != rosNodeList.end();
	if(!isPresent)
		rosNodeList[rosNode.getNodeName()] = rosNode;
}

void ParseROSTopic(YAML::Node node, ROSTopicList &rosTopicList)
{
	ROSTopic rosTopic;
	rosTopic.decodeYAML(node);

	//to see if the topic already exists
	bool isPresent = rosTopicList.find(rosTopic.getTopicName()) != rosTopicList.end();
	if(!isPresent)
		rosTopicList[rosTopic.getTopicName()] = rosTopic;
}

void ParseROSEdge(YAML::Node node, ROSGraph &rosGraph)
{
	ROSEdge rosEdge;
	rosEdge.decodeYAML(node);
	//check if existing source otherwise add
	map<string , vector<ROSEdge>>::iterator it = rosGraph.find(rosEdge.getSource());
	if(it != rosGraph.end()) //already exists
	{
		it->second.push_back(rosEdge);
	}
	else //create new entry and then add the edge
	{
		vector<ROSEdge> rosEdgeList;
		rosEdgeList.push_back(rosEdge);
		rosGraph[rosEdge.getSource()] = rosEdgeList;
	}
}

void WriteDOT(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph)
{
	ofstream dotfile(filename);
	dotfile << "digraph unix { \n rankdir=LR; " << endl;

	/* writing nodes first */
	dotfile << "/** ROSNode List **/" << endl;
	for (ROSNodeList::iterator it = rosNodeList.begin(); it != rosNodeList.end(); it++)
	{
		it->second.DOTWriter(dotfile);
	}

	/* writing topics now */
	dotfile << endl << "/** ROSTopic List **/" << endl;
	for (ROSTopicList::iterator it = rosTopicList.begin(); it != rosTopicList.end(); it++)
	{
		it->second.DOTWriter(dotfile);
	}


	/* writing edges last */
	dotfile << endl << "/** ROSEdges List **/" << endl;
	for (ROSGraph::iterator it = rosGraph.begin(); it != rosGraph.end(); it++)
	{
		for (int i = 0; i < it->second.size(); ++i)
		{
			it->second[i].DOTWriter(dotfile);
		}
	}

	dotfile << "}" <<endl;
	dotfile.close();
}


void ReadLaunchFile(char* filename)
{
	YAML::Node node = LoadFile(filename);
	YAMLFileList = node["YAMLFileList"].as<string>();
	graphOutFilename = node["graphOutFilename"].as<string>();
	analysisOutFilename = node["analysisOutFilename"].as<string>();
	affectedTopicFilename = node["affectedTopicFilename"].as<string>();
	stoppingColorsFilename = node["stoppingColorsFilename"].as<string>();

	string currentLine;
	ifstream affected(affectedTopicFilename);
	if(affected.is_open())
	{
		while (getline(affected,currentLine))
			AffectedComponents.push_back(currentLine);
		affected.close();
	}
	else
	{
		assert(!"Cannot read affected component list file!!!");
	}

	ifstream colorFile(stoppingColorsFilename);
	if(colorFile.is_open())
	{
		while (getline(colorFile,currentLine))
			StoppingColors.push_back(getColorForString(currentLine));
		colorFile.close();
	}
	else
	{
		assert(!"Cannot read stopping color list file!!!");
	}


}