#include <iostream>
#include <cstring>

#include "ServerJ.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    string localIP;
    string masterIP;
    int localport, masterport;
    int numslaves = -1;
    if (argc == 3) {
        localport = masterport = atoi(argv[1]);
        numslaves = atoi(argv[2]);
    }
    else if (argc == 4) {
        localport = atoi(argv[1]);
        masterIP = string(argv[2]);
        masterport = atoi(argv[3]);
    }
    else {
        string usage_msg = ""
        "Usage:\n"
        "\tfor master server: ./server PORT NUMSLAVES\n"
        "\t\tPORT     : port used to communicate with other servers\n"
        "\t\tNUMSLAVES: the number of slave server to be used\n"
        "\tfor  slave server: ./server PORT MASTERADDR MASTERPORT\n"
        "\t\tPORT      : used to communicate with the master server\n"
        "\t\tMASTERADDR: IP address of master server\n"
        "\t\tMASTERPORT: port to access master server\n";
        cout << usage_msg << endl;
        exit(0);
    }
    Server server(localIP, localport, masterIP, masterport, numslaves);
    server.run();
    return 0;
}
