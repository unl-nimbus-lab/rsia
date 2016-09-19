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

#include <DataStructures.h>
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <PublishPath.h>

using namespace std;

string convert(Dependency type)
{
	switch (type)
	{
		case INDEPENDENT:   		return "INDEPENDENT";
		case MAYBE_DEPENDENT:   	return "MAYBE_DEPENDENT";
		case GREEN_MAYBE_DEPENDENT: return "GREEN_MAYBE_DEPENDENT";
		case DEPENDENT: 			return "DEPENDENT";
		default:      				return "Unknown";
	}
}

Dependency getDependency(string value)
{
	if(!value.compare("INDEPENDENT"))					return INDEPENDENT;
	else if(!value.compare("MAYBE_DEPENDENT"))			return MAYBE_DEPENDENT;
	else if(!value.compare("GREEN_MAYBE_DEPENDENT"))	return GREEN_MAYBE_DEPENDENT;
	else												return DEPENDENT;
}

TimePattern getTimePattern(string value)
{
	if(!value.compare("SYNCHRONIZER")) 				return SYNCHRONIZER;
	else if(!value.compare("APPROXIMATE_TIME"))		return APPROXIMATE_TIME;
	else if(!value.compare("EXACT_TIME"))			return EXACT_TIME;
	else if(!value.compare("WHILE_SLEEP"))			return WHILE_SLEEP;
	else if(!value.compare("RATE_WHILE_SLEEP"))		return RATE_WHILE_SLEEP;
	else if(!value.compare("TIMER"))				return TIMER;
	else if(!value.compare("MERGE"))				return MERGE;
	else if(!value.compare("NO_SUBSCRIBER"))		return NO_SUBSCRIBER;
	else											return NONE;
}

string convert(TimePattern type)
{
	switch (type)
	{
		case TIMER:  			return "TIMER";
		case RATE_WHILE_SLEEP:  return "RATE_WHILE_SLEEP";
		case WHILE_SLEEP:   	return "WHILE_SLEEP";
		case SYNCHRONIZER:   	return "SYNCHRONIZER";
		case APPROXIMATE_TIME:  return "APPROXIMATE_TIME";
		case EXACT_TIME:   		return "EXACT_TIME";
		case NONE: 				return "NONE";
		case MERGE: 			return "MERGE";
		case NO_SUBSCRIBER: 	return "NO_SUBSCRIBER";
		default:      			return "Unknown";
	}
}



string Subscriber::convert(Type type)
{
	switch (type)
	{
		case REGULAR:				return "regular";
		case MESSAGE_FILTER:		return "message_filter";
		case IMAGE_TRANSPORT:		return "image_transport";
		case TRANSFORM_LISTENER:	return "transport_listener";
		default:					return "UNKNOWN";
	}
}

void Subscriber::convert(string type)
{
	if(!type.compare("regular"))				this->type = REGULAR;
	if(!type.compare("message_filter"))			this->type = MESSAGE_FILTER;
	if(!type.compare("image_transport")) 		this->type = IMAGE_TRANSPORT;
	if(!type.compare("transport_listener"))		this->type = TRANSFORM_LISTENER;
	else										this->type = REGULAR;
}




string Publisher::convert(Type type)
{
	switch (type)
	{
		case REGULAR:					return "regular";
		case REALTIME:					return "realtime";
		case IMAGE_TRANSPORT:			return "image_transport";
		case TRANSFORM_BROADCASTER:		return "transport_broadcaster";
		default:						return "UNKNOWN";
	}
}

void Publisher::convert(string type)
{
	if(!type.compare("regular"))				this->type = REGULAR;
	if(!type.compare("realtime"))				this->type = REALTIME;
	if(!type.compare("image_transport")) 		this->type = IMAGE_TRANSPORT;
	if(!type.compare("transport_broadcaster"))	this->type = TRANSFORM_BROADCASTER;
	else										this->type = REGULAR;
}


string Synchronizer::convert(Type type)
{
	switch (type)
	{
		case REGULAR:					return "regular";
		case EXACT_TIME:				return "exact_time";
		case APPROXIMATE_TIME:			return "approximate_time";
		default:						return "UNKNOWN";
	}
}

void Synchronizer::convert(string type)
{
	if(!type.compare("regular"))				this->type = REGULAR;
	if(!type.compare("exact_time"))				this->type = EXACT_TIME;
	if(!type.compare("approximate_time")) 		this->type = APPROXIMATE_TIME;
	else										this->type = REGULAR;
}

