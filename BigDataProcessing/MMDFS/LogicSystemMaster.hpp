#ifndef LOGICSYSTEMMASTER
#define LOGICSYSTEMMASTER

#include <cstring>
#include <vector>

#include "SystemTree.hpp"
#include "Server.hpp"
#include "System.hpp"

class SystemTree;

class LogicSystemMaster : public System {
public:
    Server* owner;

    LogicSystemMaster(Server* owner);
    ~LogicSystemMaster();
    void runCommandLine(const std::vector<std::string>& argv, std::string& placeholder);

private:
    SystemTree* system_tree;

    void pwd(const std::vector<std::string>& argv, std::string& placeholder);
    void mkdir(const std::vector<std::string>& argv, std::string& placeholder);
    void ls(const std::vector<std::string>& argv, std::string& placeholder);
    void readdir(const std::vector<std::string>& argv, std::string& placeholder);
    void cd(const std::vector<std::string>& argv, std::string& placeholder);
    void mv(const std::vector<std::string>& argv, std::string& placeholder);
    void stat(const std::vector<std::string>& argv, std::string& placeholder);
    void rm(const std::vector<std::string>& argv, std::string& placeholder);
    void touch(const std::vector<std::string>& argv, std::string& placeholder);
};

#endif
