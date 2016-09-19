/*
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/*
 * @author: Nishant Sharma
 */

#include <roslaunch_xml.h>
#include <helper_functions.h>

const char* node_types[8] =
{
"null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};

bool roslaunch_xml_walker::for_each(pugi::xml_node& node) {
	string node_name = node.name();
	if(!node_name.compare("launch"))
	{
		return visit_launch_node(node, depth());
	}
	else if(!node_name.compare("node"))
	{
		return visit_node_node(node, depth());
	}
	else if(!node_name.compare("machine"))
	{
		return visit_machine_node(node, depth());
	}
	else if(!node_name.compare("include"))
	{
		return visit_include_node(node, depth());
	}
	else if(!node_name.compare("remap"))
	{
		return visit_remap_node(node, depth());
	}
	//env tag is deprecated
	else if(!node_name.compare("param"))
	{
		return visit_param_node(node, depth());
	}
	else if(!node_name.compare("rosparam"))
	{
		return visit_rosparam_node(node, depth());
	}
	else if(!node_name.compare("group"))
	{
		return visit_group_node(node, depth());
	}
	else if(!node_name.compare("test"))
	{
		return visit_test_node(node, depth());
	}
	else if(!node_name.compare("arg"))
	{
		return visit_arg_node(node, depth());
	}

	return true; // continue traversal
}

void roslaunch_xml_walker::populate_ros_workspace_list() {
	string workspaces = execute("export | grep ROS_PACKAGE_PATH");

	vector<string> workspace = split(workspaces,'\'');
	this->ros_workspace_list = split(workspace[1],':');
}

bool roslaunch_xml_extender::for_each(pugi::xml_node& node) {
	string node_name = node.name();
	if(!node_name.compare("include"))
	{
		return extend_include_node(node);
	}
	//env tag is deprecated

	/*
	 * There is nothing to extend for launch, node, machine, remap,
	 * group, test, arg XML tags. They only have attributes
	 */

	/*
	 * Rosparam can load a yaml file, but we are getting it from roslaunch --params
	 */
	return true; // continue traversal
}

void resolve_find(string &find, pugi::xml_node &node){
	vector<string> tokens = split(find,' ');
	string command = "rospack ";
	command += find;
	string package_path = execute(command.c_str());
	if(package_path.empty()) {
		cerr << "Package " << tokens[1] << " not found" << endl;
		exit(EXIT_FAILURE);
	}
	find = package_path.substr(0, package_path.length()-1);
}

/*
 * TODO: instead of executing "echo $VAR", we can do getenv()
 */

void resolve_env(string &env, pugi::xml_node &node){
	vector<string> tokens = split(env,' ');
	string command = "echo $";
	command += tokens[1];
	string env_value = execute(command.c_str());
	if(env_value.empty()) {
		cerr << "ENV Variable " << tokens[1] << " not found" << endl;
		exit(EXIT_FAILURE);
	}
	env = env_value.substr(0, env_value.length()-1);
}

void resolve_optenv(string &optenv, pugi::xml_node &node){
	vector<string> tokens = split(optenv,' ');
	string command = "echo $";
	command += tokens[1];
	string env_value = execute(command.c_str());
	if(env_value.empty()) {
		env_value = "";
		for (int i = 2; i < tokens.size(); ++i) {
			env_value += tokens[i];
			env_value += " ";
		}
	}
	optenv = env_value.substr(0, env_value.length()-1);
}

void resolve_arg(string &arg_name, bool &is_resolved, pugi::xml_node &node) {
	//if the node is null, we have crossed root node
	if(!node){
		is_resolved = false;
		return;
	}

	//if the solution is not found check if the current node has a attribute for arg
	for (pugi::xml_node arg = node.child("arg"); arg; arg = arg.next_sibling("arg")) {
		string node_arg_name = arg.attribute("name").as_string();
		if(!arg_name.compare(node_arg_name)){
			if(arg.attribute("value")){
				arg_name = arg.attribute("value").as_string();
				is_resolved = true;
				return;
			}
			//if value is not found check for default
			if(arg.attribute("default")) {
				//go to the root node
				pugi::xml_node parent = node.parent();
				resolve_arg(arg_name, is_resolved, parent);

				if(is_resolved) {
					return;
				}
				else {
					arg_name = arg.attribute("default").as_string();
					is_resolved = true;
					return;
				}
			}
		}
	}

	//go to the root node
	pugi::xml_node parent = node.parent();
	resolve_arg(arg_name, is_resolved, parent);
}

void resolve_arg(string &arg, pugi::xml_node &node){
	vector<string> tokens = split(arg,' ');
	//see if we have
	bool is_resolved = false;
	resolve_arg(tokens[1], is_resolved, node);
	if(is_resolved)
	{
		arg = tokens[1];
		pugi::xml_node parent = node.parent();
		resolve_tags(arg,parent);
	}
	else
	{
		cerr << "ARG Variable " << tokens[1] << " not found" << endl;
		exit(EXIT_FAILURE);
	}
}

void resolve_anon(string &anon, pugi::xml_node &node){
	vector<string> tokens = split(anon,' ');
	anon = tokens[1];
}

void resolve_tag(string &argument, pugi::xml_node &node){
	vector<string> tokens = split(argument, ' ');

	if(!tokens[0].compare("find"))
	{
		resolve_find(argument, node);
	}
	else if(!tokens[0].compare("env"))
	{
		resolve_env(argument, node);
	}
	else if(!tokens[0].compare("optenv"))
	{
		resolve_optenv(argument, node);
	}
	else if(!tokens[0].compare("arg"))
	{
		resolve_arg(argument, node);
	}
	else if(!tokens[0].compare("anon"))
	{
		resolve_anon(argument, node);
	}
}

void resolve_tags(string &argument, pugi::xml_node &node) {
	if(argument.find("$(") == string::npos)
	{
		return;
	}

	string arg = "";
	vector<string> parsed = split_args(argument, arg);

	//resolve forward tags recursively
	resolve_tags(parsed[1], node);

	//resolve the tag
	resolve_tag(arg, node);

	argument = "";

	//now join everything to make one string
	argument += parsed[0];
	argument += arg;
	argument += parsed[1];
}