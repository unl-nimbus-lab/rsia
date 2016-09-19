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


#include <DFS.h>

vector<PublishPath> PhaseTwo(vector<Function> &model)
{
	vector<PublishPath> pathList;

	for (int i = 0; i < model.size(); ++i)
	{
		for (int j = 0; j < model.at(i).publishCalls.size(); ++j)
		{
			vector<PublishPath> publishPaths = RunDFS(model, i, j);
			pathList.insert(pathList.end(),publishPaths.begin(),publishPaths.end());
		}
	}
#ifdef debug
	cout << "DFS done" << endl;
#endif
	return pathList;
}


vector<PublishPath> RunDFS(vector<Function> &model, int functionIndex, int publishCallIndex)
{
	vector<PublishPath> paths;
	map<string , bool> visited;
	//initialize visited to false
	for (int i = 0; i < model.size(); ++i)
	{
		visited[model[i].name] = false;
	}

	//define a path
	PublishPath path;
	path.topic = model[functionIndex].publishCalls[publishCallIndex].topic;
	//in case a fake publish call
	if(path.topic.empty()) return paths;
	path.dependency = model[functionIndex].publishCalls[publishCallIndex].dependency;
	path.timePattern = model[functionIndex].publishCalls[publishCallIndex].timePattern;
	//path.model.push_back(model[functionIndex]);

	path.conditions.insert(path.conditions.begin(),
						   model[functionIndex].publishCalls[publishCallIndex].conditions.begin(),
						   model[functionIndex].publishCalls[publishCallIndex].conditions.end());

	//paths.push_back(path);
	DFSVisit(model, functionIndex, path, paths, visited);
}


//this is actually BFS TROLL
void DFSVisit(vector<Function> &model, int functionIndex, PublishPath path, vector<PublishPath> &paths, map<string, bool> visited)
{
	visited[model[functionIndex].name] = true;
	path.functionList.push_back(model[functionIndex]);

	//to store the index of the functions that call our function
	vector<int> callingFunctionIndex;
	//to store the dependency information with which they call our function
	vector<Dependency> localDependencyList;
	//to store the timing pattern information with which they call our function
	vector<TimePattern > localTimerList;
	//to store the conditions with which they call our function
	vector <vector<string>> localConditions;

	for (int i = 0; i < model.size(); ++i)
	{
		for (int j = 0; j < model[i].functionCalls.size(); ++j)
		{
			if(!model[functionIndex].name.compare(model[i].functionCalls[j].name))
			{
				localDependencyList.push_back(model[i].functionCalls[j].dependency);
				localTimerList.push_back(model[i].functionCalls[j].timePattern);
				localConditions.push_back(model[i].functionCalls[j].conditions);
				callingFunctionIndex.push_back(i);
			}
		}
	}

	//if no function calls function of interest, that's a path and add it to the path list
	if(callingFunctionIndex.empty())
	{
		paths.push_back(path);
	}
	else
	{
		for (int i = 0; i < callingFunctionIndex.size(); ++i)
		{
			if(visited[model[callingFunctionIndex[i]].name] == false)
			{
				path.functionList[path.functionList.size()-1].dependency = localDependencyList[i];
				path.functionList[path.functionList.size()-1].timePattern = localTimerList[i];
				path.functionList[path.functionList.size()-1].conditions = localConditions[i];
				DFSVisit(model, callingFunctionIndex[i], path, paths, visited);
			}
		}
	}

}
