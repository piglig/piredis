#ifndef PI_REDIS_H
#define PI_REDIS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <hiredis.h>
#include "utils/redis_utils.h"
#include "node.h"

using std::cout;
using std::endl;

struct PiRedisReply {
    std::string replyString;
    //  0 reply success
    // -1 reply null
    // -6 command parameter invalid
    // -7 cluster node connect error
    // -8 cluster inappropriate node
    int errorCode;
    std::string errorStr;
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
        // freeRedis();
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
        
        if (reply == nullptr || reply->type == REDIS_REPLY_ERROR) {
            cout << "reply is error[" << reply->str << "]" << endl;
            res.errorCode = -1;
            freeReplyObject(reply);
            return res;
        } else if (reply->type == REDIS_REPLY_INTEGER) {
            res.replyString = std::to_string(reply->integer);
        } else {
            res.replyString = reply->str;
        }

        res.errorCode = 0;
        freeReplyObject(reply);
        return res;
    }

    PiRedisReply getFromCluster(const std::string& key) {
        PiRedisReply res;

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

    PiRedisReply get(const std::string& key) {
        return sendCommandDirectly("get " + key);
    }

    PiRedisReply set(const std::string& key, const std::string& value) {
        return sendCommandDirectly("set " + key + " " + value);
    }

    PiRedisReply setToCluster(const std::string& key, const std::string& value) {
        PiRedisReply res;

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

        return set(key, value);
    }

    PiRedisReply setex(const std::string& key, int seconds, const std::string& value) {
        return sendCommandDirectly("setex " + key + " " + std::to_string(seconds) + " " + value);
    }

    PiRedisReply setexToCluster(const std::string& key, int seconds, const std::string& value) {
        PiRedisReply res;

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

        return setex(key, seconds, value);
    }

    PiRedisReply incr(const std::string& key) {
        return sendCommandDirectly("incrby " + key + " 1");
    }

    PiRedisReply incrToCluster(const std::string& key) {
        return incrbyToCluster(key, 1);
    }

    PiRedisReply incrby(const std::string& key, int increment) {
        return sendCommandDirectly("incrby " + key + " " + std::to_string(increment));
    }

    PiRedisReply incrbyToCluster(const std::string& key, int increment) {
        PiRedisReply res;

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

        return incrby(key, increment);
    }

    PiRedisReply decr(const std::string& key) {
        return sendCommandDirectly("decrby " + key + " 1");
    }

    PiRedisReply decrToCluster(const std::string& key) {
        return decrbyToCluster(key, 1);
    }

    PiRedisReply decrby(const std::string& key, int decrement) {
        return sendCommandDirectly("decrby " + key + " " + std::to_string(decrement));
    }

    PiRedisReply decrbyToCluster(const std::string& key, int decrement) {
        PiRedisReply res;

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

        return decrby(key, decrement);
    }

    PiRedisReply append(const std::string& key, const std::string& value) {
        return sendCommandDirectly("append " + key + " " + value);
    }

    PiRedisReply appendToCluster(const std::string& key, const std::string& value) {
        PiRedisReply res;

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

        return append(key, value);
    }

    PiRedisReply getset(const std::string& key, const std::string& value) {
        return sendCommandDirectly("getset " + key + " " + value);
    }

    PiRedisReply getsetToCluster(const std::string& key, const std::string& value) {
        PiRedisReply res;

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

        return getset(key, value);
    }

    PiRedisReply lpush(const std::string& key, const std::vector<std::string>& value) {
        PiRedisReply res;

        std::string command = "lpush " + key;
        if (value.size() == 0) {
            res.errorCode = -6;
            res.errorStr = "lpush value is empty";
            return res;
        }

        for (const auto& v : value) {
            command += " " + v;
        }
        return sendCommandDirectly(command);
    }

    PiRedisReply lpushToCluster(const std::string& key, const std::vector<std::string>& value) {
        PiRedisReply res;

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

        return lpush(key, value);
    }





    // PiRedisReply mset(const std::map<std::string, std::string>& entries) {
    //     std::string command = "mset";
    //     for (const auto& entry : entries) {
    //         command += " " + entry.first + " " + entry.second;
    //     }
        
    //     return sendCommandDirectly(command);
    // }

    // PiRedisReply msetToCluster(const std::map<std::string, std::string>& entries) {    
    //     PiRedisReply res;

    //     for (const auto& entry : entries) {
    //         unsigned short slot = MyUtils::GetSlotValue(entry.first);
    //         PiRedisNodeStruct* clusterNode = RedisUtils::getRightClusterNode(entry.first, m_vPiRedisNodes);
    //         if (clusterNode == nullptr) {
    //             res.errorCode = -8;
    //             return res;
    //         }
    //         cout << clusterNode->m_strIp << ":" << clusterNode->m_iPort << endl;
    //         if (!connectPiRedisClusterNode(clusterNode->m_strIp, clusterNode->m_iPort)) {
    //             res.errorCode = -7;
    //             return res;
    //         }
    //     }

    //     return mset(entries);
    // }

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