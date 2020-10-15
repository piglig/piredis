#ifndef REDIS_UTILS_H
#define REDIS_UTILS_H

#include <vector>

#include "utils.h"
#include "../node.h"

const int NODE_ID_INDEX = 0;
const int NODE_IP_PORT_CPORT = 1;
const int NODE_FLAGS_INDEX = 2;
const int NODE_MASTER_INDEX = 3;
const int NODE_PING_SENT_INDEX = 4;
const int NODE_PONG_RECV_INDEX = 5;
const int NODE_CONFIG_EPOCH_INDEX = 6;
const int NODE_LINK_STATE_INDEX = 7;

class RedisUtils {
public:
    // <id> <ip:port@cport> <flags> <master> <ping-sent> <pong-recv> <config-epoch> <link-state> <slot>
    static PiRedisNodeStruct nodeStringSplit(std::string nodeStr)
    {
        std::vector<std::string> nodeInfo = MyUtils::SplitString(nodeStr, " ");
        
        PiRedisNodeStruct res;
        for (size_t i = 0; i < nodeInfo.size(); ++i)
        {
            switch (i)
            {
            case NODE_ID_INDEX:
                res.m_strNodeId = nodeInfo[i];
                break;
            case NODE_FLAGS_INDEX:
                res.m_strFlags = nodeInfo[i];
                break;
            case NODE_MASTER_INDEX:
                res.m_strMaster = nodeInfo[i];
                break;
            case NODE_PING_SENT_INDEX:
                res.m_ui64PingSent = stoull(nodeInfo[i]);
                break;
            case NODE_PONG_RECV_INDEX:
                res.m_ui64PongRecv = stoull(nodeInfo[i]);
                break;
            case NODE_CONFIG_EPOCH_INDEX:
                res.m_strConfigEpoch = nodeInfo[i];
                break;
            case NODE_LINK_STATE_INDEX :
                res.m_strLinkState = nodeInfo[i];
                break;
            case NODE_IP_PORT_CPORT: {
                size_t ipPos = nodeInfo[i].find(":");
                size_t portPos = nodeInfo[i].find("@");
                std::string ip = nodeInfo[i].substr(0, ipPos);
                std::string port = nodeInfo[i].substr(ipPos + 1, portPos - (ipPos + 1));
                std::string cport = nodeInfo[i].substr(portPos + 1);
                res.m_strIp = ip;
                res.m_iPort = stoull(port);
                res.m_iCport = stoull(cport);
            }
                break;
            default:
                std::vector<std::string> slots = MyUtils::SplitString(nodeInfo[i], "-");
                res.m_iSlotBegin = atoi(slots[0].c_str());
                res.m_iSlotEnd = atoi(slots[1].c_str());
                break;
            }
        }
        return res;
    }

    // 根据 key 计算出 slot，然后从 cluster 列表中选择合适的节点
    static PiRedisNodeStruct* getRightClusterNode(const std::string& key, std::vector<PiRedisNodeStruct> clusterNodes) {
        int slot = MyUtils::GetSlotValue(key);
        for (size_t i = 0; i < clusterNodes.size(); ++i) {
            if (slot >= clusterNodes[i].m_iSlotBegin && slot <= clusterNodes[i].m_iSlotEnd) {
                return &clusterNodes[i];
            }
        }
        return nullptr;
    }
};

#endif