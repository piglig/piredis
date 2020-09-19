#ifndef PI_REDIS_H
#define PI_REDIS_H

#include <iostream>
#include <string>
#include <hiredis.h>

using std::cout;
using std::endl;

class PiRedis {
public:
    // timeout 微秒
    PiRedis(const std::string& host, const int& port, int timeout = 10000) : m_host(host), m_port(port), m_timeout(timeout) {};

    bool init() {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = m_timeout;
        piRedisContext = redisConnectWithTimeout(host.c_str(), port, tv);
        if (piRedisContext == nullptr || piRedisContext->err) {
            if (piRedisContext) {
                cout << "Error:" << c->errstr << endl;
            } else {
                cout << "Can't allocate redis context" << endl;
            }
        }
    }

    std::string sendCommandDirectly(const std::string& command) {
        redisReply *reply = (redisReply *)redisCommand(piRedisContext, command.c_str());
        if (reply == nullptr) {
            cout << "reply is null[" << c->errstr << "]" << endl;
            freeReplyObject(reply);
        }

        std::string res{reply->str};
        freeReplyObject(reply);
        return res;
    }

    void freeRedis() {
        redisFree(piRedisContext);
    }

private:
    std::string m_host;
    int m_port;
    int m_timeout;
    redisContext *piRedisContext;
};

#endif