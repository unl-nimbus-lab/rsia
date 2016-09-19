/*
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/**
 * \file
 *
 * \brief Interface for extracting RosParams
 *
 * This file provides interface for extracting information in different form
 * from the RosParam list via doing a reverse lookup on StringLiterals from any stmt and matching
 * it to the rosparams list keys.
 *
 * \author Nishant Sharma
 *
 * \version 0.5
 *
 * \date 19th June, 2016
 *
 * \pre RosParam list should already be exported into a file.
 *
 * Contact: nsharma@cse.unl.edu
 */

#ifndef LLVM_CLANG_ANALYSIS_ANALYSES_RosParamAnalysis_H
#define LLVM_CLANG_ANALYSIS_ANALYSES_RosParamAnalysis_H


#include "clang/AST/Decl.h"
#include "clang/Analysis/Analyses/CFG.h"
#include "string"
#include "map"
#include "yaml/yaml-cpp.h"

using namespace std;

namespace clang{
	/**
	 * \class RosParamAnalysis
	 *
	 * \brief Interface for extracting RosParams
	 *
	 * This class provides interface for extracting information in different form
 	 * from the RosParam list via doing a reverse lookup on StringLiterals from any stmt and matching
	 * it to the rosparams list keys.
	 *
	 * If the variable is matched to multiple decelerations they are returned using ' = ' as a delimiter or user can specify that as well.
	 *
	 */
	class RosParamAnalysis{
	public:
		/**
		 * \brief Instantiate Ros Parameter analysis
		 *
		 * Will read ros params from "/tmp/rosparams.yml"
		 *
		 * \param cfg control flow graph of the function being analyzed
		 * \param ns namespace of the rosnode
		 */
		RosParamAnalysis(const CFG *cfg, string ns);

		/**
		 * \brief Instantiate Ros Parameter analysis
		 *
		 * \param cfg control flow graph of the function being analyzed
		 * \param ns namespace of the rosnode
		 * \param filename from where we will read the ros parameters
		 */
		RosParamAnalysis(const CFG *cfg, string ns, string filename);

		/**
		 * \brief returns a std::string for any given variable
		 *
		 * \param varDecl for which the client wants the string resolved
		 */
		string getResolvedString(const VarDecl * varDecl);

	private:

		/**
		 * \brief This function explores CFG to fill in ros param values
		 */
		void populateRosParams();

		/// To store the namespace of the node
		string ns;

		/// To store the map of VarDecl* to it's namespace(ish) string
		map<const VarDecl* , string> varDeclToNS;

		/// To store the map of VarDecl* to it's ros parameter values
		map<const VarDecl* , string> varDeclToRosParams;

		/// To store the map of rosparams
		YAML::Node rosParamMap;
	};

} //end namespace clang


#endif//LLVM_CLANG_ANALYSIS_ANALYSES_RosParamAnalysis_H



/*
 * Taken out from the rospattern analysis
 */

/////hold the information about the node being compiled
//RosNode rosNode;
/////hold the rosparam list in YAML format
//YAML::Node rosparams;
//
////read and store the information about the node being compiled
//void resolveRosNode();
////read and store the list of ros parameters
//void readRosParams();
//
////function which takes a topic variable and tries to get's it's topic name
//void resolveTopicVariable(string &topic);
//
////given a string return it's corresponding ros param (resolving private header and everything else)
//void resolveRosParam(string &rosparam);
//
////reads through ROSPARAM list to get proper topic details
//string resolveVariableSequence(YAML::Node &node);
//string resolveVariableMap(YAML::Node &node);
//
//
//void RosPatternsImpl::resolveRosParam(string &rosparam) {
//	/*
//	 * if rosparam is absolute do nothing, otherwise resolve using namespace + "/" + value
//	 */
//	if(rosparam[0] == '/') {
//		return;
//	}
//	string parameter = "";
//	string ns = rosNode.ns;
//	if(!ns.empty()){
//		parameter = ns;
//	}
//	parameter += "/";
//	parameter += rosparam;
//	rosparam = parameter;
//}
//
//string RosPatternsImpl::resolveVariableSequence(YAML::Node &node) {
//	string topic = "";
//
//	/*
//	 * If it's a scalar : assume all other are scalars as well. traverse and append
//	 */
//	if(node[node.size()-1].IsScalar())
//	{
//		for (unsigned int i = 0; i < node.size(); ++i) {
//			if(node[i].IsScalar()) //just checking for safety
//			{
//				topic += node[i].as<string>();
//				topic += "-";
//			}
//		}
//		return topic;
//	}
//	/*
//	 * If it's a sequence: assume all other are sequences as well. traverse and append
//	 */
//	//One level of sequence
//	/*if(node[node.size()-1].IsSequence())
//	{
//		for (unsigned int i = 0; i < node.size(); ++i) {
//			if(node[i].IsSequence()) //just checking for safety
//			{
//				YAML::Node intermediateNode = node[i];
//				topic += resolveVariableSequence(intermediateNode);
//				topic += "-";
//			}
//		}
//		return topic;
//	}*/
//	/*
//	 * If it's a map: assume all other are map as well. traverse (find a topic key, if found append otherwise continue) and append
//	 */
//	if(node[node.size()-1].IsMap())
//	{
//		for (unsigned int i = 0; i < node.size(); ++i) {
//			if(node[i].IsMap()) //just checking for safety
//			{
//				YAML::Node intermediateNode = node[i];
//				topic += resolveVariableMap(intermediateNode);
//				topic += "-";
//			}
//		}
//		return topic;
//	}
//	return topic;
//}
//
//string RosPatternsImpl::resolveVariableMap(YAML::Node &node) {
//	/*
//	 * If it's a map: find a "topic"/"topics" key, if found append otherwise continue and append
//	 * NOTE: We are only looking for topics in keys otherwise the use will specify and also we are not exploring multi level maps
//	 */
//	if(node["topic"])
//	{
//		if(node["topic"].IsScalar())
//		{
//			return node["topic"].as<string>();
//		}
//		if(node["topic"].IsSequence())
//		{
//			YAML::Node intermediateNode = node["topic"];
//			return resolveVariableSequence(intermediateNode);
//		}
//	}
//	if(node["topics"])
//	{
//		if(node["topics"].IsScalar())
//		{
//			return node["topics"].as<string>();
//		}
//		if(node["topics"].IsSequence())
//		{
//			YAML::Node intermediateNode = node["topics"];
//			return resolveVariableSequence(intermediateNode);
//		}
//	}
//	return "";
//}
//
//void RosPatternsImpl::resolveTopicVariable(string &topic) {
//	string variable = topic;
//
//	map<string, string>::iterator it = variableToRosparam.find(variable);
//
//	if(it != variableToRosparam.end())
//	{
//		YAML::Node node = rosparams[it->second];
//		//if it's scalar (just one value) -store it as string
//		if(node.IsScalar())
//			topic = node.as<string>();
//		if(node.IsSequence())
//			topic = resolveVariableSequence(node);
//		if(node.IsMap())
//			topic = resolveVariableMap(node);
//	}
//	else{
//		topic = "UNKNOWN";
//	}
//	if(topic.empty())
//	{
//		topic = "UNKNOWN";
//	}
//}
//
//
//void RosPatternsImpl::resolveRosNode() {
////the file is expected to be at a fixed location
//	YAML::Node rosNode = YAML::LoadFile("/tmp/node.yml");
//	if(!rosNode["name"])
//	{
//		//if name is not present fill default;
//		this->rosNode.name = "UnKnown";
//		this->rosNode.type = "UnKnown";
//		this->rosNode.package = "UnKnown";
//		this->rosNode.ns = "/";
//		return;
//	}
//	//parse the document
//	this->rosNode.name = rosNode["name"].as<string>();
//	this->rosNode.type = rosNode["type"].as<string>();
//	this->rosNode.package = rosNode["pkg"].as<string>();
//	this->rosNode.ns = rosNode["ns"].as<string>();
//	this->rosNode.args = rosNode["args"].as<string>();
//	for (unsigned int i = 0; i < rosNode["remap"].size(); ++i) {
//		TopicRemap remap;
//		remap.from = rosNode["remap"][i]["from"].as<string>();
//		remap.from = rosNode["to"][i]["to"].as<string>();
//		this->rosNode.remaps.push_back(remap);
//	}
//}
//
//void RosPatternsImpl::readRosParams() {
//	this->rosparams = YAML::LoadFile("/tmp/rosparams.yml");
//}


//				if(!calleeNameString.compare("getParam")
//						|| !calleeNameString.compare("param")){
//					const CXXMemberCallExpr *mce = dyn_cast<CXXMemberCallExpr>(S); //we expect the stmt to be a CXXMember call expr at this point
//					string rosparam = "";
//					bool isString = isArgAString(mce,0/*subscribe topic is first argument*/,rosparam);
//					if(!isString)
//					{
//							continue;
//					}
//					resolveRosParam(rosparam);
//					string variable = "";
//					bool isStringVar = isArgAString(mce,1/*subscribe topic is first argument*/,variable);
//					if(isStringVar)
//					{
//						//should not have happened
//						continue;
//					}
//					/*
//					 * if doesn't exist add it
//					 */
//					if(!rosparams[rosparam])
//					{
//						string value = "";
//						int noParams = mce->getDirectCallee()->getNumParams();
//						if(noParams < 3)
//						{
//							value = "UNKNOWN";
//						}
//						else
//						{
//							isArgAString(mce,2/*subscribe topic is first argument*/,value);
//						}
//						rosparams[rosparam] = value;
//					}
//					variableToRosparam[variable] = rosparam;
//					cout << rosparams[rosparam] << endl;
//				}