#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/resp_utils.h"

using std::cout;
using std::endl;
using std::string;

typedef struct sockaddr_in SocketAddr;



class RedisConnection {
public:
    RedisConnection(string _host, unsigned int _port = 6379) : host(_host), port(_port) {
    }

    ~RedisConnection() {

    }

    bool Connect();

    bool Close();

    void SendCommand(const std::string& command);
    RESPReply ReceiveResp();

private:
    bool Init();

private:
    string host;
    unsigned int port;
    // redis descriptor 
    int sock{0};
    SocketAddr server_addr;
};


#endif