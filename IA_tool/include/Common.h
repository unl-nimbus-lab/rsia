/*
888b    888 d8b               888                             888               888
8888b   888 Y8P               888                             888               888
88888b  888                   888                             888               888
888Y88b 888 888 88888b.d88b.  88888b.  888  888 .d8888b       888       8888b.  88888b.
888 Y88b888 888 888 "888 "88b 888 "88b 888  888 88K           888          "88b 888 "88b
888  Y88888 888 888  888  888 888  888 888  888 "Y8888b.      888      .d888888 888  888
888   Y8888 888 888  888  888 888 d88P Y88b 888      X88      888      888  888 888 d88P
888    Y888 888 888  888  888 88888P"   "Y88888  88888P'      88888888 "Y888888 88888P"
 */
//
// Created by nishant on 2/4/16.
//

#ifndef GRAPHTRAVERSAL_COMMON_H
#define GRAPHTRAVERSAL_COMMON_H

#include <string>
#include <vector>
#include <map>
#include <yaml-cpp/yaml.h>
#include <assert.h>
#include <fstream>

#define DEPRECATED_DISABLED

using namespace std;



enum class Color{
	RED,
	GREEN,
	YELLOW,
	BLUE,
	LIGHT_BLUE,
	MAROON,
	CYAN,
	NONE
};

string getStringForColor(Color color);
Color getColorForString(string colorValue);


/* Global Details */
extern vector<string> AffectedComponents;
extern vector<Color> StoppingColors;
extern int PenWidth;
extern string YAMLFileList;
extern string graphOutFilename;
extern string analysisOutFilename;
extern string affectedTopicFilename;
extern string stoppingColorsFilename;

#endif //GRAPHTRAVERSAL_COMMON_H
