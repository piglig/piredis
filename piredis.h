#ifndef PI_REDIS_H
#define PI_REDIS_H

#include <iostream>
#include <string>
#include <hiredis.h>
#include "utils/redis_utils.h"
#include "node.h"

using std::cout;
using std::endl;

struct PiRedisReply {
    std::string replyString;
    //  0 reply success
    // -1 reply null
    // -7 cluster node connect error
    // -8 cluster inappropriate node
    int errorCode;
};

// enum ReplyCode {
//     Success,
//     ReplyNull
// };

class PiRedis {
public:
    // timeout 微秒
    PiRedis(const std::string& host, const int& port, int timeout = 1000000) : m_strHost(host), m_iPort(port), m_iTimeout(timeout) {};

    bool init() {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = m_iTimeout;
        piRedisContext = redisConnectWithTimeout(m_strHost.c_str(), m_iPort, tv);
        if (piRedisContext == nullptr || piRedisContext->err) {
            if (piRedisContext) {
                cout << "Error:" << piRedisContext->errstr << endl;
            } else {
                cout << "Can't allocate redis context" << endl;
            }
            return false;
        }
        return true;
    }

    bool setClusterMode() {
        PiRedisReply res = sendCommandDirectly("CLUSTER NODES");
        if (res.errorCode != 0) {
            return false;
        }
    
        std::string delimiter = "\n";
        size_t pos = 0;
        std::vector<std::string> vs;
        vs = MyUtils::SplitString(res.replyString, delimiter);
        
        for_each(vs.begin(), vs.end(), [&](const auto& str) {
            PiRedisNodeStruct temp;
            temp = RedisUtils::nodeStringSplit(str);
            // cluster info 中节点 ip 为 127.0.0.1
            // 防止用 (127.0.0.1) 远程连接 redis cluster 
            temp.m_strIp = m_strHost;
            m_vPiRedisNodes.push_back(temp);
        });
        return true;
    }

    PiRedisReply sendCommandDirectly(const std::string& command) {
        PiRedisReply res;

        redisReply *reply = (redisReply *)redisCommand(piRedisContext, command.c_str());
        res.replyString = reply->str;
        if (reply == nullptr) {
            cout << "reply is null[" << piRedisContext->errstr << "]" << endl;
            res.errorCode = -1;
            freeReplyObject(reply);
            return res;
        }

        res.errorCode = 0;
        freeReplyObject(reply);
        return res;
    }

    PiRedisReply getFromCluster(const std::string& key) {
        PiRedisReply res;

        unsigned short slot = MyUtils::GetSlotValue(key);
        PiRedisNodeStruct* clusterNode = RedisUtils::getRightClusterNode(key, m_vPiRedisNodes);
        if (clusterNode == nullptr) {
            res.errorCode = -8;
            return res;
        }
        cout << clusterNode->m_strIp << ":" << clusterNode->m_iPort << endl;
        if (!connectPiRedisClusterNode(clusterNode->m_strIp, clusterNode->m_iPort)) {
            res.errorCode = -7;
            return res;
        }

        return sendCommandDirectly("get " + key);
    }

    bool connectPiRedisClusterNode(const std::string& ip, int port) {
        m_strHost = ip.c_str();
        m_iPort = port;

        return init();
    }
    
    void freeRedis() {
        redisFree(piRedisContext);
    }

private:
    std::vector<PiRedisNodeStruct> m_vPiRedisNodes;
    std::string m_strHost;
    int m_iPort;
    int m_iTimeout;
    redisContext *piRedisContext;
};

#endif