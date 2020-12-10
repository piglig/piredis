#include "connection.h"


bool RedisConnection::Connect() {
    if (!Init()) {
        return false;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cout << "connect failed" << endl;
        return false;
    }
    return true;
}

bool RedisConnection::Init() {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "socket create error" << endl;
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        cout << "invalid address / address not supported" << endl;
        return false;
    }

    return true;
}

bool RedisConnection::Close() {
    int res = close(sock);
    if (res == 0) {
        return true;
    }
    cout << "close failed[" << errno << "] " << endl;
    return false;
}

RESPReply RedisConnection::SendCommand(const std::string& command) {
    RESPReply reply;
    if (send(sock, (char *)command.c_str(), command.size(), 0) < 0) {
        reply.errorCode = -1;
        reply.innerError = enumSocketSendMsgFailed;
        cout << "send failed[" << errno << "] " << endl;
        return reply;
    }

    return ReceiveResp();
}

RESPReply RedisConnection::ReceiveResp() {
    RESPReply res;
    string reply(1024, 0);
    if (read(sock, &reply[0], 1024 - 1) < 0) {
        cout << "failed read data from socket" << endl;
        return res;
    }
    
    return RESPUtils::ConvertToRESPReply(reply);
    // cout << RESPUtils::ConvertToSimpleString(reply) << endl;
}

int main(void) 
{
    RedisConnection connection("10.10.243.233");
    if (connection.Connect()) {
        cout << "Connected to Redis success" << endl;
    }

    RESPReply reply = connection.SendCommand("auth zshshy0192837465443\r\n");
    cout << reply.type << " " << reply.str << endl;
    reply = connection.SendCommand("ping\r\n");
    cout << reply.type << " " << reply.str << endl;

    reply = connection.SendCommand("ping1\r\n");
    cout << reply.type << " " << reply.str << endl;

    reply = connection.SendCommand("DBSIZE\r\n");
    cout << reply.type << " " << reply.integerResp << endl;

    reply = connection.SendCommand("client list\r\n");
    // cout << reply.type << " " << reply.integerResp << endl;

    return 0;
}