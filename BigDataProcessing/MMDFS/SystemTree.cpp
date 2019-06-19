#include <iostream>
#include <cstring>
#include <ctime>

#include "API.hpp"
#include "SystemTree.hpp"

using namespace std;


void tokenizePath(const string& path, vector<string>& paths) {
    tokenize(path, paths, "/");
}

string getTimeStr() {
    time_t this_moment = time(0);
    string time_raw_str = string(ctime(&this_moment));
    vector<string> time_tokens;
    tokenize(time_raw_str, time_tokens);
    string time_fine_str = time_tokens[0];
    for (int i=1; i<time_tokens.size(); i++)
        time_fine_str += "-" + time_tokens[i];
    return time_fine_str;
}


// SystemTree::Node
int SystemTree::Node::_node_count = 0;

SystemTree::Node::Node(SystemTree* st, bool is_file): st(st), _object_id(Node::_node_count++), _is_file(is_file) {}

SystemTree::Node::Node(SystemTree* st): Node(st, false) {
    this->_parent = nullptr;
    this->_name = "~";
}

SystemTree::Node::Node(SystemTree* st, Node* parent, const string name, const bool is_file, const int size, string& placeholder): Node(st, is_file) {
    if (parent == nullptr) {
        printf("Error: parent should never be nullptr. debug it.");
        exit(1);
    }
    this->_parent = parent;
    this->_name = name;
    string creation_msg = this->creationMessage();
    this->systemCall(creation_msg, placeholder);
}

void SystemTree::Node::systemCall(const string& message, string& placeholder) const {
    if (this->_object_id == 0) {
        printf("Error: the root has no relation to metadata target server.\n");
        exit(1);
    }
    vector<int> slaveids;
    this->st->lsm->owner->slaveHash(this->_object_id, slaveids);
    this->st->lsm->owner->sendTo(slaveids, message, placeholder);
}

string SystemTree::Node::creationMessage() const {
    string str = "create ";
    str += to_string(this->_object_id) + " ";
    if (this->_parent == nullptr)
        str += "-1 ";
    else
        str += to_string(this->_parent->getObjectId()) + " ";
    string timestr = getTimeStr();
    str += timestr + " ";
    str += timestr + " ";
    str += "0 ";
    str += this->_name + " ";
    str += string(this->_is_file?"1":"0") + " ";
    return str;
}

const string& SystemTree::Node::name() const {
    return this->_name;
}

SystemTree::Node* const& SystemTree::Node::parent() const {
    return this->_parent;
}

const vector<SystemTree::Node*>& SystemTree::Node::children() const {
    return this->_children;
}

int SystemTree::Node::contain(const string& name) const {
    int len = this->_children.size();
    for (int i=0; i<len; i++) {
        if (this->_children[i]->_name == name)
            return i;
    }
    return -1;
}

void SystemTree::Node::moveTo(const string& name, Node*& dest) {
    if (name == "..")
        dest = this->_parent;
    else if (name == ".")
        dest = this;
    else {
        int id = this->contain(name);
        if (id < 0)
            dest = nullptr;
        else
            dest = this->_children[id];
    }
}

void SystemTree::Node::detach() {
    int num_siblings = this->_parent->_children.size();
    for (int i=0; i<num_siblings; i++) {
        if (this->_parent->_children[i]->_name == this->_name) {
            this->_parent->_children.erase(this->_parent->_children.begin()+i);
            break;
        }
    }
    this->_parent = nullptr;
}

void SystemTree::Node::attachTo(Node* new_parent) {
    if (new_parent == nullptr) {
        printf("Error: new_parent should never be nullptr. debug it.\n");
        exit(1);
    }
    else if (new_parent->_is_file) {
        printf("Error: new_parent should never be a file. debug it.\n");
        exit(1);
    }
    this->_parent = new_parent;
    this->_parent->_children.push_back(this);
}

bool SystemTree::Node::isFile() const {
    return this->_is_file;
}

void SystemTree::Node::reattachTo(Node* new_parent, string& placeholder) {
    this->detach();
    this->attachTo(new_parent);
    string message = this->updateMessage("parent", to_string(new_parent->_object_id));
    this->systemCall(message, placeholder);
}

SystemTree::Node* const& SystemTree::Node::addChild(const string& name, const bool is_file, const int size, string& placeholder) {
    if (this->_is_file) {
        printf("Error: this should never be a file. debug it.\n");
        exit(1);
    }
    Node* child = new Node(this->st, this, name, is_file, size, placeholder);
    this->_children.push_back(child);
    return this->_children.back();
}

void SystemTree::Node::setName(const string& name, string& placeholder) {
    this->_name = name;
    string message = this->updateMessage("name", name);
    this->systemCall(message, placeholder);
}

void SystemTree::Node::releaseChild(int i, string& placeholder) {
    string message = "delete " + to_string(this->_children[i]->_object_id);
    this->_children[i]->systemCall(message, placeholder);
    delete this->_children[i];
    this->_children.erase(this->_children.begin()+i);
}

void SystemTree::Node::releaseChild(Node* ch, string& placeholder) {
    for (int i=0; i<this->_children.size(); i++) {
        if (this->_children[i]->_object_id == ch->_object_id) {
            this->releaseChild(i, placeholder);
            return;
        }
    }
    printf("Error: ch should always be a valid child of this. debug it.\n");
    exit(1);
}

string SystemTree::Node::updateMessage(const string& key, const string& value) const {
    return "update " + to_string(this->_object_id) + " " + key + " " + value;
}

void SystemTree::Node::updateNothing(string& placeholder) {
    string timestr = getTimeStr();
    string message = this->updateMessage("lastmodify_time", timestr);
    this->systemCall(message, placeholder);
}

const int SystemTree::Node::getObjectId() const {
    return this->_object_id;
}

void SystemTree::Node::asString(string& placeholder) const {
    string str = "request " + to_string(this->_object_id);
    this->systemCall(str, placeholder);
}


// SystemTree
SystemTree::SystemTree(LogicSystemMaster* lsm): lsm(lsm) {
    this->root = new Node(this);
    this->current_node = this->root;
}

SystemTree::~SystemTree() {
    string msg;
    this->recursiveCleanMemory(this->root, msg);
    delete this->root;
}

void SystemTree::getWorkingDirectory(string& placeholder) {
    placeholder = "\n";
    Node* local_node = this->current_node;
    while (true) {
        placeholder = local_node->name() + placeholder;
        local_node = local_node->parent();
        if (local_node == nullptr)
            break;
        else
            placeholder = "/" + placeholder;
    }
}

void SystemTree::makeDirectory(const string& path, string& placeholder) {
    vector<string> paths;
    tokenizePath(path, paths);
    Node* nodeholder = nullptr;
    if (!this->createDirectory(paths, nodeholder, placeholder))
        placeholder = "Error: " + path + " is impossible because it passes by some file.\n";
}

bool SystemTree::createDirectory(const vector<string>& paths, Node*& nodeholder, string& placeholder) {
    int len_paths = paths.size();
    Node* local_node = this->current_node;
    Node* tmp_node = nullptr;
    for (int i=0; i<len_paths; i++) {
        if (local_node->isFile())
            return false;
        else {
            local_node->moveTo(paths[i], tmp_node);
            if (tmp_node == nullptr)
                tmp_node = local_node->addChild(paths[i], false, 0, placeholder);
            local_node = tmp_node;
        }
    }
    nodeholder = local_node;
    return true;
}

void SystemTree::list(const string& path, const bool recursive, string& placeholder) {
    vector<string> paths;
    tokenizePath(path, paths);
    SystemTree::Node* dest = nullptr;
    if (validPath(paths, this->current_node, dest)) {
        if (recursive) {
            vector<string> strs;
            recursiveList(dest, strs, 0);
            placeholder = "";
            for (int i=0; i<strs.size(); i++)
                placeholder += strs[i] + "\n";
        }
        else {
            int num_current_dirs = dest->children().size();
            for (int i=0; i<num_current_dirs; i++)
                placeholder = placeholder + dest->children()[i]->name() + "\n";
        }
    }
    else
        placeholder = "Error: directory " + path + " is not found.\n";
}

void SystemTree::changeDirectory(const string& path, string& placeholder) {
    vector<string> paths;
    tokenizePath(path, paths);
    Node* destholder = nullptr;
    if (this->validPath(paths, this->current_node, destholder)) {
        if (destholder->isFile())
            placeholder = "Error: " + path + " is a file.\n";
        else
            this->current_node = destholder;
    }
    else
        placeholder = "Error: directory " + path + " is not found.\n";
}

void SystemTree::move(const std::string& src, const std::string& dest, std::string& placeholder) {
    vector<string> src_paths, dest_paths;
    tokenizePath(src, src_paths);
    tokenizePath(dest, dest_paths);
    Node* src_node = nullptr;
    Node* dest_node = nullptr;
    if (this->validPath(src_paths, this->current_node, src_node)) {
        if (this->validPath(dest_paths, this->current_node, dest_node)) {
            if (dest_node->isFile())
                placeholder = "Error: destination " + dest + " is a file.\n";
            else {
                if (dest_node->contain(src_node->name()) >= 0)
                    placeholder = "Error: destination " + dest + " has a child of the same name.\n";
                else
                    src_node->reattachTo(dest_node, placeholder);
            }
        }
        else {
            vector<string> dest_dirpaths;
            this->dirPaths(dest_paths, dest_dirpaths);
            if(this->createDirectory(dest_dirpaths, dest_node, placeholder)) {
                src_node->reattachTo(dest_node, placeholder);
                src_node->setName(dest_paths.back(), placeholder);
            }
            else
                placeholder = "Error: " + dest + " is impossible because it passes by some file.\n";
        }
    }
    else
        placeholder = "Error: directory/file " + src + " is not found.\n";
}

void SystemTree::state(const string& path, string& placeholder) {
    vector<string> paths;
    tokenizePath(path, paths);
    Node* dest_node = nullptr;
    if (this->validPath(paths, this->current_node, dest_node))
        dest_node->asString(placeholder);
    else
        placeholder = "Error: directory/file " + path + " not found.\n";
}

void SystemTree::touch(const string& path, string& placeholder) {
    vector<string> paths;
    tokenizePath(path, paths);
    Node* dest_node = nullptr;
    if (this->validPath(paths, this->current_node, dest_node))
        dest_node->updateNothing(placeholder);
    else {
        vector<string> dirpaths;
        this->dirPaths(paths, dirpaths);
        if(this->createDirectory(dirpaths, dest_node, placeholder))
            dest_node->addChild(paths.back(), true, 0, placeholder);
        else
            placeholder = "Error: " + path + " is impossible because it passes by some file.\n";
    }
}

void SystemTree::remove(const std::string& path, const bool recursive, std::string& placeholder) {
    vector<string> paths;
    tokenizePath(path, paths);
    Node* node = nullptr;
    if (this->validPath(paths, this->current_node, node)) {
        if (node == this->root)
            placeholder = "Error: cannot delete the root.\n";
        else if (node->isFile() || recursive) {
            this->recursiveCleanMemory(node, placeholder);
            node->parent()->releaseChild(node, placeholder);
        }
        else
            placeholder = "Error: " + path + " is a directory. use rm -r DIRNAME to remove a directory.\n";
    }
    else
        placeholder = "Error: file " + path + " not found.\n";
}

bool SystemTree::validPath(const vector<string>& paths, Node* src, Node*& dest) {
    dest = src;
    int num_part = paths.size();
    for (int i=0; i<num_part; i++) {
        dest->moveTo(paths[i], dest);
        if (dest == nullptr)
            return false;
    }
    return true;
}

void SystemTree::recursiveList(const Node* const node, vector<string>& collected, int offset) {
    string indented_name = string((size_t)(offset * 2), ' ') + node->name();
    if (!node->isFile())
        indented_name += "/";
    collected.push_back(indented_name);
    int num_current_dirs = node->children().size();
    for (int i=0; i<num_current_dirs; i++)
        recursiveList(node->children()[i], collected, offset+1);
}

void SystemTree::recursiveCleanMemory(Node* node, string& placeholder) {
    while (node->children().size() != 0) {
        recursiveCleanMemory(node->children()[0], placeholder);
        node->releaseChild(0, placeholder);    
    }
}

void SystemTree::dirPaths(const vector<string>& paths, vector<string>& pathholder) {
    int len_path = paths.size()-1;
    for (int i=0; i<len_path; i++)
        pathholder.push_back(paths[i]);
}
