#include "iofile.h"


using namespace std;


IOFile::IOFile() {
	string file = "config.ini";
    string line;
    std::ifstream input;
    input.open(file.c_str());
    if (input.is_open()) {
		while (getline(input,line)) {
            str.push_back(line);
        }
    }
    input.close();
}

IOFile::~IOFile() {

}

// Updated
string IOFile::getElement(string section, string key) {

	/* Changes:
	   - For-Loops use str.size() instead of data->size().
	   - Added final return line.
	   - Would previously check all lines after section found,
		 now only checks until next section begins.
	   - Function str.size() is of variable size_t, not int.
		 Thus, For-Loops use size_t for iteration.
	 */

	std::string* data = str.data();

	for (size_t i = 0, strSizeI = str.size(); i < strSizeI; ++i) {
		if (data[i] == section) {
			for (size_t  j = i + 1, strSizeJ = str.size(); j < strSizeJ; ++j) {
				if (data[j].substr(0, 1) == "[") {
					return "nothing found";
				}
				if (data[j].substr(0,key.length()) == key) {
					return data[j].substr(key.size());
				}
			}
		}
	}

	return "nothing found";
}
