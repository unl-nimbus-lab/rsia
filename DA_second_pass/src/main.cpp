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


#include <iostream>
#include "GlobalFunctions.h"
#include <yaml-cpp/yaml.h>
#include <IOFunctions.h>
#include <DataStructures.h>
#include <Function.h>
#include <UpdateModel.h>
#include <cstring>
#include <DFS.h>
#include <Node.h>
#include <climits>

//#define DEBUG_MAIN

using namespace std;
using namespace YAML;

//Input Defaults
static string launchFile = "launch.yaml";
static string NodeName = "Node";
static string OutputFilename = "node.yaml";
static string OutPathsFile = "node.log";
static string InTimers = "timers.log";
static string InSynchronizers = "synchronizers.log";
static string InSubscribers = "subscribers.log";
static string InPublishers = "publishers.log";
static string InModel = "model.log";


void help()
{
	cout << endl;
	cout << "HELP ./phaseTwo -h" << endl;
	cout << "USAGE: ./phaseTwo <launchFile> or ./phaseTwo defaults" << endl;
	cout << "Launch file should contain (With Default Values) \n\t-> NodeName\t\t:Node \n\t-> OutputFilename\t:node.yaml "
		 << "\n\t-> InTimers\t\t:timers.log \n\t-> InSynchronizers\t:synchronizers.log \n\t-> InSubscribers\t:subscribers.log "
		 << "\n\t-> InPublishers\t\t:publishers.log \n\t-> InModel\t\t:model.log" << endl;
	cout << "NOTE: defaults expect all the files to be in the same directory. Yaml-cpp throws an instance of BadFile if launch file is corrupted or doesn't exist." << endl << endl;
}

void ParseInputs(string input)
{
	if(!input.compare("-h") || !input.compare("-help") || !input.compare("-H") || !input.compare("-?"))
	{
		help();
		exit(1);
	}
	if(input.compare("defaults"))
	{
		launchFile = input;
		Node launch = LoadFile(launchFile);
		NodeName = launch["NodeName"].as<string>();
		OutputFilename = launch["OutputFilename"].as<string>();
		InTimers = launch["InTimers"].as<string>();
		InSynchronizers = launch["InSynchronizers"].as<string>();
		InSubscribers = launch["InSubscribers"].as<string>();
		InPublishers = launch["InPublishers"].as<string>();
		InModel = launch["InModel"].as<string>();
	}
}

void DumpDebuggingInfo()
{
	vector <Function> model, original;
	model = original = readModel(InModel);
	updateModelTimers(InTimers, model);
	bool isSubscriberAbsent = updateModelSubscribers(InSubscribers, model);
	updateModelPublishers(InPublishers, model);
	updateModelSynchronizers(InSynchronizers, model);

	vector<PublishPath> paths = PhaseTwo(model);

	//refine paths
	for(PublishPath &path : paths)
	{
		path.refinePath();
	}

	//merge paths
	map<string, PublishPath> uniquePublishers;
	for (PublishPath &path : paths)
	{
		map<string, PublishPath>::iterator it = uniquePublishers.find(path.topic);
		if(it != uniquePublishers.end())
		{
			it->second.mergeWithPath(path);
		}
		else
		{
			uniquePublishers[path.topic] = path;
		}
	}

	/*
 * if there are no subs then make it independent
 */
	if(isSubscriberAbsent)
	{
		for(auto &kv: uniquePublishers) {
			kv.second.dependency = INDEPENDENT;
			kv.second.timePattern = NO_SUBSCRIBER;
		}
	}

	string in = "upaths";
	while(in.compare("@exit"))
	{
		if (!in.compare("rpaths"))
		{
			for (PublishPath path: paths)
			{
				cout << "______________" << endl;
				path.Dump();
			}
			cout << "________________________________" << endl;
		}
		else if(!in.compare("upaths"))
		{

			for (auto &kv: uniquePublishers)
			{
				cout << "______________" << endl;
				kv.second.Dump();
			}
			cout << "________________________________" << endl;
		}
		else
		{
			cout << "Original Model" << endl;
			for (Function fcn: model) {
				if (!fcn.name.compare(in))
				{
					cout << "______________" << endl;
					fcn.Dump();
				}
			}
			cout << "________________________________" << endl;
		}


		//fflush(stdin);
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		getchar();

		//cout << "\n\033[2J\033[1;1H"; //clear screen
		system("reset");
		cout << "Function being tested -> ";
		cin >> in;
	}

}



int main(int argCount, char** argValues)
{

	if(argCount < 2)
	{
		help();
	}
	else
	{
		ParseInputs(argValues[1]);
	}
	vector <Function> model = readModel(InModel);
	updateModelTimers(InTimers, model);
	bool isSubscriberAbsent = updateModelSubscribers(InSubscribers, model);
	updateModelPublishers(InPublishers, model);
	updateModelSynchronizers(InSynchronizers, model);
	//writeModel(model);
#ifdef DEBUG_MAIN
	cout << "main:" << __LINE__ << endl;
#endif
	vector<PublishPath> paths = PhaseTwo(model);
#ifdef DEBUG_MAIN
	cout << "main:" << __LINE__ << endl;
#endif
	//refine paths
	for(PublishPath &path : paths)
	{
		path.refinePath();
	}
#ifdef DEBUG_MAIN
	cout << "main:" << __LINE__ << endl;
#endif
	//merge paths
	map<string, PublishPath> uniquePublishers;
	for (PublishPath &path : paths)
	{
		map<string, PublishPath>::iterator it = uniquePublishers.find(path.topic);
		if(it != uniquePublishers.end())
		{
			it->second.mergeWithPath(path);
		}
		else
		{
			uniquePublishers[path.topic] = path;
		}
	}

	/*
	 * if there are no subs then make it independent
	 */
	if(isSubscriberAbsent)
	{
		for(auto &kv: uniquePublishers) {
			kv.second.dependency = INDEPENDENT;
			kv.second.timePattern = NO_SUBSCRIBER;
		}
	}

	for(auto kv: uniquePublishers)
		kv.second.dumpPublishPath(OutPathsFile);
#ifdef DEBUG_MAIN
	cout << "main:" << __LINE__ << endl;
#endif
	DumpNodeYaml(OutputFilename, NodeName, readSubscriber(InSubscribers), readPublisher(InPublishers), uniquePublishers);
#ifdef DEBUG_MAIN
	cout << "main:" << __LINE__ << endl;
#endif

	if(argCount > 2) DumpDebuggingInfo();

	return 1;
}