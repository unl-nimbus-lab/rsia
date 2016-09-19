/**
888b    888 d8b               888                             888               888
8888b   888 Y8P               888                             888               888
88888b  888                   888                             888               888
888Y88b 888 888 88888b.d88b.  88888b.  888  888 .d8888b       888       8888b.  88888b.
888 Y88b888 888 888 "888 "88b 888 "88b 888  888 88K           888          "88b 888 "88b
888  Y88888 888 888  888  888 888  888 888  888 "Y8888b.      888      .d888888 888  888
888   Y8888 888 888  888  888 888 d88P Y88b 888      X88      888      888  888 888 d88P
888    Y888 888 888  888  888 88888P"   "Y88888  88888P'      88888888 "Y888888 88888P"
 */

/**
 * @author Nishant Sharma
 * @date 2016/05/12
 * @version 1.0
 */



#ifndef PHASETWO_NODE_H
#define PHASETWO_NODE_H

#include <DataStructures.h>
#include <fstream>
#include "PublishPath.h"

using namespace YAML;

Node EncodeNode(string nodeName);
vector<Node> EncodeSubscribers(string nodeName, map<string, Subscriber> subscribers);
vector<Node> EncodePublishers(string nodeName, map<string, Publisher> publishers, map<string, PublishPath> paths);

void DumpNodeYaml(string filename, string nodeName, map<string, Subscriber> subscribers, map<string, Publisher> publishers, map<string, PublishPath> paths);
#endif //PHASETWO_NODE_H
