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


#ifndef PHASETWO_DATASTRUCTURES_H
#define PHASETWO_DATASTRUCTURES_H

#include <string>
#include <vector>
#include <map>


using namespace std;


/**
 * Different Types of dependencies: green_maybe_dependent is actually independent with some conditions
 */
enum Dependency
{
	DEPENDENT,
	INDEPENDENT,
	MAYBE_DEPENDENT,
	GREEN_MAYBE_DEPENDENT
};

/**
 * Different Time patterns we recognize at this moment
 */
enum TimePattern
{
	NONE,
	RATE_WHILE_SLEEP,
	TIMER,
	WHILE_SLEEP,
	SYNCHRONIZER,
	APPROXIMATE_TIME,
	EXACT_TIME,
	MERGE,
	NO_SUBSCRIBER
};

Dependency getDependency(string value);
TimePattern getTimePattern(string value);
string convert(Dependency type);
string convert(TimePattern type);



/**
 *
 */
struct Subscriber
{
	enum Type
	{
		REGULAR,
		MESSAGE_FILTER,
		IMAGE_TRANSPORT,
		TRANSFORM_LISTENER
	};

	string convert(Type type);
	void convert(string type);

	string variable;
	string callbackFn;
	string topic;
	Type type;
};

struct Publisher
{
	enum Type
	{
		REGULAR,
		REALTIME,
		IMAGE_TRANSPORT,
		TRANSFORM_BROADCASTER
	};

	string convert(Type type);
	void convert(string type);

	string variable;
	string topic;
	Type type;
};

/**
 * For now timers are just a callback function with a frequency at which they are called
 */
struct Timer
{
	string callbackFn;
	double rate;
};


struct Synchronizer
{
	enum Type
	{
		REGULAR,
		EXACT_TIME,
		APPROXIMATE_TIME
	};

	string convert(Type type);
	void convert(string type);

	string variable;
	string type;
	vector<string> subscribers;
	string callback;
};


#endif