//
// Created by nishant on 17/5/16.
//

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
	//read graph
	ReadGraph(YAMLFileList,rosNodeList,rosTopicList,rosGraph);
	WriteDOT(graphOutFilename, rosNodeList, rosTopicList, rosGraph);

	cout << "Nodes: " << rosNodeList.size() << endl;
	cout << "Topics: " << rosTopicList.size() << endl;

	return 0;
}