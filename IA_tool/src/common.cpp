//
// Created by nishant on 3/4/16.
//
#include <Common.h>

//defining extern variables
vector<string> AffectedComponents;
vector<Color> StoppingColors;
int PenWidth = 5;
string YAMLFileList;
string graphOutFilename;
string analysisOutFilename;
string affectedTopicFilename;
string stoppingColorsFilename;


string getStringForColor(Color color)
{
	string colorValue = "none";
	if(color == Color::RED)
		colorValue = "red";
	else if(color == Color::GREEN)
		colorValue = "green";
	else if(color == Color::YELLOW)
		colorValue = "yellow";
	else if(color == Color::BLUE)
		colorValue = "blue";
	else if(color == Color::LIGHT_BLUE)
		colorValue = "lightblue";
	else if(color == Color::MAROON)
		colorValue = "maroon";
	else if(color == Color::CYAN)
		colorValue = "cyan";
	else
		colorValue = "none";
	return colorValue;
}

Color getColorForString(string colorValue)
{
	Color color;
	if(colorValue == "red")
		color = Color::RED;
	else if(colorValue == "green")
		color = Color::GREEN;
	else if(colorValue == "yellow")
		color = Color::YELLOW;
	else if(colorValue == "blue")
		color = Color::BLUE;
	else if(colorValue == "lightblue")
		 color = Color::LIGHT_BLUE;
	else if(colorValue == "maroon")
		 color = Color::MAROON;
	else if(colorValue == "cyan")
		 color = Color::CYAN;
	else
		color = Color::NONE;
	return color;
}
