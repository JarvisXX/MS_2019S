#ifndef MSYSTEM
#define MSYSTEM

#include <vector>
#include <string>

using namespace std;

class System
{
public:
    virtual void runCommandLine(const vector<string>& argv, string& placeholder) = 0;
};

#endif
