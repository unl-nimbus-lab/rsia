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

#ifndef GRAPHTRAVERSAL_DFS_H
#define GRAPHTRAVERSAL_DFS_H

#include <string>
#include <vector>
#include <Common.h>
#include <set>
#include <Analysis.h>

using namespace std;

void DFS(Analysis &analysis, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);

int DFSVisit(string current, vector<string> &visited, Analysis &analysis, ROSNodeList &rosNodeList, ROSTopicList &rosTopicList, ROSGraph &rosGraph);

bool isVisited(string current, vector<string> &visited);

bool isStoppingColor(Color color);

#endif //GRAPHTRAVERSAL_DFS_H
