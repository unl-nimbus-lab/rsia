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
// Created by nishant on 2/4/16.
//

#ifndef GRAPHTRAVERSAL_ROSNODE_H
#define GRAPHTRAVERSAL_ROSNODE_H

#include <string>
#include <yaml-cpp/yaml.h>
#include "Common.h"
#include <fstream>
#include <vector>

using namespace std;
using namespace YAML;

class ROSNode {

public:

	/* constructors */
	ROSNode();
	ROSNode(string &label, string &nodeName, Color &color);

	/* destructors */
	virtual ~ROSNode() { }

	/* setters */
	void setNodeName(string nodeName);
	void setLabel(string label);
	void setColor(Color color);
	void setBold(bool bold);

	/* getters */
	string getNodeName();
	string getLabel();
	Color  getColor();
	bool isBold();

	/* YAML encoder - decoder */
	Node encodeYAML();
	void decodeYAML(Node node);

	/* DOT writer */
	bool DOTWriter(ofstream &file);

private:
	string nodeName;
	string label;
	Color color;
	bool _isBold;
};

typedef map<string, ROSNode> ROSNodeList;

#endif //GRAPHTRAVERSAL_ROSNODE_H
