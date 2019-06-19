#include <iostream>
#include <cstring>

#include "API.hpp"
#include "Connection.hpp"

#define N 512

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Error usage of HOSTNAME and PORT");
        exit(1);
    }
    string hostname = string(argv[1]);
    int portno = atoi(argv[2]);
    ClientConnection con(hostname, portno);
    int sockfd = con.getSockFD();
    char buffer[N];
    while (true) {
        printf("$ ");
        bzero(buffer, N);
        fgets(buffer, N-1, stdin);
        string line = string(buffer);
        vector<string> command;
        tokenize(line, command);
        if (command.size() == 0)
            continue;
        else {
            if (command[0] == "exit" or command[0] == "quit")
                break;
            else {
                string message(buffer);
                if (!con.writeDown(message)) {
                    con = ClientConnection(hostname, portno);
                    continue;
                }
                string line;
                if (!con.readIn(line)) {
                    con = ClientConnection(hostname, portno);
                    continue;
                }
                if (line != "<no output>\n")
                    cout << line;
            }
        }
    }
    return 0;
}
