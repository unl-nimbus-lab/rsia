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


#include <Function.h>
#include <IOFunctions.h>

void updateModelTimers(string fileName,vector<Function> &model)
{
	map<string, Timer> timers = readTimer(fileName);
	for(int j=0;j<model.size();j++)
	{
		//if key exists
		if(timers.find(model.at(j).name)  != timers.end())
		{
			model.at(j).dependency = INDEPENDENT;
			model.at(j).timePattern = TIMER;
			model.at(j).isCallback = true;
		}
	}
}


bool updateModelSubscribers(string fileName,vector<Function> &model)
{
	map<string, Subscriber> subscribers = readSubscriber(fileName);
	for(int j=0;j<model.size();j++)
	{
		map<string, Subscriber>::iterator it = subscribers.find(model.at(j).name);
		if(it != subscribers.end())
		{
			model.at(j).dependency = DEPENDENT;
			model.at(j).timePattern = NONE;
			model.at(j).isCallback = true;
			model.at(j).subscribedTopic = it->second.topic;
			//NOTE: Variable Present but not used!
		}
	}
	return subscribers.empty();
}


void updateModelPublishers(string fileName,vector<Function> &model)
{
	map<string, Publisher> publishers = readPublisher(fileName);
	for(int j=0; j<model.size(); j++)
	{
		for(int k=0; k < model.at(j).publishCalls.size(); k++)
		{
			map<string, Publisher>::iterator it = publishers.find(model.at(j).publishCalls.at(k).variable);
			if(it != publishers.end())
			{
				//Do I need to change topic name of Function or publish list
				model.at(j).publishCalls.at(k).topic = it->second.topic;
			}
		}
	}
}

void updateModelSynchronizers(string fileName,vector<Function> &model)
{
	map<string, Synchronizer> synchronizers = readSynchronizers(fileName);
	for(int j=0;j<model.size();j++)
	{
		map<string, Synchronizer>::iterator it = synchronizers.find(model.at(j).name);
		if(it != synchronizers.end())
		{
			model.at(j).dependency = MAYBE_DEPENDENT;
			model.at(j).timePattern = SYNCHRONIZER;
			model.at(j).isCallback = true;
			//Future work to add subscribed topics
			/// /model.at(j).subscribedTopic = it->second.topic;
			//NOTE: Variable Present but not used!
		}
	}
}