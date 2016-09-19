//
// Created by nishant on 4/4/16.
//

#include <DFS.h>



void DFS(Analysis &analysis, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph)
{
	vector<string> visited;
	for (int i = 0; i < AffectedComponents.size(); ++i) {
		bool isTopic = rosTopicList.find(AffectedComponents[i]) != rosTopicList.end();
		bool isNode = rosNodeList.find(AffectedComponents[i]) != rosNodeList.end();
		if (isTopic || isNode)
		{
			if(isTopic)
			{
				rosTopicList[AffectedComponents[i]].setBold(true);
			}
			if(isNode)
			{
				rosNodeList[AffectedComponents[i]].setBold(true);
				//setting every outgoing edge from node affected
				for (int j = 0; j < rosGraph[AffectedComponents[i]].size(); ++j)
				{
					rosGraph[AffectedComponents[i]][j].setColor(Color::RED);
				}
			}
			int depth = DFSVisit(AffectedComponents[i], visited, analysis, rosNodeList, rosTopicList, rosGraph);
			analysis.SetImpactedDepth(depth);
		}
	}
}

bool isVisited(string current, vector<string> &visited)
{
	bool isPresent = false;
	for (int i = 0; i < visited.size(); ++i) {
		if (current == visited[i])
			isPresent = true;
	}
	return isPresent;
}

bool isStoppingColor(Color color)
{
	bool isStoppingColor = false;
	for (int i = 0; i < StoppingColors.size(); ++i) {
		if(color == StoppingColors[i])
		{
			isStoppingColor = true;
		}
	}
	return isStoppingColor;
}

int DFSVisit(string current, vector<string> &visited, Analysis &analysis, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph)
{
	int depth = 0;
	visited.push_back(current);
	for (int i = 0; i < rosGraph[current].size(); ++i)
	{
		int levelDepth = 0;
		if(!isVisited(rosGraph[current][i].getDestination(), visited))
		{
			if(!isStoppingColor(rosGraph[current][i].getColor()))
			{
				//for analysis
				bool isTopic = rosTopicList.find(rosGraph[current][i].getDestination()) != rosTopicList.end();
				bool isNode = rosNodeList.find(rosGraph[current][i].getDestination()) != rosNodeList.end();
				if(isTopic)
				{
					analysis.IncImpactedTopics();
					rosTopicList[rosGraph[current][i].getDestination()].setBold(true);
				}
				if(isNode)
				{
					analysis.IncImpactedNodes();
					rosNodeList[rosGraph[current][i].getDestination()].setBold(true);
				}
				//setting edge to bold
				rosGraph[current][i].setBold(true);
				//rosGraph[current][i].setColor(Color::RED);
				levelDepth = DFSVisit(rosGraph[current][i].getDestination() ,visited, analysis, rosNodeList, rosTopicList, rosGraph);
			}
		}
		else //if it's visited just bold the edge
		{
			rosGraph[current][i].setBold(true);
			//rosGraph[current][i].setLabel("v");
		}
		if(levelDepth > depth)
		{
			depth = levelDepth;
		}
	}
	bool isNode = rosNodeList.find(current) != rosNodeList.end();
	if(isNode)
	{
		return depth + 1;
	}
	else
	{
		return depth;
	}
}