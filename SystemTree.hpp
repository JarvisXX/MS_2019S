#ifndef SYSTEMTREE
#define SYSTEMTREE

#include <cstring>
#include <vector>

#include "LogicSystemMaster.hpp"

using namespace std;

class LogicSystemMaster;

class SystemTree {
public:
    SystemTree(LogicSystemMaster* lsm);
    ~SystemTree();
    void getWorkingDirectory(string& placeholder);
    void makeDirectory(const string& path, string& placeholder);
    void list(const string& path, const bool recursive, string& placeholder);
    void changeDirectory(const string& path, string& placeholder);
    void move(const string& src, const string& dest, string& placeholder);
    void touch(const string& path, string& placeholder);
    void state(const string& path, string& placeholder);
    void remove(const string& path, const bool recursive, string& placeholder);

private:
    class Node {
    public:
        Node(SystemTree* st);
        Node(SystemTree* st, Node* parent, const string name, const bool is_file, const int size, string& placeholder);

        Node* const& addChild(const string& name, const bool is_file, const int size, string& placeholder);
        void reattachTo(Node* new_parent, string& placeholder);
        void setName(const string& name, string& placeholder);
        void updateNothing(string& placeholder);
        void releaseChild(int i, string& placeholder);
        void releaseChild(Node* ch, string& placeholder);
        void moveTo(const string& name, Node*& dest);
        int contain(const string& name) const;
        const string& name() const;
        Node* const& parent() const;
        const vector<Node*>& children() const;
        const int getObjectId() const;
        bool isFile() const;
        void asString(string& placeholder) const;

    private:
        static int _node_count;
        SystemTree* const st;
        const int _object_id;
        string _name;
        const bool _is_file;
        Node* _parent;
        vector<Node*> _children;

        Node(SystemTree* st, bool is_file);
        void detach();
        void attachTo(Node* new_parent);
        void update();
        string creationMessage() const;
        string updateMessage(const string& key, const string& value) const;
        void systemCall(const string& message, string& placeholder) const;
    };

    LogicSystemMaster* lsm;
    Node* root;
    Node* current_node;
    bool validPath(const vector<string>& paths, Node* src, Node*& dest);
    void recursiveCleanMemory(Node* node, string& placeholder);
    void recursiveList(const Node* const node, vector<string>& collected, int offset);
    void dirPaths(const vector<string>& paths, vector<string>& pathholder);
    bool createDirectory(const vector<string>& paths, Node*& nodeholder, string& placeholder);
};

#endif
