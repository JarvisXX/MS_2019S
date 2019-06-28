#include <iostream>
#include <cstring>
#include <vector>

#include "API.hpp"
#include "ServerJ.hpp"
#include "LogicSystemMaster.hpp"
#include "LogicSystemSlave.hpp"

using namespace std;

void flush(const string& msg) {
    cout << msg;
    if (msg.size()==0 || msg.back()!='\n')
        cout << endl;
    cout.flush();
}

Server::Server(string local_IP, int local_port, string master_IP, int master_port, int num_slaves):
               local_IP(local_IP), local_port(local_port), master_IP(master_IP), master_port(master_port), num_slaves(num_slaves) {
    this->is_master = (this->local_IP == this->master_IP) && (this->local_port == this->master_port);
    if (is_master)
        this->initializeMaster();
    else
        this->initializeSlave();
}

void Server::initializeMaster() {
    cout << "This is the master server." << endl;
    cout << "Waiting for " << this->num_slaves << " slaves..." << endl;
    this->server_con = new ServerConnection(this->local_port);
    for (int i=0; i<this->num_slaves; i++) {
        while (true) {
            this->server_con->acceptConnectionRequest();
            string message = to_string(i) + " " + this->server_con->getClientIpAddr();
            string line;
            vector<string> argv;
            if (!(this->server_con->readIn(line) && this->server_con->writeDown(message)))
                continue;
            else {
                vector<string> tokens;
                tokenize(line, tokens);
                if (tokens[0] != "port")
                    continue;
                else {
                    vector<string> slave_profile;
                    slave_profile.push_back(this->server_con->getClientIpAddr());
                    slave_profile.push_back(tokens[1]);
                    this->slave_table.push_back(slave_profile);
                    cout << "Slave No." + to_string(i) + " is at " + slave_profile[0] + " with port " + slave_profile[1] << endl;
                    break;
                }
            }
        }
    }
    for (int i=0; i<this->num_slaves; i++) {
        string ip = this->slave_table[i][0];
        int port = atoi(this->slave_table[i][1].c_str());
        this->connections.push_back(new ClientConnection(ip, port));
        string message = "hello";
        string line;
        while (true) {
            if (this->connections.back()->writeDown(message) && this->connections.back()->readIn(line))
                break;
            else {
                delete this->connections[i];
                this->connections[i] = new ClientConnection(ip, port);
            }
        }
        this->connecteds.push_back(true);
        this->replacements.push_back(false);
        cout << "Connection with " << ip << " " << port << " established." << endl;
    }
    cout << "Ready!" << endl;
}

void Server::initializeSlave() {
    cout << "This is a slave server." << endl;
    cout << "My master server is at " + this->master_IP + " with port " + to_string(this->master_port) << endl;
    ClientConnection con(this->master_IP, this->master_port);
    string message("port " + to_string(this->local_port));
    while (true) {
        string line;
        if (con.writeDown(message) && con.readIn(line)) {
            vector<string> tokens;
            tokenize(line, tokens);
            if (tokens[0] == "Error:") {
                printf("No need for replacement.");
                exit(1);
            } 
            this->slave_id = atoi(tokens[0].c_str());
            this->local_IP = tokens[1];
            cout << "My IP address is " + this->local_IP << endl;
            cout << "Slave No." + to_string(this->slave_id) + " at your service." << endl;
            break;
        }
        else
            con = ClientConnection(this->master_IP, this->master_port);
    }
    this->server_con = new ServerConnection(this->local_port);
    cout << "Ready!" << endl;
}

void Server::run() {
    if (this->is_master) {
        System* mds = new LogicSystemMaster(this);
        this->establishService(mds);
    }
    else {
        System* mdt = new LogicSystemSlave(this);
        this->establishService(mdt);
    }
}

void Server::establishService(System* sys) {
    this->server_con->acceptConnectionRequest();
    while (true){
        string line;
        vector<string> argv;
        string message;
        if (!this->server_con->readIn(line))
            continue;
        flush("Input: "+line);
        tokenize(line, argv);
        sys->runCommandLine(argv, message);
        if (message.size() == 0)
            message = "<no output>\n";
        if (!this->server_con->writeDown(message))
            continue;
        flush("Output: "+message);
    }
}

void Server::slaveHash(int identifier, vector<int>& slaveholder) {
    if (this->num_slaves == 2) {
        slaveholder.push_back(0);
        slaveholder.push_back(1);
    }
    else {
        int residu = identifier % this->num_slaves;
        for (int i=0; i<this->num_slaves; i++) {
            if (residu != i)
            slaveholder.push_back(i);
        }
    }
}

void Server::sendTo(const string& IP, int port, const string& message, string& placeholder) {
    ClientConnection con(IP, port);
    string line;
    if (!con.writeDown(message) || !con.readIn(placeholder))
        placeholder = "Failure";
}

void Server::sendTo(const vector<int>& slaveids, const string& message, string& feedback) {
    vector<string> responses;
    for (int i=0; i<slaveids.size(); i++) {
        int sid = slaveids[i];
        string slstr = this->slaveStr(sid);
        string line;
        flush("out to " + slstr + ": " + message);
        if (!this->connecteds[sid] && !this->replacements[sid]) {
            responses.push_back("");
            feedback += to_string(sid) + "-th slave " + slstr + " is still dead\n";
        }
        else if (this->replacements[sid]) {
            responses.push_back("");
            delete this->connections[sid];
            this->connectTo(sid);
            if (!this->recover(sid)) {
                feedback += to_string(sid) + "-th slave " + slstr + " recovery failed\n";
                flush("Successful connection but failed recovery to " + slstr + ".");
            }
            else {
                this->connecteds[sid] = true;
                this->replacements[sid] = false;
                feedback += to_string(sid) + "-th slave " + slstr + " finds a replacement\n";
                flush("Successful connection and recovery to " + slstr + ".");
            }
        }
        else if (this->connections[sid]->writeDown(message) && this->connections[sid]->readIn(line)) {
            responses.push_back(line);
            feedback += to_string(sid) + "-th slave " + slstr + " responds: " + line + "\n";
            flush("in from " + slstr + ": " + line);
        }
        else {
            responses.push_back("");
            this->connecteds[sid] = false;
            flush("Error: connection to " + slstr + " is lost.");
            feedback += to_string(sid) + "-th slave " + slstr + " has lost the contact.\n";
        }
    }
    string formal_output;
    for (int i=0; i<slaveids.size(); i++) {
        if (responses[i] == "")
            continue;
        else {
            formal_output = responses[i];
            vector<string> tokens;
            tokenize(responses[i], tokens);
            if (tokens[0] != "Error:")
                break;
        }
    }
    feedback += "formal output: " + formal_output + "\n";
}

string Server::slaveStr(int id) {
    return "[" + to_string(id) + ", " + this->slave_table[id][0] + ", " + this->slave_table[id][1] + "]";
}

void Server::connectTo(int sid) {
    string ip = this->slave_table[sid][0];
    int port = atoi(this->slave_table[sid][1].c_str());
    this->connections[sid] = new ClientConnection(ip, port);
}

bool Server::recover(int id) {
    int cnt = 0;
    for (int i=1; i<this->num_slaves; i++) {
        int sid = (id+1) % this->num_slaves;
        if (this->connecteds[sid] && this->synchronizeData(sid, id)) {
            cnt++;
            if (cnt >= 2)
                break;
        }
    }
    return (cnt >= 2) || ((cnt == 1) && this->num_slaves == 2);
}

bool Server::synchronizeData(int src, int dst) {
    string message = "pushto " + this->slave_table[dst][0] + " " + this->slave_table[dst][1];
    vector<int> slaveidarr; slaveidarr.push_back(src);
    string line;
    delete this->connections[dst];
    this->sendTo(slaveidarr, message, line);
    this->connectTo(dst);
    vector<string> lines;
    tokenize(line, lines, "\n");
    vector<string> tokens;
    tokenize(lines[1], tokens);
    return tokens.size() == 3 && tokens[2] == "Success";
}
