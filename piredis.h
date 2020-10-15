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
    std::vector<std::string> replyElements;
    //  0 reply success
    // -1 reply null
    // -6 command parameter invalid
    // -7 cluster node connect error
    // -8 cluster inappropriate node
    // -9 list operate option invalid
    int errorCode;
    std::string errorStr;
};

enum ListOptions {
    RANK,
    COUNT,
    MAXLEN
};

struct ListOptionalArgs {
    ListOptions option;
    int optionValue;
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
        cout << "command:" << command << " " << reply->type << endl;
        if (reply->type == REDIS_REPLY_ERROR) {
            std::string temp{REDIS_REPLY_STATUS};
            res.errorStr = "reply is error[" + temp + "]";
            res.errorCode = -1;
            freeReplyObject(reply);
            return res;
        } else if (reply->type == REDIS_REPLY_INTEGER) {
            res.replyString = std::to_string(reply->integer);
        } else if (reply->type == REDIS_REPLY_NIL) {
            res.errorCode = -1;
            res.errorStr = "reply is error[empty]";
            freeReplyObject(reply);
            return res;
        } else if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; ++i) {
                // 返回 integer 数组
                if (reply->element[i]->str == nullptr) {
                    res.replyElements.push_back(std::to_string(reply->element[i]->integer));
                // 返回 字符串 数组
                } else {
                    res.replyElements.push_back(reply->element[i]->str);
                }
                
            }
            res.replyString = "REDIS_REPLY_ARRAY";
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

    PiRedisReply lpop(const std::string& key) {
        return sendCommandDirectly("lpop " + key);
    }

    PiRedisReply lpopToCluster(const std::string& key) {
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

        return lpop(key);
    }

    PiRedisReply rpush(const std::string& key, const std::vector<std::string>& value) {
        PiRedisReply res;

        std::string command = "rpush " + key;
        if (value.size() == 0) {
            res.errorCode = -6;
            res.errorStr = "rpush value is empty";
            return res;
        }

        for (const auto& v : value) {
            command += " " + v;
        }
        return sendCommandDirectly(command);
    }

    PiRedisReply rpushToCluster(const std::string& key, const std::vector<std::string>& value) {
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

        return rpush(key, value);
    }

    PiRedisReply rpop(const std::string& key) {
        return sendCommandDirectly("rpop " + key);
    }

    PiRedisReply rpopToCluster(const std::string& key) {
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

        return rpop(key);
    }

    PiRedisReply llen(const std::string& key) {
        return sendCommandDirectly("llen " + key);
    }

    PiRedisReply llenToCluster(const std::string& key) {
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

        return llen(key);
    }

    PiRedisReply lindex(const std::string& key, int index) {
        return sendCommandDirectly("lindex " + key + " " + std::to_string(index));
    }

    PiRedisReply lindexToCluster(const std::string& key, int index) {
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

        return lindex(key, index);
    }

    PiRedisReply lpushx(const std::string& key, const std::vector<std::string>& value) {
        PiRedisReply res;

        std::string command = "lpushx " + key;
        if (value.size() == 0) {
            res.errorCode = -6;
            res.errorStr = "lpushx value is empty";
            return res;
        }

        for (const auto& v : value) {
            command += " " + v;
        }
        return sendCommandDirectly(command);
    }

    PiRedisReply lpushxToCluster(const std::string& key, const std::vector<std::string>& value) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return lpushx(key, value);
        }
        
        return reply;
    }

    PiRedisReply rpushx(const std::string& key, const std::vector<std::string>& value) {
        PiRedisReply res;

        std::string command = "rpushx " + key;
        if (value.size() == 0) {
            res.errorCode = -6;
            res.errorStr = "rpushx value is empty";
            return res;
        }

        for (const auto& v : value) {
            command += " " + v;
        }
        return sendCommandDirectly(command);
    }

    PiRedisReply rpushxToCluster(const std::string& key, const std::vector<std::string>& value) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return rpushx(key, value);
        }
        
        return reply;
    }

    PiRedisReply lrange(const std::string& key, int start, int end) {
        return sendCommandDirectly("lrange " + key + " " + std::to_string(start) + " " + std::to_string(end));
    }

    PiRedisReply lrangeToCluster(const std::string& key, int start, int end) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return lrange(key, start, end);
        }
        
        return reply;
    }

    PiRedisReply lset(const std::string& key, int index, const std::string& value) {
        return sendCommandDirectly("lset " + key + " " + std::to_string(index) + " " + value);
    }

    PiRedisReply lsetToCluster(const std::string& key, int index, const std::string& value) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return lset(key, index, value);
        }
        
        return reply;
    }

    PiRedisReply lpos(const std::string& key, const std::string& value) {
        return sendCommandDirectly("lpos " + key + " " + value);
    }

    PiRedisReply lpos(const std::string& key, const std::string& value, ListOptionalArgs args) {
        std::string command = "lpos ";
        if (args.option == ListOptions::RANK) {
            command += key + " " + value + " RANK " + std::to_string(args.optionValue);
        } else if (args.option == ListOptions::COUNT) {
            command += key + " " + value + " COUNT " + std::to_string(args.optionValue);
        }
        return sendCommandDirectly(command);
    }

    // 其实 args 数组长度最多为 3
    PiRedisReply lpos(const std::string& key, const std::string& value, std::vector<ListOptionalArgs> args) {
        PiRedisReply res;
        std::string command = "lpos " + key + " " + value;
        ListOptionalArgs temp = args[0];

        if (temp.option == args[args.size() - 1].option) {
            res.errorCode = -9;
            res.errorStr = "lpos option1[" + std::to_string(temp.option) + "] same as option2[" + std::to_string(args[args.size() - 1].option) + "]";
            return res;
        }

        if (temp.option == ListOptions::RANK) {
            command += " RANK " + std::to_string(temp.optionValue);
        } else if (temp.option == ListOptions::COUNT) {
            command += " COUNT " + std::to_string(temp.optionValue);
        } else if (temp.option == ListOptions::MAXLEN) {
            command += " MAXLEN " + std::to_string(temp.optionValue);
        }

        for (size_t i = 1; i < args.size(); ++i) {
            if (temp.option == args[i].option) {    
                res.errorCode = -9;
                res.errorStr = "lpos option1[" + std::to_string(temp.option) + "] same as option2[" + std::to_string(args[i].option) + "]";
                return res;
            }

            if (args[i].option == ListOptions::RANK) {
                command += " RANK " + std::to_string(args[i].optionValue);
            } else if (args[i].option == ListOptions::COUNT) {
                command += " COUNT " + std::to_string(args[i].optionValue);
            } else if (args[i].option == ListOptions::MAXLEN) {
                command += " MAXLEN " + std::to_string(args[i].optionValue);
            }
            
            temp = args[i];
        }
        cout << command << endl;
        return sendCommandDirectly(command);
    }

    PiRedisReply lposToCluster(const std::string& key, const std::string& value) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return lpos(key, value);
        }
        
        return reply;
    }

    PiRedisReply lposToCluster(const std::string& key, const std::string& value, ListOptionalArgs args) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return lpos(key, value, args);
        }
        
        return reply;
    }

    PiRedisReply lposToCluster(const std::string& key, const std::string& value, std::vector<ListOptionalArgs> args) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return lpos(key, value, args);
        }
        
        return reply;
    }

    PiRedisReply ltrim(const std::string& key, int start, int end) {
        return sendCommandDirectly("ltrim " + key + " " + std::to_string(start) + " " + std::to_string(end));
    }

    PiRedisReply ltrimToCluster(const std::string& key, int start, int end) {
        PiRedisReply reply = searchTargetClusterNode(key, m_vPiRedisNodes);
        if (reply.errorCode == REDIS_OK) {
            return ltrim(key, start, end);
        }
        
        return reply;
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

    PiRedisReply searchTargetClusterNode(const std::string& key, std::vector<PiRedisNodeStruct> clusterNodes) {
        PiRedisReply res;

        PiRedisNodeStruct* clusterNode = RedisUtils::getRightClusterNode(key, m_vPiRedisNodes);
        if (clusterNode == nullptr) {
            cout << "clusterNode is null" << endl;
            res.errorCode = -8;
            return res;
        }
        cout << clusterNode->m_strIp << ":" << clusterNode->m_iPort << endl;
        
        if (!connectPiRedisClusterNode(clusterNode->m_strIp, clusterNode->m_iPort)) {
            cout << "clusterNode cannot find \"" << key << "\"" << endl;
            res.errorCode = -7;
            return res;
        }
        res.errorCode = 0;
        return res;
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