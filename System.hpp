#ifndef SYSTEM
#define SYSTEM

#include <cstring>
#include <vector>

using namespace std;

class System {
public:
    virtual void runCommandLine(const vector<string>& argv, string& placeholder) = 0;
};

#endif
