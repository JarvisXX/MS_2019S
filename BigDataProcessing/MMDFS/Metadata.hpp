#ifndef METADATA
#define METADATA

using namespace std;

class Metadata {
public:
    Metadata(int id, int parent, string creation_time, size_t size, string lastmodify_time, string name, int type):
             id(id), parent(parent), creation_time(creation_time), lastmodify_time(lastmodify_time), size(size), name(name), type(type){}

    void updateParent(int parent) {
        this->parent = parent;
    }

    void updateLastmodifyTime(string lastmodify_time) {
        this->lastmodify_time = lastmodify_time;
    }

    void updateName(string name) {
        this->name = name;
    }

    string summary() {
        return to_string(id) + " " + to_string(parent) + " " + creation_time + " " + name + " " + to_string(type);
    }

private:
    const int id;
    int parent;
    const string creation_time;
    string lastmodify_time;
    size_t size;
    string name;
    const int type;
};

#endif
