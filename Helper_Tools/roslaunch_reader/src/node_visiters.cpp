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

bool roslaunch_xml_walker::visit_launch_node(pugi::xml_node &node, int depth){
	// dump if defined
#ifdef DUMP
	for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation
	cout << node_types[node.type()] << ": type='" << node.name();
	cout << "'\n";
#endif

	//if launch tag is deprecated stop traversing child
	/*
	 * We commented this out because of the library support limitation of tree traversals
	 */
//	if(node.attribute("deprecated"))
//	{
//		return false;
//	}
	return true;
}

bool roslaunch_xml_walker::visit_node_node(pugi::xml_node &node, int depth){
	// dump if defined
	#ifdef DUMP
		for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation

		cout << node_types[node.type()] << ": type='" << node.name();

		if(node.attribute("pkg"))
			cout << "', pkg='" << node.attribute("pkg").value();

		if(node.attribute("type"))
			cout << "', type='" << node.attribute("type").value();

		if(node.attribute("name"))
			cout << "', name='" << node.attribute("name").value();

		if(node.attribute("args"))
			cout << "', args='" << node.attribute("args").value();

		if(node.attribute("machine"))
			cout << "', machine='" << node.attribute("machine").value();

		if(node.attribute("respawn"))
			cout << "', respawn='" << node.attribute("respawn").value();

		if(node.attribute("respawn_delay"))
			cout << "', respawn_delay='" << node.attribute("respawn_delay").value();

		if(node.attribute("required"))
			cout << "', required='" << node.attribute("required").value();

		if(node.attribute("ns"))
			cout << "', ns='" << node.attribute("ns").value();

		if(node.attribute("clear_params"))
			cout << "', clear_params='" << node.attribute("clear_params").value();

		if(node.attribute("output"))
			cout << "', output='" << node.attribute("output").value();

		if(node.attribute("cwd"))
			cout << "', cwd='" << node.attribute("cwd").value();

		if(node.attribute("launch-prefix"))
			cout << "', launch-prefix='" << node.attribute("launch-prefix").value();

		cout << "'\n";
	#endif


	/*
	 * Resolve if and unless tags before anything else
	 * Visit all the parents and see if they have a if or unless tag
	 */
	pugi::xml_node parent = node;
	while(parent) {
		if(parent.attribute("if")) {
			string value = parent.attribute("if").as_string();
			resolve_tags(value, node);
			if(value.compare("1") && value.compare("true")) {
				return true; // true because otherwise it will stop traversing
			}
		}
		if(parent.attribute("unless")) {
			string value = parent.attribute("unless").as_string();
			resolve_tags(value, node);
			if(value.compare("0") && value.compare("false")) {
				return true; // true because otherwise it will stop traversing
			}
		}
		parent = parent.parent();
	}


	YAML::Node yaml_node;
	launch_nodes["nodes"].push_back(yaml_node);

	if(node.attribute("pkg")) {
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["pkg"] = node.attribute("pkg").as_string();
		string query = "rospack find ";
		query += node.attribute("pkg").as_string();
		string package_dir = execute(query.c_str());
		if (package_dir.empty())
		{
			cerr << "package" << node.attribute("pkg") << "not found. Exiting!!!" << endl;
			exit(EXIT_FAILURE);
		}
		//also add the workspace for this node
		for (string &s : ros_workspace_list ){
			//assuming two workspaces won't be nested
			if(package_dir.find(s) != std::string::npos) {
				launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["workspace"] = s;
				add_to_workspaces(s);
			}
		}
	}

	if(node.attribute("type")) {
		string type = node.attribute("type").as_string();
		resolve_tags(type, node);
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["type"] = type;
	}

	if(node.attribute("name")) {
		string name = node.attribute("name").as_string();
		resolve_tags(name, node);
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["name"] = name;
	}

	if(node.attribute("args")) {
		string args = node.attribute("args").as_string();
		resolve_tags(args, node); //resolve any $(<tag>) in the string
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["args"] = args;
	}

//commented these out because I didn't need it at that time
/*
	if(node.attribute("machine"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["machine"] = node.attribute("machine").as_string();

	if(node.attribute("respawn"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["respawn"] = node.attribute("respawn").as_string();

	if(node.attribute("respawn_delay"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["respawn_delay"] = node.attribute("respawn_delay").as_string();

	if(node.attribute("required"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["required"] = node.attribute("required").as_string();
*/

	//we will add namespace no matter what
	//if(node.attribute("ns"))
	{
		string ns = "";

		//concatinate namespace till the root node
		pugi::xml_node parent = node;
		while(parent) {
			if(parent.attribute("ns")) {
				string resolve = parent.attribute("ns").as_string();
				resolve_tags(resolve, node);
				string ns_local = resolve;
				ns = ns_local + ns;
			}
			parent = parent.parent();
		}

		//if no namespace make root it's namespace
//		if(ns.empty())
//		{
//			ns = "/";
//		}
		//if root symbol is missing add it
		if(!ns.empty() && ns[0] != '/')
		{
			ns = "/" + ns;
		}
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["ns"] = ns;
	}

//commented these out because I didn't need it at that time
/*
	if(node.attribute("clear_params"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["clear_params"] = node.attribute("clear_params").as_string();

	if(node.attribute("output"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["output"] = node.attribute("output").as_string();

	if(node.attribute("cwd"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["cmd"] = node.attribute("cmd").as_string();

	if(node.attribute("launch-prefix"))
		launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["launch-prefix"] = node.attribute("launch-prefix").as_string();
*/
	if(!launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["ns"].as<string>().empty())
	cout << launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["ns"].as<string>() ;
	cout << "/" << launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["name"] << endl;

	return true;
}

bool roslaunch_xml_walker::visit_machine_node(pugi::xml_node &node, int depth){
	// dump if defined
#ifdef DUMP
	for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation
	cout << node_types[node.type()] << ": type='" << node.name();
	if(node.attribute("name"))
		cout << "', name='" << node.attribute("name").value();

	if(node.attribute("address"))
		cout << "', address='" << node.attribute("address").value();

	if(node.attribute("env-loader"))
		cout << "', env-loader='" << node.attribute("env-loader").value();

	if(node.attribute("default"))
		cout << "', default='" << node.attribute("default").value();

	if(node.attribute("user"))
		cout << "', user='" << node.attribute("user").value();

	if(node.attribute("timeout"))
		cout << "', timeout='" << node.attribute("timeout").value();

	cout << "'\n";
#endif

	//do nothing for ros_launch
	return true;
}

bool roslaunch_xml_walker::visit_include_node(pugi::xml_node &node, int depth){
	// dump if defined
	#ifdef DUMP
		for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation
		cout << node_types[node.type()] << ": type='" << node.name();
		if(node.attribute("file"))
			cout << "', file='" << node.attribute("file").value();
		if(node.attribute("ns"))
			cout << "', ns='" << node.attribute("ns").value();
		if(node.attribute("clear_params"))
			cout << "', clear_params='" << node.attribute("clear_params").value();
		if(node.attribute("pass_all_args"))
			cout << "', pass_all_args='" << node.attribute("pass_all_args").value();
		cout << "'\n";
	#endif

	//do nothing for ros_launch, as of now only node tags are necessary
	return true;
}

bool roslaunch_xml_walker::visit_remap_node(pugi::xml_node &node, int depth){
	// dump if defined
#ifdef DUMP
	for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation

	cout << node_types[node.type()] << ": type='" << node.name();
	cout << "', from='" << node.attribute("from").value();
	cout << "', to='" << node.attribute("to").value();
	cout << "'\n";
#endif

	//append remap info to last ros_launch node
	YAML::Node map;
	string from = node.attribute("from").as_string();
	string to = node.attribute("to").as_string();
	resolve_tags(from, node);
	resolve_tags(to, node);
	map["from"] = from;
	map["to"] = to;
	launch_nodes["nodes"][launch_nodes["nodes"].size() - 1]["remap"].push_back(map);
	//cout << launch_nodes["nodes"][launch_nodes["nodes"].size() - 1] << endl;
	return true;
}

bool roslaunch_xml_walker::visit_param_node(pugi::xml_node &node, int depth){
	// dump if defined
	#ifdef DUMP
		for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation
		cout << node_types[node.type()] << ": type='" << node.name();
		if(node.attribute("name"))
			cout << "', name='" << node.attribute("name").value();
		if(node.attribute("value"))
			cout << "', value='" << node.attribute("value").value();
		if(node.attribute("type"))
			cout << "', type='" << node.attribute("type").value();
		if(node.attribute("textfile"))
			cout << "', textfile='" << node.attribute("textfile").value();
		if(node.attribute("binfile"))
			cout << "', binfile='" << node.attribute("binfile").value();
		if(node.attribute("command"))
			cout << "', command='" << node.attribute("command").value();
		cout << "'\n";
	#endif

	//do nothing for ros_launch, as of now only node tags are necessary
	return true;
}

bool roslaunch_xml_walker::visit_rosparam_node(pugi::xml_node &node, int depth){
	// dump if defined
#ifdef DUMP
	for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation
	cout << node_types[node.type()] << ": type='" << node.name();
	if(node.attribute("command"))
		cout << "', command='" << node.attribute("command").value();
	if(node.attribute("file"))
		cout << "', file='" << node.attribute("file").value();
	if(node.attribute("param"))
		cout << "', param='" << node.attribute("param").value();
	if(node.attribute("ns"))
		cout << "', ns='" << node.attribute("ns").value();
	if(node.attribute("subst_value"))
		cout << "', subst_value='" << node.attribute("subst_value").value();
	cout << "'\n";
#endif

	//do nothing for ros_launch, as of now only node tags are necessary
	return true;
}

bool roslaunch_xml_walker::visit_group_node(pugi::xml_node &node, int depth){
	//do nothing for ros_launch
	// dump if defined
#ifdef DUMP
	for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation

	cout << node_types[node.type()] << ": type='" << node.name();
	if(node.attribute("ns"))
		cout << "', ns='" << node.attribute("ns").value();
	if(node.attribute("clear_params"))
		cout << "', clear_params='" << node.attribute("clear_params").value();
	cout << "'\n";
#endif
	return true;
}

bool roslaunch_xml_walker::visit_test_node(pugi::xml_node &node, int depth){
//do nothing for ros_launch
	// dump if defined
	#ifdef DUMP
		for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation

		cout << node_types[node.type()] << ": type='" << node.name();
		cout << "', pkg='" << node.attribute("pkg").value();
		cout << "', test-name='" << node.attribute("test-name").value();
		cout << "', type='" << node.attribute("type").value();
		cout << "'\n";

		/*
		 * TODO: Add more attributes here for more informative dump
		 * PRIORITY: LOW
		 */

	#endif
	return true;}

bool roslaunch_xml_walker::visit_arg_node(pugi::xml_node &node, int depth){
	//do nothing for ros_launch
	// dump if defined
#ifdef DUMP
	for (int i = 0; i < depth; ++i) std::cout << "\t"; // indentation

	cout << node_types[node.type()] << ": type='" << node.name();
	cout << "', name='" << node.attribute("name").value();
	if(node.attribute("value"))
		cout << "', value='" << node.attribute("value").value();
	if(node.attribute("default"))
		cout << "', default='" << node.attribute("default").value();
	cout << "'\n";
#endif
	return true;
}

bool roslaunch_xml_extender::extend_include_node(pugi::xml_node &node){

	//if visited abort
	if(node.attribute("visited"))
	{
		string visited = node.attribute("visited").as_string();
		if(!visited.compare("true"))
		{
			return true;
		}
	}


	/*
	* Resolve if and unless tags
	* Visit all the parents and see if they have a if or unless tag
	*/
	pugi::xml_node parent = node;
	if(parent.attribute("if"))
	{
		string value = parent.attribute("if").as_string();
		resolve_tags(value, node);
		if(value.compare("1") && value.compare("true"))
		{
			return true; // true because otherwise it will stop traversing
		}
	}
	if(parent.attribute("unless"))
	{
		string value = parent.attribute("unless").as_string();
		resolve_tags(value, node);
		if(value.compare("0") && value.compare("false"))
		{
			return true; // true because otherwise it will stop traversing
		}
	}



	// dump if defined
	string file_value = "NULL";
	if(node.attribute("file"))
	{
		file_value = node.attribute("file").as_string();
	}
	if(file_value.compare("NULL")) //if it's not null
	{
		/*
		 * Check if the file path given is absolute or package based
		 * if absolute first char will be '/'
		 */
		resolve_tags(file_value, node);
	}
	//load the included document
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(file_value.c_str());

	//recursively parse the doc contents
	roslaunch_xml_extender walker;
	doc.traverse(walker);

	if(!result)
	{
		std::cerr << "XML [" << file_value.c_str() << "] parsed with errors\n";
		std::cerr << "Error description: " << result.description() << "\n";
		std::cerr << "Error offset: " << result.offset << " (error at [..." << (file_value.c_str() + result.offset) << "]\n\n";
	}
	else
	{
		//append the other document inside this node and tag it visited to stop traversing it recursively
		if(doc.child("launch"))
		{
			node.append_copy(doc.child("launch"));
			node.append_attribute("visited") = "true";
		}
	}

	return true;
}

void roslaunch_xml_walker::add_to_workspaces(string workspace){
	bool is_present = false;
	for (YAML::const_iterator it= launch_nodes["workspaces"].begin(); it != launch_nodes["workspaces"].end(); ++it)  {
		if(!workspace.compare(it->as<string>()))
		{
			is_present = true;
		}
	}
	if(!is_present)
	{
		launch_nodes["workspaces"].push_back(workspace);
	}
}

