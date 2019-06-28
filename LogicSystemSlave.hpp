#ifndef LOGICSYSTEMSLAVE
#define LOGICSYSTEMSLAVE

#include <cstring>
#include <vector>
#include <map>

#include "ServerJ.hpp"
#include "Metadata.hpp"

class LogicSystemSlave : public System
{
public:
    LogicSystemSlave(Server* owner);
    ~LogicSystemSlave();
    void runCommandLine(const std::vector<std::string>& argv, std::string& placeholder);

private:
    Server* owner;
    std::map<int, Metadata*> metadata_map;
    void shakehand(const std::vector<std::string>& argv, std::string& placeholder);
    void request(const std::vector<std::string>& argv, std::string& placeholder);
    void remove(const std::vector<std::string>& argv, std::string& placeholder);
    void create(const std::vector<std::string>& argv, std::string& placeholder);
    void update(const std::vector<std::string>& argv, std::string& placeholder);
    void pushto(const std::vector<std::string>& argv, std::string& placeholder);
};

#endif
