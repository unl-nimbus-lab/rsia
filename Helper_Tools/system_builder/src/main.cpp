#include <iostream>
#include <yaml-cpp/yaml.h>
#include <string>
#include <sys/param.h>
#include <unistd.h>

using namespace std;

bool change_directory(string &directory) {
	//we are assuming it's workspace/build directory
	string build_folder = directory += "/build";
	int status = chdir(build_folder.c_str());
	if(status != 0) {
		//make that directory
		string mkdir_command = "mkdir -p " + build_folder;
		int mkdir_status = system(mkdir_command.c_str());
		if(mkdir_status == 0){
			int mkdir_chdir_status = chdir(build_folder.c_str());
			if(mkdir_chdir_status==0) {
				return true;
			}
			else {
				return false;
			}
		}
		else{
			return false;
		}
	}
	return true;
}

bool build_directory() {
	int status = system("cmake ../src -DCMAKE_INSTALL_PREFIX=../install -DCATKIN_DEVEL_PREFIX=../devel -DCMAKE_CXX_COMPILER=/usr/local/libexec/c++-analyzer -DCMAKE_CC_COMPILER=/usr/local/libexec/ccc-analyzer");
	if(status !=0 ) {
		return false;
	}
	// now make
	status = system("make");
	if(status != 0) {
		return false;
	}
	return true;
}

void build_all_workspaces(YAML::Node &workspaces) {
	for (int i = 0; i < workspaces.size(); ++i) {
		string workspace = workspaces[i].as<string>();
		if(workspace.find("/opt/ros/")!=string::npos) {
			continue; //skip the nodes that are installed
		}

		bool result = change_directory(workspace);
		if(!result) {
			cerr << "Cannot cd/mkdir into " << workspace << "/build directory!!! Exiting!!!" << endl;
			exit(EXIT_FAILURE);
		}

		result = build_directory();
		if(!result) {
			cerr << "Cannot build " << workspace << "!!! Exiting!!!" << endl;
			exit(EXIT_FAILURE);
		}
	}
}

bool build_node(string &node_name) {
	string command = "scan-build --use-analyzer=/usr/local/bin/clang -enable-checker debug.DumpReachConds make ";
	command += node_name;
	command += " --always-make";
	int status = system(command.c_str());
	if(status != 0) {
		return false;
	}
	return true;
}

void build_all_nodes(YAML::Node &nodes) {
	for (int i = 0; i < nodes.size(); ++i) {
		string workspace = nodes[i]["workspace"].as<string>();
		if(workspace.find("/opt/ros/")!=string::npos) {
			continue; //skip the nodes that are installed
		}

		bool result = change_directory(workspace);
		if(!result) {
			cerr << "Cannot cd/mkdir into " << workspace << "/build directory!!! Exiting!!!" << endl;
			exit(EXIT_FAILURE);
		}
		string node_name = nodes[i]["type"].as<string>();
		result = build_node(node_name);
		if(!result) {
			cerr << "Cannot build " << workspace << "!!! Exiting!!!" << endl;
			exit(EXIT_FAILURE);
		}
	}
}



int main(int argCount, char** argValues) {

	if(argCount < 2)
	{
		cerr << "Usage: software_builder <input_node_file>. Exiting" << endl;
		exit(EXIT_FAILURE);
	}

	YAML::Node node = YAML::LoadFile(argValues[1]);

	YAML::Node workspaces = node["workspaces"];
	build_all_workspaces(workspaces);

	YAML::Node nodes = node["nodes"];
	build_all_nodes(nodes);

	return 0;
}