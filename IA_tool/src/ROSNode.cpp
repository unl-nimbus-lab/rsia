//
// Created by nishant on 2/4/16.
//

#include "ROSNode.h"

/////////////////////////////class ROSNode////////////////////////////////////////////////////

/* constructors */
ROSNode::ROSNode()
{
	this->nodeName = "undefined";
	this->label = "none";
	this->color = Color::NONE;
	this->_isBold = false;
}

ROSNode::ROSNode(string &label,
				 string &nodeName,
				 Color &color):
		nodeName(nodeName),
		label(label),
		color(color)
{
	this->_isBold = false;
}

/* setters */
void ROSNode::setNodeName(string nodename)
{
	this->nodeName = nodename;
}

void ROSNode::setLabel(string label)
{
	this->label = label;
}

void ROSNode::setColor(Color color)
{
	this->color = color;
}

void ROSNode::setBold(bool bold)
{
	this->_isBold = bold;
}

/* getters */
string ROSNode::getNodeName()
{
	return this->nodeName;
}

string ROSNode::getLabel()
{
	return this->label;
}

Color  ROSNode::getColor()
{
	return this->color;
}

bool ROSNode::isBold()
{
	return this->_isBold;
}


/* other functions */
Node ROSNode::encodeYAML()
{
	Node node;
	node["type"] = "ROSNode";
	node["name"] = this->getNodeName();
	node["label"] = this->getLabel();
	node["color"] = getStringForColor( this->getColor() );
	return node;
}

void ROSNode::decodeYAML(Node node)
{
	//assert( this != NULL && "ROSNode Object is NULL" );
	assert( node["type"].as<string>() == "ROSNode" && "YAML node being decoded is not ROSNode type" );
	this->setNodeName( node["name"].as<string>() );
	this->setLabel( node["label"].as<string>() );
	this->setColor( getColorForString( node["color"].as<string>() ) );
}

bool ROSNode::DOTWriter(ofstream &file)
{
	assert(file && "File object is null");
	file << " ";
	file << this->getNodeName();
	file << " [label=\"";
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

