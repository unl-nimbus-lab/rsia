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
#ifndef GRAPHTRAVERSAL_ROSTOPIC_H
#define GRAPHTRAVERSAL_ROSTOPIC_H

#include <string>
#include <yaml-cpp/yaml.h>
#include "Common.h"
#include <fstream>

using namespace std;
using namespace YAML;

class ROSTopic {

public:

	/* Constructors */
	ROSTopic();
	ROSTopic(const string &topicName, const string &label, Color &color);

	/* Destructors */
	virtual ~ROSTopic() { }

	/* setters */
	void setTopicName(string topicName);
	void setLabel(string label);
	void setColor(Color color);
	void setBold(bool bold);

	/* getters */
	string getTopicName();
	string getLabel();
	Color  getColor();
	bool isBold();

	/* other functions */
	Node encodeYAML();
	void decodeYAML(Node node);
	bool DOTWriter(ofstream &file);

private:
	string topicName;
	string label;
	Color color;
	bool _isBold;
};

typedef map<string, ROSTopic> ROSTopicList;

#endif //GRAPHTRAVERSAL_ROSTOPIC_H
