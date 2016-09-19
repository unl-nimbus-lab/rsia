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
 * @date 2016/05/16
 * @version 1.0
 */

#include <Function.h>
#include <iostream>


void Function::Dump()
{
	PrintInCyan(this->name);							cout << " -- ";
	PrintInYellow(convert(this->dependency));			cout << endl;
	PrintInGreen(convert(this->timePattern));			cout << endl;
	if(this->isCallback)
		cout << "It's a callback function" << endl;

	cout << endl;
	PrintInRed("Function calls :->");
	cout << endl;
	for(Function fn : this->functionCalls)
	{
		cout << "\t" << fn.name << endl;
	}

	cout << endl;
	PrintInRed("Publish calls :->");
	cout << endl;
	for(PublishCall pb: this->publishCalls)
	{
		cout << "\t" << pb.topic << endl;
	}
}