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



#include <Node.h>

Node EncodeNode(string nodeName)
{
	Node node;
	node["type"] = "ROSNode";
	node["name"] = nodeName;
	node["label"] = nodeName;
	node["color"] = "none";
	return node;
}

vector<Node> EncodeSubscribers(string nodeName, map<string, Subscriber> subscribers)
{
	vector<Node> subscriptions;
	for(auto& kv : subscribers)
	{
		Node topic;
		topic["type"] = "ROSTopic";
		topic["name"] = kv.second.topic;
		topic["label"] = kv.second.topic;
		topic["color"] = "none";
		subscriptions.push_back(topic);

		//cout << topic;
		//cout << endl;

		Node edge;
		edge["type"] = "ROSEdge";
		edge["source"] = kv.second.topic;
		edge["destination"] = nodeName;
		edge["label"] = "none";
		edge["color"] = "none";
		subscriptions.push_back(edge);
	}
	return subscriptions;
}


string GetColor(string publishTopic, map<string, PublishPath> paths)
{
	map<string, PublishPath>::iterator kv;
	kv = paths.find(publishTopic);
	if(kv != paths.end())
	{
		if(kv->second.dependency == INDEPENDENT) 			return "green";
		if(kv->second.dependency == DEPENDENT) 				return "red";
		if(kv->second.dependency == MAYBE_DEPENDENT) 		return "yellow";
		if(kv->second.dependency == GREEN_MAYBE_DEPENDENT) 	return "cyan";
	}
	return "red";
}


vector<Node> EncodePublishers(string nodeName, map<string, Publisher> publishers, map<string, PublishPath> paths)
{
	vector<Node> subscriptions;
	for(auto &kv : publishers)
	{
		Node topic;
		topic["type"] = "ROSTopic";
		topic["name"] = kv.second.topic;
		topic["label"] = kv.second.topic;
		topic["color"] = "none";
		subscriptions.push_back(topic);

		Node edge;
		edge["type"] = "ROSEdge";
		edge["source"] = nodeName;
		edge["destination"] = kv.second.topic;
		edge["label"] = "none";
		edge["color"] = GetColor(kv.second.topic, paths);
		subscriptions.push_back(edge);
	}
	return subscriptions;
}

void DumpNodeYaml(string filename, string nodeName, map<string, Subscriber> subscribers, map<string, Publisher> publishers, map<string, PublishPath> paths)
{
//	cout << "debug" << endl;
	ofstream output(filename);
//	cout << "debug C" << endl;
	if(output.is_open())
	{
		Emitter out;
		out << BeginSeq;
		out << EncodeNode(nodeName);

		vector<Node> nodes = EncodeSubscribers(nodeName, subscribers);
		for(int i =0 ; i < nodes.size(); i++)
		{
			out << nodes[i];

		}
		for(Node node: EncodePublishers(nodeName, publishers, paths))
			out << node;

		out << EndSeq;
		output << out.c_str();
		output.close();
	}
	else
	{
		cout << "Unable to open " << filename << "file. Skipping Node YAML dump." << endl;
	}



}