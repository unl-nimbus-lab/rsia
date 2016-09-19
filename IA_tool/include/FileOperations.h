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
// Created by nishant on 3/4/16.
//

#ifndef GRAPHTRAVERSAL_YAMLPARSER_H
#define GRAPHTRAVERSAL_YAMLPARSER_H

#include <string>
#include <vector>
#include "ROSNode.h"
#include "ROSTopic.h"
#include "ROSEdge.h"
#include <yaml-cpp/yaml.h>

using namespace std;

bool ReadGraph(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);

vector<string> ReadFilenames(string filename);

void ReadYAMLFile(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);

void WriteYAMLFile(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);

void CheckAndParse(YAML::Node node, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);

void ParseROSNode(YAML::Node node, ROSNodeList &rosNodeList);

void ParseROSTopic(YAML::Node node, ROSTopicList &rosTopicList);

void ParseROSEdge(YAML::Node node, ROSGraph &rosGraph);

void WriteDOT(string filename, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);

void ReadLaunchFile(char* filename);

#endif //GRAPHTRAVERSAL_YAMLPARSER_H
