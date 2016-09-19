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

/*
 * TODO: Right now, we do a reverse lookup for any resolve or anything,
 * we might wanna optimize by keeping a list of args and everything as we parse the launch file
 * unfortunatly, it will be impossible with pugixml
 * */

#include <iostream>
#include <pugixml.hpp>
#include <roslaunch_xml.h>
#include <fstream>
#include <cstring>
#include <helper_functions.h>

using namespace std;

string output_file;

int main(int argc, char ** argv) {

	if(argc < 3)
	{
		cout << "Usage: roslaunch_reader <launch_file> <output_file> <optional: save extended launch file \"save\">" << endl;
		exit(EXIT_FAILURE);
	}

	output_file = argv[2];

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(argv[1]);

	if(!result)
	{
		std::cerr << "XML [" << argv[1] << "] parsed with errors\n";
		std::cerr << "Error description: " << result.description() << "\n";
		std::cerr << "Error offset: " << result.offset << " (error at [..." << (argv[1] + result.offset) << "]\n\n";
	}

	//read and included included files
	roslaunch_xml_extender extender;
	doc.traverse(extender);

	//save if asked for
	if(argc >= 4 && !strcmp(argv[3],"save"))
		doc.save_file("/tmp/node.xml");

	//parse and populate nodes from XML DOM tree
	roslaunch_xml_walker walker;
	walker.populate_ros_workspace_list();
	doc.traverse(walker);

	cout << "Total Nodes Detected: " << walker.launch_nodes["nodes"].size() << endl;

	//write output to file
	ofstream file_out(output_file);
	if(file_out.is_open())
	{
		file_out << walker.launch_nodes;
		file_out.close();
	}
	else
	{
		cout << "Unable to open output file. SKIPPING!!!" << endl;
	}
	return EXIT_SUCCESS;
}