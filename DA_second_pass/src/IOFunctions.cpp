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
#include <IOFunctions.h>
#include <GlobalFunctions.h>
#include <algorithm>

map<string, Timer> readTimer(string filename)
{
	map<string, Timer> timerCallbackList;
	string currentLine;
	ifstream timerInfoFile(filename);
	if(timerInfoFile.is_open())
	{
		while(getline(timerInfoFile,currentLine))
		{
			vector<string> current = split(currentLine,'$');
			Timer timer;
			timer.callbackFn = current[0];
			timer.rate = stoi(current[1]);
			timerCallbackList[current[0]] = timer;
		}
		timerInfoFile.close();
	}
	else
	{
		cout << "Unable to open Timer log. Skipping and setting timers as NULL." << endl;
	}
	return timerCallbackList;
}


map<string, Subscriber> readSubscriber(string filename)
{
	map<string, Subscriber> subscriberList;
	string currentLine;
	vector<string> splitedCurrrentLine;

	ifstream subscriberInfoFile(filename);
	if(subscriberInfoFile.is_open())
	{
		while(getline(subscriberInfoFile,currentLine))
		{
			splitedCurrrentLine = split(currentLine,'$');
			Subscriber subscriber;
			subscriber.variable = splitedCurrrentLine[0];
			subscriber.callbackFn = splitedCurrrentLine[1];
			subscriber.topic = splitedCurrrentLine[2];
			std::replace( subscriber.topic.begin(), subscriber.topic.end(), '/', '_');
			//subscriber.convert(splitedCurrrentLine[3]);
			subscriberList[splitedCurrrentLine[1]] = subscriber;
		}
		subscriberInfoFile.close();
	}
	else
	{
		cout << "Unable to open Timer log. Skipping and setting subscribers as NULL." << endl;
	}
	return  subscriberList;
}

map<string, Publisher> readPublisher(string filename)
{
	map<string, Publisher> publisherList;
	string currentLine;
	vector<string> splitedCurrrentLine;
	ifstream publisherInfoFile(filename);
	if(publisherInfoFile.is_open())
	{
		while(getline(publisherInfoFile,currentLine))
		{
			splitedCurrrentLine = split(currentLine,'$');
			Publisher publisher;
			publisher.variable = splitedCurrrentLine[0];
			publisher.topic = splitedCurrrentLine[1];
			std::replace( publisher.topic.begin(), publisher.topic.end(), '/', '_');
			//publisher.convert(splitedCurrrentLine[2]);
			publisherList[splitedCurrrentLine[0]] = (publisher);
		}
		publisherInfoFile.close();
	}
	else
	{
		cout << "Unable to open Timer log. Skipping and setting publishers as NULL." << endl;
	}
	return  publisherList;
}

map<string, Synchronizer> readSynchronizers(string filename)
{
	map<string, Synchronizer> snchronizerList;
	string currentLine;
	vector<string> splitedCurrrentLine;
	ifstream publisherInfoFile(filename);
	if(publisherInfoFile.is_open())
	{
		while(getline(publisherInfoFile,currentLine))
		{
			splitedCurrrentLine = split(currentLine,'$');
			Synchronizer synchronizer;
			synchronizer.variable = splitedCurrrentLine[0];
			synchronizer.callback = splitedCurrrentLine[1];
			synchronizer.convert(splitedCurrrentLine[2]);
			for (int i = 3; i < splitedCurrrentLine.size(); ++i)
			{
				synchronizer.subscribers.push_back(splitedCurrrentLine[i]);
			}
			snchronizerList[splitedCurrrentLine[1]] = (synchronizer);
		}
		publisherInfoFile.close();
	}
	else
	{
		cout << "Unable to open Timer log. Skipping and setting publishers as NULL." << endl;
	}
	return snchronizerList;
};


//read function calls made from the current function being read
void readCallList(ifstream &inputFile, Function &function)
{
	vector<Function> callList;
	string currentLine;
	vector<string> splitedCurrentLine;
	while (getline(inputFile, currentLine))
	{

		//for ending condition
		if(!currentLine.compare("CALL_LIST_END"))
		{
			break;
		}
		splitedCurrentLine = split(currentLine,'$');
		Function _function;
		_function.name = splitedCurrentLine[0];
		_function.dependency = getDependency(splitedCurrentLine[1]);
		_function.timePattern = getTimePattern(splitedCurrentLine[2]);
		if(splitedCurrentLine.size() > 3 )
			_function.conditions.insert(_function.conditions.begin(), splitedCurrentLine.begin()+3,splitedCurrentLine.end());

		/** Populating green maybe  */
		if(_function.dependency == INDEPENDENT && !_function.conditions.empty())
		{
			_function.dependency = GREEN_MAYBE_DEPENDENT;
		}

		callList.push_back(_function);
	}
	function.functionCalls = callList;
}

//read publish calls from the current function being read
void readPublishList(ifstream &inputFile, Function &function)
{
	vector<PublishCall> publishList;
	string currentLine;
	vector<string> splitedCurrentLine;
	while (getline(inputFile, currentLine))
	{
		;
		//for ending condition
		if(!currentLine.compare("PUBLISH_LIST_END"))
		{
			break;
		}
		splitedCurrentLine = split(currentLine,'$');
		PublishCall publish;
		publish.variable = splitedCurrentLine[0];
		publish.topic = splitedCurrentLine[1];
		std::replace( publish.topic.begin(), publish.topic.end(), '/', '_');
		publish.dependency = getDependency(splitedCurrentLine[2]);
		publish.timePattern = getTimePattern(splitedCurrentLine[3]);
		if(splitedCurrentLine.size() > 4 )
			publish.conditions.insert(publish.conditions.begin(), splitedCurrentLine.begin()+4,splitedCurrentLine.end());

		/** Populating green maybe  */
		if(publish.dependency == INDEPENDENT && !publish.conditions.empty())
		{
			publish.dependency = GREEN_MAYBE_DEPENDENT;
		}

		publishList.push_back(publish);
	}
	function.publishCalls = publishList;
}

//read model and it calls readCallList and readPublishList
vector<Function> readModel (string filename)
{
	vector<Function> functionList;

	string currentLine;
	vector<string> splitedCurrrentLine;

	ifstream inputFile(filename);
	if(inputFile.is_open())
	{
		//int i =0;
		while(getline(inputFile,currentLine)) //FUNCTION_START is read and ignored
		{
			getline(inputFile,currentLine); //reading function name and dependency
			splitedCurrrentLine = split(currentLine,'$');
			Function function;
			function.name = splitedCurrrentLine[0];
			function.dependency = getDependency(splitedCurrrentLine[1]);
			getline(inputFile,currentLine); // reading CALL_LIST_START and ignored
			readCallList(inputFile, function);
			getline(inputFile,currentLine); // reading PUBLISH_LIST_START and ignored
			readPublishList(inputFile, function);
			getline(inputFile,currentLine); // reading FUNCTION_END and ignored
			functionList.push_back(function);
			//cout << i++ << endl;
		}
		inputFile.close();
	}
	else
		assert(!"Unable to open MODEL.LOG file");
	return functionList;
}


void writeModel(vector<Function> model)
{
	ofstream out("debug.log");
	//if file is not open return
	if(!out.is_open())
	{
		cout << "Cannot open debug.log file. skipping!!!" << endl;
		return;
	}
	for (Function fn: model)
	{
		out << "FUNCTION_START" << endl;
		out << fn.name << "$" << convert(fn.dependency) << endl;
		if(fn.isCallback)
			out << "CALLBACK_FUNCTION" << endl;
		out << "CALL_LIST_START" << endl;
		for (Function fnCall : fn.functionCalls)
		{
			out << 	fnCall.name << "$" << convert(fnCall.dependency) << "$" << convert(fnCall.timePattern) << "$" << endl;
		}
		out << "CALL_LIST_END" << endl;
		out << "PUBLISH_LIST_START" << endl;
		for(PublishCall pbCall: fn.publishCalls)
		{
			out << pbCall.variable << "$" <<
				  pbCall.topic << "$"
				<< convert(pbCall.dependency) << "$"
					<< convert( pbCall.timePattern)<< "$"  << endl;

		}
		out << "PUBLISH_LIST_END" << endl;
		out << "FUNCTION_END" << endl;
	}
}