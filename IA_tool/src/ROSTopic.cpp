//
// Created by nishant on 3/4/16.
//

#include "ROSTopic.h"

/////////////////////////////class ROSNode////////////////////////////////////////////////////

/* Constructors */

ROSTopic::ROSTopic()
{
	this->topicName = "undefined";
	this->label = "none";
	this->color = Color::NONE;
	this->_isBold = false;
}

ROSTopic::ROSTopic(const string &topicName,
				   const string &label,
				   Color &color) :
		topicName(topicName),
		label(label),
		color(color)
{
	this->_isBold = false;
}


/* setters */
void ROSTopic::setTopicName(string topicName)
{
	this->topicName = topicName;
}

void ROSTopic::setLabel(string label)
{
	this->label = label;
}

void ROSTopic::setColor(Color color)
{
	this->color = color;
}

void ROSTopic::setBold(bool bold)
{
	this->_isBold = bold;
}

/* getters */
string ROSTopic::getTopicName()
{
	return this->topicName;
}

string ROSTopic::getLabel()
{
	return this->label;
}

Color  ROSTopic::getColor()
{
	return this->color;
}

bool ROSTopic::isBold()
{
	return this->_isBold;
}

/* Other Functions */

Node ROSTopic::encodeYAML()
{
	Node node;
	node["type"] = "ROSTopic";
	node["name"] = this->getTopicName();
	node["label"] = this->getLabel();
	node["color"] = getStringForColor( this->getColor() );
	return node;
}

void ROSTopic::decodeYAML(Node node)
{
	//assert( this != NULL && "ROSTopic Object is NULL" );
	assert( node["type"].as<string>() == "ROSTopic" && "YAML node being decoded is not ROSTopic type" );
	this->setTopicName( node["name"].as<string>() );
	this->setLabel( node["label"].as<string>() );
	this->setColor( getColorForString( node["color"].as<string>() ) );
}

bool ROSTopic::DOTWriter(ofstream &file)
{
	assert(file && "File object is null");
	file << " ";
	file << this->getTopicName();
	file << " [ shape = box,";
	file <<	"label=\"";
	file << this->getLabel();
	file << "\"";
	if (this->getColor() != Color::NONE)
	{
		file << ", style = filled , color=\"";
		file << getStringForColor(this->color);
		file << "\"";
	}
	if(this->_isBold)
	{
		file << " , penwidth = ";
		file << PenWidth;
	}
	file << "];";
	file << endl;

	return true;
}
