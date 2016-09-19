//
// Created by nishant on 3/4/16.
//

#include "ROSEdge.h"


ROSEdge::ROSEdge()
{
	this->color = Color::NONE;
	this->_isBold = false;
}

ROSEdge::ROSEdge(const string &source,
				 const string &destination,
				 const string &label,
				 Color color) :
		source(source),
		destination(destination),
		label(label),
		color(color)
{
	this->color = Color::NONE;
	this->_isBold = false;
}


/* setters */
void ROSEdge::setSource(string source)
{
	this->source = source;
}

void ROSEdge::setDestination(string destination)
{
	this->destination = destination;
}

void ROSEdge::setLabel(string label)
{
	this->label = label;
}

void ROSEdge::setColor(Color color)
{
	this->color = color;
}

void ROSEdge::setBold(bool bold)
{
	this->_isBold = bold;
}

/* getters */
string ROSEdge::getSource()
{
	return this->source;
}

string ROSEdge::getDestination()
{
	return this->destination;
}

string ROSEdge::getLabel()
{
	return this->label;
}

Color ROSEdge::getColor()
{
	return this->color;
}

bool ROSEdge::isBold()
{
	return this->_isBold;
}

/* YAML encoder - decoder */
Node ROSEdge::encodeYAML()
{
	Node node;
	node["type"] = "ROSEdge";
	node["source"] = this->getSource();
	node["destination"] = this->getDestination();
	node["label"] = this->getLabel();
	node["color"] = getStringForColor( this->getColor() );
	return node;
}

void ROSEdge::decodeYAML(Node node)
{
	//assert( this != NULL && "ROSEdge Object is NULL" );
	assert( node["type"].as<string>() == "ROSEdge" && "YAML node being decoded is not ROSEdge type" );
	this->setSource( node["source"].as<string>() );
	this->setDestination( node["destination"].as<string>() );
	this->setLabel( node["label"].as<string>() );
	this->setColor( getColorForString( node["color"].as<string>() ) );
}

/* DOT Writer */
bool ROSEdge::DOTWriter(ofstream &file)
{
	assert(file && "File object is null");
	file << " ";
	file << this->getSource();
	file << " -> ";
	file << this->getDestination();
	file << " [";
	bool isFirst = false;
	bool isSecond = false;
	if(!this->label.empty() && !(this->label == "NONE" || this->label == "none"))
	{
		file << "label=\"";
		file << this->getLabel();
		file << "\"";
		isFirst = true;
	}
//	if(!this->label.empty() && !(this->label == "NONE" || this->label == "none") && this->getColor() != Color::NONE)
//	{
//		file << " , ";
//	}
	if (this->getColor() != Color::NONE)
	{
		if(isFirst)
		{
			file << " , " ;
		}
		file << "color=\"";
		file << getStringForColor(this->color);
		file << "\"";
		isSecond = true;
	}
	if(this->isBold())
	{
		if(isFirst || isSecond)
		{
			file << " , ";
		}
		file << "penwidth=";
		file << PenWidth;
	}
	file << "];";
	file << endl;

	return true;
}