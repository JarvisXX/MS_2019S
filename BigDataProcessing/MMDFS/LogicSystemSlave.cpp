#include <iostream>
#include <cstring>
#include <vector>

#include "LogicSystemSlave.hpp"

using namespace std;

LogicSystemSlave::LogicSystemSlave(Server* owner): metadata_map(), owner(owner) {}

LogicSystemSlave::~LogicSystemSlave() {
    for (auto it = metadata_map.begin(); it != metadata_map.end(); it++) {
        delete it->second;
        metadata_map.erase(it);
    }
}

void LogicSystemSlave::shakehand(const vector<string>& argv, string& placeholder) {
    placeholder = "hello world";
}

void LogicSystemSlave::pushto(const vector<string>& argv, string& placeholder) {
    placeholder = "Success";
    string ip = argv[1];
    int port = atoi(argv[2].c_str());
    for (auto it=metadata_map.begin(); it!=metadata_map.end(); it++) {
        string line;
        string message = "create " + it->second->summary();
        this->owner->sendTo(ip, port, message, line);
        if (line != "Success" && line != "Error: file id already exists.")
            placeholder = "Failure";
    }
}

void LogicSystemSlave::runCommandLine(const vector<string>& argv, string& placeholder) {
    if (argv.size() == 0) {
        printf("argv size should never be zero. debug it.\n");
        exit(1);
    }
    const string& command = argv[0];
    if (command == "hello")
        this->shakehand(argv, placeholder);
    else if (command == "request")
       this->request(argv, placeholder);
    else if (command == "delete")
        this->remove(argv, placeholder);
    else if (command == "create")
      this->create(argv, placeholder);
    else if (command == "update")
        this->update(argv, placeholder);
    else if (command == "pushto")
        this->pushto(argv, placeholder);
    else
        placeholder = "Error: command " + command + " is not supported.\n";
    std::cout << "------metadata------" << std::endl;
    for (auto it=metadata_map.begin(); it!=metadata_map.end(); it++)
        std::cout << it->second->summary() << std::endl;}

void LogicSystemSlave::request(const std::vector<std::string>& argv, std::string& placeholder) {
    if (argv.size() != 2) {
        placeholder = "Error: wrong argument number for request.\n";
        return;
    }
    int id = atoi(argv[1].c_str());
    auto it = metadata_map.find(id);
    if (it == metadata_map.end()) {
        placeholder = "Error: request failed.";
        return;
    }
    placeholder = it->second->summary();
}

void LogicSystemSlave::remove(const std::vector<std::string>& argv, std::string& placeholder) {
    if (argv.size() != 2) {
        placeholder = "Error: wrong argument number for delete.";
        return;
    }
    int id = atoi(argv[1].c_str());
    auto it = metadata_map.find(id);
    if (it == metadata_map.end()) {
        placeholder = "Error: delete failed.";
        return;
    }
    delete it->second;
    metadata_map.erase(it);
    placeholder = "Success";
}

void LogicSystemSlave::create(const std::vector<std::string>& argv, std::string& placeholder) {
    if (argv.size() != 8) {
        placeholder = "Error: wrong argument number for delete.";
        return;
    }
    int id = atoi(argv[1].c_str());
    int parent = atoi(argv[2].c_str());
    const std::string& creation_time = argv[3];
    const std::string& lastmodify_time = argv[4];
    size_t size = atoi(argv[5].c_str());
    const std::string& name = argv[6];
    const int type = atoi(argv[7].c_str());
    Metadata* new_metadata = new Metadata(id, parent, creation_time, size, lastmodify_time, name, type);
    auto it = metadata_map.find(id);
    if (it != metadata_map.end()) {
        placeholder = "Error: file id already exists.";
        return;
    }
    metadata_map.insert(std::pair<int, Metadata*>(id, new_metadata));
    placeholder = "Success";
}

void LogicSystemSlave::update(const std::vector<std::string>& argv, std::string& placeholder) {
    if ((argv.size()) % 2 != 0) {
        placeholder = "Error: incomplete key value pairs.";
        return;
    }
    int id = atoi(argv[1].c_str());
    auto it = metadata_map.find(id);
    if (it == metadata_map.end()) {
        placeholder = "Error: file id doesn't exist.";
        return;
    }
    Metadata* metadata = metadata_map[id];
    for (int i = 2; i < argv.size(); i+=2) {
        if (argv[i] == "parent")
            metadata->updateParent(atoi(argv[i+1].c_str()));
        else if (argv[i] == "lastmodify_time")
            metadata->updateLastmodifyTime(argv[i+1]);
        else if (argv[i] == "name")
            metadata->updateName(argv[i+1]);
        else {
            placeholder = "Error: unknown key value.";
            return;
        }
        placeholder = "Success";
    }
}
