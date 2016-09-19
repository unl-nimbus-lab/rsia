/*
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/*
 * @author: Nishant Sharma
 */



#ifndef ROSLAUNCH_READER_HELPER_FUNCTIONS_H
#define ROSLAUNCH_READER_HELPER_FUNCTIONS_H

#include <string>
#include <sstream>
#include <vector>
#include <assert.h>

using namespace std;

/*
 * returns the result of a system call as a string
 */
std::string execute(const char *cmd);

/*
 * split string based on the delimiter
 */
vector<string> &split(const string &s, char delim, vector<string> &elems);
vector<string> split(const string &s, char delim);

/*
 * Splits and returns the first $(<argument>) in the string
 */
vector<string> split_args(const string &s, string &arg);

#endif //ROSLAUNCH_READER_HELPER_FUNCTIONS_H
