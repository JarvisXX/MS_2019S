#ifndef CONNECTION
#define CONNECTION

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

const int connection_bandwidth = 512;

class ServerConnection {
public:
    ServerConnection(int portno);
    ~ServerConnection();
    int acceptConnectionRequest();
    bool readIn(string& line);
    bool writeDown(const string& message);
    string getClientIpAddr();

private:
    int sockfd;
    int portno;
    int newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    struct sockaddr * client_addr;
    string client_ip_address_str;
};

class ClientConnection {
public:
    ClientConnection(const string& hostname, int portno);
    ~ClientConnection();
    bool readIn(string& line);
    bool writeDown(const string& message);
    int getSockFD();

private:
    int sockfd;
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
};

#endif
