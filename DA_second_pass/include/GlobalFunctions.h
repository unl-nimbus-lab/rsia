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



#ifndef PHASETWO_GLOBALFUNCTIONS_H
#define PHASETWO_GLOBALFUNCTIONS_H
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <assert.h>

#include "Function.h"
#include <DataStructures.h>
#include <set>
#include <stack>

using namespace std;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

Dependency MergeDependency(Dependency current, Dependency other);
Dependency UpgradeDependency(Dependency current, Dependency other);

void PrintInYellow(string str);
void PrintInRed(string str);
void PrintInGreen(string str);
void PrintInCyan(string str);


#endif //PHASETWO_GLOBALFUNCTIONS_H
