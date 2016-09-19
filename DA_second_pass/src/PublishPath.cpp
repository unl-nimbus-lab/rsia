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


#include <PublishPath.h>
#include <GlobalFunctions.h>

void PublishPath::refinePath()
{


	for(Function fn : functionList)
	{
		Dependency newDependency = UpgradeDependency(this->dependency, fn.dependency);
		if(newDependency != this->dependency)
		{
			this->dependency = newDependency;
			this->timePattern = fn.timePattern;
		}

		if(this->dependency == INDEPENDENT && !fn.conditions.empty())
		{
			this->dependency = GREEN_MAYBE_DEPENDENT;
		}
	}

	if(this->dependency == INDEPENDENT && !this->conditions.empty())
	{
		this->dependency = GREEN_MAYBE_DEPENDENT;
	}
}

void PublishPath::mergeWithPath(PublishPath path)
{
	Dependency newDependency = MergeDependency(this->dependency, path.dependency);
	if(newDependency != this->dependency)
	{
		this->dependency = newDependency;
		this->timePattern = MERGE;
	}
}

void PublishPath::dumpPublishPath(string filename)
{
//	YAML::Node path;
//	path = this->encodeYaml();
	ofstream pathFile(filename, ofstream::app);//append
	if(pathFile.is_open())
	{
//		pathFile << endl;
//		pathFile << path;
//		pathFile << "- ";
		pathFile << this->topic << " : " << convert(this->dependency) << " : " << convert(this->timePattern);
		pathFile << endl;
	}
	else
	{
		cout << "Unable to open file for dumping path info. Skipping path dump!!!" << endl;
	}
}

YAML::Node PublishPath::encodeYaml()
{
	YAML::Node node;
	node["TopicName"] = this->topic;
	node["Dependency"] =  convert(dependency);
	node["TimePattern"] =  convert(timePattern);
	for (Function fn : this->functionList)
	{
		YAML::Node function;
		function["Name"] = fn.name;
		function["Dependency"] = convert(fn.dependency);
		node["FunctionList"].push_back(function);
	}
	return node;
}


void PublishPath::Dump()
{

	YAML::Node path;
	path = this->encodeYaml();
	cout << endl;
	PrintInGreen(this->topic);
	cout << " -- ";
	PrintInCyan(convert(dependency));
	cout <<  endl;
	PrintInYellow(convert(timePattern));
	cout <<  endl;
	PrintInRed("Function Call order");
	cout << endl;
	for (Function fn : this->functionList)
	{
		cout << "\t" << fn.name;
		cout << " -> ";
		cout << convert(fn.dependency);
		cout << endl;
	}
	cout << endl;

}
