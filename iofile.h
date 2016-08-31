#ifndef IOFILE_H
#define IOFILE_H
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class IOFile
{
public:
    IOFile();
    ~IOFile();

    std::string getElement(std::string section, std::string key);
    std::vector<std::string> str;
};

#endif // IOFILE_H
