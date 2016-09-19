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


#ifndef ROSLAUNCH_READER_ROSLAUNCH_XML_H
#define ROSLAUNCH_READER_ROSLAUNCH_XML_H

#include <string>
#include <assert.h>
#include <pugixml.hpp>
#include <iostream>
#include <vector>
#include <yaml-cpp/yaml.h>

using namespace std;

#define ROSLAUNCH_DEBUG std::cerr<< "Executed " << __LINE__ << std::end;

/*
 * Uncomment this define if you want to get screen dump about each node
 */
//#define DUMP

/**
 * We walk the DOM tree and keep information as required by us
 */
extern const char* node_types[8];

/*
 * XML DOM tree walker, we also keep information about the workspaces required by each node
 */
struct roslaunch_xml_walker: pugi::xml_tree_walker
{
	virtual bool for_each(pugi::xml_node& node);
	void populate_ros_workspace_list();
	YAML::Node launch_nodes;

	/*
	 * List of all exported workspace in the system
	 */
	vector<string> ros_workspace_list;

	bool visit_launch_node(pugi::xml_node &node, int depth);

	bool visit_node_node(pugi::xml_node &node, int depth);

	bool visit_machine_node(pugi::xml_node &node, int depth);

	bool visit_include_node(pugi::xml_node &node, int depth);

	bool visit_remap_node(pugi::xml_node &node, int depth);

	bool visit_param_node(pugi::xml_node &node, int depth);

	bool visit_rosparam_node(pugi::xml_node &node, int depth);

	bool visit_group_node(pugi::xml_node &node, int depth);

	bool visit_test_node(pugi::xml_node &node, int depth);

	bool visit_arg_node(pugi::xml_node &node, int depth);

	/*
	 * maintain a list of workspaces used by the nodes in question
	 * NOTE: this is different than ros_workspace_list member as this
	 * only contains the used workspaces rather than all
	 */
	void add_to_workspaces(string workspace);
};


/*
 * Another XML DOM walker but this one is used to extend the launch files
 * to include the included ones
 */
struct roslaunch_xml_extender: pugi::xml_tree_walker
{
	virtual bool for_each(pugi::xml_node& node);
	bool extend_include_node(pugi::xml_node &node);
};

/*
 * These functions are used to resolve any tags specified in the launch file
 */
void resolve_find(string &find, pugi::xml_node &node);
void resolve_env(string &env, pugi::xml_node &node);
void resolve_optenv(string &optenv, pugi::xml_node &node);
void resolve_arg(string &arg, pugi::xml_node &node);
void resolve_anon(string &anon, pugi::xml_node &node);

void resolve_tag(string &argument, pugi::xml_node &node);
void resolve_tags(string &argument, pugi::xml_node &node);


#endif //ROSLAUNCH_READER_ROSLAUNCH_XML_H
