/*
888b    888 d8b               888                             888               888
8888b   888 Y8P               888                             888               888
88888b  888                   888                             888               888
888Y88b 888 888 88888b.d88b.  88888b.  888  888 .d8888b       888       8888b.  88888b.
888 Y88b888 888 888 "888 "88b 888 "88b 888  888 88K           888          "88b 888 "88b
888  Y88888 888 888  888  888 888  888 888  888 "Y8888b.      888      .d888888 888  888
888   Y8888 888 888  888  888 888 d88P Y88b 888      X88      888      888  888 888 d88P
888    Y888 888 888  888  888 88888P"   "Y88888  88888P'      88888888 "Y888888 88888P"
 */
//
// Created by nishant on 4/4/16.
//

#ifndef GRAPHTRAVERSAL_ANALYSIS_H
#define GRAPHTRAVERSAL_ANALYSIS_H


#include "ROSEdge.h"
#include "ROSTopic.h"
#include "ROSNode.h"

class Analysis {
public:

	/* constructors */
	Analysis();

	/* Destructors */
	virtual ~Analysis() { }

	/* other functions */
	void Analise(ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);
	void DumpAnalysis(string filename);
	void AppendAnalysis();

	/* modifiers */
	void reset();
	void IncImpactedTopics();
	void IncImpactedNodes();
	void IncImpactedDepth();
	void SetImpactedDepth(int depth);

private:
	int totalROSNodes;
	int totalROSTopics;
	int totalROSPublishes;
	int totalIndependentEdges;
	int totalDependentEdges;
	int totalMaybeDependentEdges;

	int totalImpactedTopics;
	int totalImpactedNodes;
	int totalImpactedDepth;

	YAML::Node yamlNode;

};


#endif //GRAPHTRAVERSAL_ANALYSIS_H
