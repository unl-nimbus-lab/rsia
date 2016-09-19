/**
888b    888 d8b               888                             888               888
8888b   888 Y8P               888                             888               888
88888b  888                   888                             888               888
888Y88b 888 888 88888b.d88b.  88888b.  888  888 .d8888b       888       8888b.  88888b.
888 Y88b888 888 888 "888 "88b 888 "88b 888  888 88K           888          "88b 888 "88b
888  Y88888 888 888  888  888 888  888 888  888 "Y8888b.      888      .d888888 888  888
888   Y8888 888 888  888  888 888 d88P Y88b 888      X88      888      888  888 888 d88P
888    Y888 888 888  888  888 88888P"   "Y88888  88888P'      88888888 "Y888888 88888P"
 */

/**
 * @author Nishant Sharma
 * @date 2016/05/12
 * @version 1.0
 */



#include "../include/GlobalFunctions.h"


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

Dependency MergeDependency(Dependency current, Dependency other)
{
	//a maybe is a maybe no matter what
	if(current == MAYBE_DEPENDENT)
		return MAYBE_DEPENDENT;
	//if current is a green maybe then final can be either maybe or greenmaybe
	if(current == GREEN_MAYBE_DEPENDENT)
	{
		if(other == MAYBE_DEPENDENT)
		{
			return MAYBE_DEPENDENT;
		}
		else if(other == DEPENDENT)
		{
			return MAYBE_DEPENDENT;
		}
		else
			return GREEN_MAYBE_DEPENDENT;
	}
	//if current is independent than it can either be independent or maybe or green maybe
	if(current == INDEPENDENT)
	{
		if(other == INDEPENDENT)
			return INDEPENDENT;
		else if (other == GREEN_MAYBE_DEPENDENT)
			return GREEN_MAYBE_DEPENDENT;
		else
			return MAYBE_DEPENDENT;
	}
	//if current is dependent than it can either be dependent of maybe
	if(current == DEPENDENT)
	{
		if(other == DEPENDENT)
			return DEPENDENT;
		else
			return MAYBE_DEPENDENT;
	}
	return DEPENDENT;
}

Dependency UpgradeDependency(Dependency current, Dependency other)
{
	//if current is INDEPENDENT : it's independent no matter what
	if(current == INDEPENDENT)
		return INDEPENDENT;
	//for green it can get upgraded to INDEPENDENT otherwise it's GREEN MAYBE
	if(current == GREEN_MAYBE_DEPENDENT)
	{
		if(other == INDEPENDENT)
		{
			return INDEPENDENT;
		}
		else
		{
			return GREEN_MAYBE_DEPENDENT;
		}
	}
	//if current is a maybe then final can be either maybe or greenmaybe or independent
	if(current == MAYBE_DEPENDENT)
	{
		if(other == INDEPENDENT)
		{
			return INDEPENDENT;
		}
		else if(other == GREEN_MAYBE_DEPENDENT)
		{
			return GREEN_MAYBE_DEPENDENT;
		}
		else
			return MAYBE_DEPENDENT;
	}
	//if current is dependent than it can either be anything
	if(current == DEPENDENT)
	{
		if(other == INDEPENDENT)
		{
			return INDEPENDENT;
		}
		else if(other == GREEN_MAYBE_DEPENDENT)
		{
			return GREEN_MAYBE_DEPENDENT;
		}
		else if(other == MAYBE_DEPENDENT)
		{
			return MAYBE_DEPENDENT;
		}
		else
		{
			return DEPENDENT;
		}
	}
	return DEPENDENT;
}

void PrintInYellow(string str)
{
	cout <<  "\033[1;33m" ; //turn yellow on
	cout << str;
	cout <<  "\033[0m" ; //turn yellow off
}

void PrintInRed(string str)
{
	cout <<  "\033[1;31m" ; //turn red on
	cout << str;
	cout <<  "\033[0m" ; //turn red off
}

void PrintInGreen(string str)
{
	cout <<  "\033[1;32m" ; //turn green on
	cout << str;
	cout <<  "\033[0m" ; //turn green off
}

void PrintInCyan(string str)
{
	cout <<  "\033[1;36m" ; //turn cyan on
	cout << str;
	cout <<  "\033[0m" ; //turn cyan off
}

