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



#ifndef PHASETWO_PUBLISHPATH_H
#define PHASETWO_PUBLISHPATH_H

#include <DataStructures.h>
#include <Function.h>
#include <yaml-cpp/yaml.h>

//using namespace YAML;

class PublishPath
{
public:
	string topic;
	Dependency dependency;
	TimePattern timePattern;
	vector<string> conditions;
	vector<Function> functionList;

	void refinePath();
	void mergeWithPath(PublishPath path);
	void dumpPublishPath(string filename);
	YAML::Node encodeYaml();
	void Dump();
};

#endif //PHASETWO_PUBLISHPATH_H
