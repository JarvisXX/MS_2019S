#include <iostream>
#include <cstring>
#include <vector>
#include <map>

#include "LogicSystemMaster.hpp"

using namespace std;

bool isOptionKW(const string& arg) {
    if (arg.size() == 0) {
        printf("Error: arg should never be empty string. debug it.");
        exit(1);
    }
    return arg[0] == '-';
}

LogicSystemMaster::LogicSystemMaster(Server* owner) {
    this->system_tree = new SystemTree(this);
    this->owner = owner;
}

LogicSystemMaster::~LogicSystemMaster() {
    delete this->system_tree;
}

void LogicSystemMaster::pwd(const vector<string>& argv, string& placeholder) {
    this->system_tree->getWorkingDirectory(placeholder);
}

void LogicSystemMaster::mkdir(const vector<string>& argv, string& placeholder) {
    if (argv.size() < 2)
        placeholder = "Error: mkdir accepts 1 argument.\n";
    else {
        const string& name = argv[1];
        this->system_tree->makeDirectory(name, placeholder);
    }
}

void LogicSystemMaster::ls(const vector<string>&argv, string& placeholder) {
    string src = ".";
    bool recursive = false;
    int len = argv.size();
    for (int i=1; i<len; i++) {
        if (isOptionKW(argv[i])) {
            if (argv[i] == "-r")
                recursive = true;
            else
                placeholder = "Error: option/argument " + argv[i] + " is not supported.\n";
        }
        else
            src = argv[i];
    }
    this->system_tree->list(src, recursive, placeholder);
}

void LogicSystemMaster::readdir(const vector<string>& argv, string& placeholder) {
    string src = ".";
    if (argv.size() > 1)
        src = argv[1];
    this->system_tree->list(argv[1], false, placeholder);
}

void LogicSystemMaster::cd(const vector<string>& argv, string& placeholder) {
    if (argv.size() == 1)
        placeholder = "Error: cd accepts 1 argument.\n";
    else
        this->system_tree->changeDirectory(argv[1], placeholder);
}

void LogicSystemMaster::mv(const vector<string>& argv, string& placeholder) {
    if (argv.size() < 3)
        placeholder = "Error: mv accepts 2 argument.\n";
    else
        this->system_tree->move(argv[1], argv[2], placeholder);
}

void LogicSystemMaster::stat(const vector<string>& argv, string& placeholder) {
    if (argv.size() == 1)
        placeholder = "Error: stat accepts 1 argument.\n";
    else
        this->system_tree->state(argv[1], placeholder);
}

void LogicSystemMaster::touch(const vector<string>& argv, string& placeholder) {
    if (argv.size() == 1)
        placeholder = "Error: touch accepts 1 argument.\n";
    else
        this->system_tree->touch(argv[1], placeholder);
}

void LogicSystemMaster::rm(const vector<string>& argv, string& placeholder) {
    string src = "";
    bool recursive = false;
    int len = argv.size();
    for (int i=1; i<len; i++) {
        if (isOptionKW(argv[i])) {
            if (argv[i] == "-r")
                recursive = true;
            else
                placeholder = "Error: option/argument " + argv[i] + " is not supported.\n";
        }
        else
            src = argv[i];
    }
    if (src == "")
        placeholder = "Error: rm accepts 1 positional arguments.\n";
    else
        this->system_tree->remove(src, recursive, placeholder);
}

void LogicSystemMaster::runCommandLine(const vector<string>& argv, string& placeholder) {
    if (argv.size() == 0) {
        printf("Error: argv size should never be zero. debug it.\n");
        exit(1);
    }
    const string& command = argv[0];
    if (command == "pwd")
        this->pwd(argv, placeholder);
    else if (command == "mkdir")
        this->mkdir(argv, placeholder);
    else if (command == "ls")
        this->ls(argv, placeholder);
    else if (command == "readdir")
        this->readdir(argv, placeholder);
    else if (command == "cd")
        this->cd(argv, placeholder);
    else if (command == "mv")
        this->mv(argv, placeholder);
    else if (command == "stat")
        this->stat(argv, placeholder);
    else if (command == "touch")
        this->touch(argv, placeholder);
    else if (command == "rm")
        this->rm(argv, placeholder);
    else
        placeholder = "Error: command " + command + " is not supported.\n";
}
