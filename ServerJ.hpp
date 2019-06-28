#ifndef SERVER
#define SERVER

#include <cstring>
#include <vector>

#include "Connection.hpp"
#include "System.hpp"

using namespace std;

class Server {
public:
    Server(string localIP, int localport, string masterIP, int masterport, int numslaves);
    void run();
    void slaveHash(int identifier, vector<int>& slaveholder);
    void sendTo(const string& IP, int port, const string& message, string& placeholder);
    void sendTo(const vector<int>& slaveids, const string& message, string& feedback);

private:
    ServerConnection* server_con;
    string local_IP;
    const int local_port;
    const string master_IP;
    const int master_port;
    const int num_slaves;
    bool is_master;
    int slave_id;
    vector<vector<string> > slave_table;
    vector<ClientConnection*> connections;
    vector<bool> connecteds;
    vector<bool> replacements;

    void initializeMaster();
    void initializeSlave();
    void establishService(System* system);
    string slaveStr(int id);
    void connectTo(int sid);
    bool recover(int id);
    bool synchronizeData(int src, int dst);
};

#endif
