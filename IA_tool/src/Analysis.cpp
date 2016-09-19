//
// Created by nishant on 4/4/16.
//

#include "Analysis.h"

Analysis::Analysis()
{
	this-> totalROSNodes = 0;
	this-> totalROSTopics = 0;
	this-> totalROSPublishes = 0;
	this-> totalIndependentEdges = 0;
	this-> totalDependentEdges = 0;
	this-> totalMaybeDependentEdges = 0;
	this-> totalImpactedTopics = 0;
	this-> totalImpactedNodes = 0;
	this->totalImpactedDepth = 0;
}

void Analysis::reset()
{
	this-> totalImpactedTopics = 0;
	this-> totalImpactedNodes = 0;
	this->totalImpactedDepth = 0;
}

void Analysis::IncImpactedTopics()
{
	this->totalImpactedTopics++;
}

void Analysis::IncImpactedNodes()
{
	this->totalImpactedNodes++;
}

void Analysis::IncImpactedDepth()
{
	this->totalImpactedDepth++;
}

void Analysis::SetImpactedDepth(int depth)
{
	this->totalImpactedDepth = depth;
}


void Analysis::Analise(ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph)
{
	this->totalROSNodes = (int) rosNodeList.size();
	this->totalROSTopics = (int) rosTopicList.size();

	//going through the map
	for (map<string, vector<ROSEdge>>::iterator it = rosGraph.begin(); it != rosGraph.end(); it++)
	{
		//actual edges
		for (int i = 0; i < it->second.size(); ++i)
		{
			//if edge is originating from a node or not
			bool isSource = rosNodeList.find(it->second[i].getSource()) != rosNodeList.end();
			if (isSource)
			{
				this->totalROSPublishes++;
			}

			//counting different kinds of edges
			if(it->second[i].getColor() == Color::GREEN)
			{
				this->totalIndependentEdges++;
			}
			else if (it->second[i].getColor() == Color::RED)
			{
				this->totalDependentEdges++;
			}
			else if (it->second[i].getColor() == Color::YELLOW || it->second[i].getColor() == Color::BLUE)
			{
				this->totalMaybeDependentEdges++;
			}
		}
	}
}

void Analysis::DumpAnalysis(string filename)
{
	//dump into file
	ofstream analysis(filename);
	analysis << this->yamlNode;
	analysis.close();
}

void Analysis::AppendAnalysis()
{
	YAML::Node node;
	for (int i = 0; i < AffectedComponents.size(); ++i)
	{
		node["AffectedComponents"].push_back(AffectedComponents[i]);
	}
	node["ImpactedTopicsN"] = totalImpactedTopics;
	node["ImpactedTopicsP"] = this->totalImpactedTopics * (100.00 / this->totalROSTopics);
	node["ImpactedNodesN"] = totalImpactedNodes;
	node["ImpactedNodesP"] = this->totalImpactedNodes * (100.00 / this->totalROSNodes);
	node["ImpactedDepth"] = this->totalImpactedDepth;
	for (int j = 0; j < StoppingColors.size(); ++j)
	{
		node["StoppingConditions"].push_back(getStringForColor(StoppingColors[j]));
	}
	this->yamlNode.push_back(node);
}
