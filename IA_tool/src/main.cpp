#include <iostream>
#include <yaml-cpp/yaml.h>
#include <assert.h>
#include <Common.h>
#include <ROSNode.h>
#include <FileOperations.h>
#include <ROSTopic.h>
#include <ROSEdge.h>
#include <Analysis.h>
#include <DFS.h>

using namespace std;

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		cout << "Usage ./GraphTraversal <filename>" << endl;
		exit(0);
	}

	ReadLaunchFile(argv[1]);

	ROSNodeList rosNodeList;
	ROSTopicList rosTopicList;
	ROSGraph rosGraph;
	Analysis analysis;
	//read graph
	ReadGraph(YAMLFileList,rosNodeList,rosTopicList,rosGraph);
	analysis.Analise(rosNodeList,rosTopicList,rosGraph);

	if(AffectedComponents[0] == "all_individual_nodes")
	{
		for (ROSNodeList::iterator it = rosNodeList.begin(); it != rosNodeList.end(); it++)
		{
			ROSGraph tempROSGraph = rosGraph;
			analysis.reset();
			AffectedComponents.clear();
			AffectedComponents.push_back(it->second.getNodeName());
			DFS(analysis,rosNodeList,rosTopicList,tempROSGraph);
			analysis.AppendAnalysis();
		}
	}
	else if(AffectedComponents[0] == "all_individual_topics")
	{
		for (ROSTopicList::iterator it = rosTopicList.begin(); it != rosTopicList.end(); it++)
		{
			ROSGraph tempROSGraph = rosGraph;
			analysis.reset();
			AffectedComponents.clear();
			AffectedComponents.push_back(it->second.getTopicName());
			DFS(analysis,rosNodeList,rosTopicList,tempROSGraph);
			analysis.AppendAnalysis();
		}
	}
	else
	{
		DFS(analysis,rosNodeList,rosTopicList,rosGraph);
		analysis.AppendAnalysis();
	}
	analysis.DumpAnalysis(analysisOutFilename);
	WriteDOT(graphOutFilename, rosNodeList, rosTopicList, rosGraph);
	return 0;
}