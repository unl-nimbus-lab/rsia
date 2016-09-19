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

#ifndef GRAPHTRAVERSAL_ROSEDGE_H
#define GRAPHTRAVERSAL_ROSEDGE_H

#include "Common.h"

using namespace std;
using namespace YAML;

class ROSEdge {
public:

	/* constructors */
	ROSEdge();
	ROSEdge(const string &source, const string &destination, const string &label, Color color);

	/* destructors */
	virtual ~ROSEdge() {}

	/* setters */
	void setSource(string source);
	void setDestination(string destination);
	void setLabel(string label);
	void setColor(Color color);
	void setBold(bool bold);

	/* getters */
	string getSource();
	string getDestination();
	string getLabel();
	Color getColor();
	bool isBold();

	/* YAML encoder - decoder */
	Node encodeYAML();
	void decodeYAML(Node node);

	/* DOT Writer */
	bool DOTWriter(ofstream &file);

private:
	string source;
	string destination;
	string label;
	Color color;
	bool _isBold;
};

typedef map<string, vector<ROSEdge>> ROSGraph;

#endif //GRAPHTRAVERSAL_ROSEDGE_H
