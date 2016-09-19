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


#include <helper_functions.h>
#include <memory>

std::string execute(const char *cmd) {
	char buffer[128];
	std::string result = "";
	std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
	if (!pipe) throw std::runtime_error("popen() failed!");
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			result += buffer;
	}
	return result;
}

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}


vector<string> split_args(const string &s, string &arg){
	arg = "";
	vector<string> elems;
	string temp;
	bool is_arg = false;
	int i =0;
	for (i = 0; i < s.size(); ++i) {
		if(s[i] == '$' && s[i+1] == '(')
		{
			//skip the next '('
			i=i+1;
			is_arg = true;
			//push anything till now to the vector and reset the temp string;
			elems.push_back(temp);
			temp = "";
			continue;
		}
		if(is_arg && s[i] == ')')
		{
			break;
			// add the rest of the string to the elems
		}

		//if is_arg is set add to argument
		if(is_arg)
			arg += s[i];
		else
		{
			temp += s[i];
		}
	}

	//now push the remaining string to the vector
	elems.push_back(s.substr(i+1));
	return elems;
}