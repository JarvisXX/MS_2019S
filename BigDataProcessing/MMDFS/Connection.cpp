#include <iostream>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>

#include "Connection.hpp"

using namespace std;

int isValidFD(int fd) {
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

void *getInAddr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

string getIpAddrStr(int ai_family, struct sockaddr *sa) {
    char s[INET6_ADDRSTRLEN];
    inet_ntop(ai_family, getInAddr(sa), s, sizeof s);
    return string(s);
}


// Server Connection
ServerConnection::ServerConnection(int portno) {
    signal(SIGPIPE, SIG_IGN);
    while (true) {
        this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->sockfd < 0)
            continue;
        bzero((char *) &(this->serv_addr), sizeof(this->serv_addr));
        this->portno = portno;
        this->serv_addr.sin_family = AF_INET;
        this->serv_addr.sin_addr.s_addr = INADDR_ANY;
        this->serv_addr.sin_port = htons(portno);
        if (bind(this->sockfd, (struct sockaddr *) &(this->serv_addr), sizeof(this->serv_addr)) < 0)
            continue;
        listen(this->sockfd, 5);
        this->clilen = sizeof(this->cli_addr);
        this->client_addr = (struct sockaddr *) &(this->cli_addr);
        break;
    }
    cout << "Service on port " << portno << " established." << endl;
}

int ServerConnection::acceptConnectionRequest() {
    this->newsockfd = accept(this->sockfd, this->client_addr, &(this->clilen));
    this->client_ip_address_str = getIpAddrStr(AF_INET, this->client_addr);
    if (this->newsockfd < 0) {
        printf("ERROR on accept");
        exit(1);
    }
    return this->newsockfd;
}

bool ServerConnection::readIn(string& line) {
    char buffer[connection_bandwidth];
    bzero(buffer,connection_bandwidth);
    int n = read(this->newsockfd,buffer,connection_bandwidth-1);
    if (n <= 0) {
        cout << "socket connection stopped." << endl;
        this->acceptConnectionRequest();
        return false;
    }
    line = string(buffer);
    return true;
}

bool ServerConnection::writeDown(const string& message) {
    if (!isValidFD(this->sockfd)) {
        cout << "Error: invalid file descriptor." << endl;
        return false;
    }
    int n = write(this->newsockfd, message.c_str(), message.size());
    if (n <= 0) {
        cout << "Error: socket connection stopped." << endl;
        this->acceptConnectionRequest();
        return false;
    }
    return true;
}

string ServerConnection::getClientIpAddr() {
    return this->client_ip_address_str;
}

ServerConnection::~ServerConnection() {
    close(this->newsockfd);
    close(this->sockfd);
}


// Client Connection
ClientConnection::ClientConnection(const string& hostname, int portno) {
    signal(SIGPIPE, SIG_IGN);
    this->portno = portno;
    this->server = gethostbyname(hostname.c_str());
    if (this->server == nullptr) {
        cout << "Error: fails to get the hostname." << endl;
        return;
    }
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sockfd < 0) {
        cout << "Error: fails to open the socket." << endl;
        return;
    }
    bzero((char *) &this->serv_addr, sizeof(this->serv_addr));
    this->serv_addr.sin_family = AF_INET;
    bcopy((char *)this->server->h_addr,
        (char *)&this->serv_addr.sin_addr.s_addr,
        this->server->h_length);
    this->serv_addr.sin_port = htons(this->portno);
    if (connect(this->sockfd,(struct sockaddr *) &(this->serv_addr),sizeof(this->serv_addr)) < 0) {
        // cout << "Error: connection fails." << endl;
        return;
    }
}

bool ClientConnection::readIn(string& line) {
    char buffer[connection_bandwidth];
    bzero(buffer,connection_bandwidth);
    int n = read(this->sockfd,buffer,connection_bandwidth-1);
    if (n < 0) {
        cout << "Error: read operation non succeeded." << endl;
        return false;
    }
    line = string(buffer);
    return true;
}

bool ClientConnection::writeDown(const string& message) {
    if (!isValidFD(this->sockfd)) {
        cout << "Error: invalid file descriptor." << endl;
        return false;
    }
    int n = write(this->sockfd, message.c_str(), message.size());
    if (n < 0) {
        // cout << "Error: message writing non succeeded." << endl;
        return false;
    }
    return true;
}

int ClientConnection::getSockFD() {
    return this->sockfd;
}

ClientConnection::~ClientConnection() {
    close(this->sockfd);
}
