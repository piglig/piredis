#ifndef NODE_H
#define NODE_H

#include <vector>

// const int NODE_ID_INDEX = 0;
// const int NODE_IP_INDEX = 1;
// const int NODE_PORT_INDEX = 2;
// const int NODE_CPORT_INDEX = 3;
// const int NODE_FLAGS_INDEX = 4;
// const int NODE_MASTER_INDEX = 5;
// const int NODE_PING_SENT_INDEX = 6;
// const int NODE_PONG_RECV_INDEX = 7;
// const int NODE_CONFIG_EPOCH_INDEX = 8;
// const int NODE_LINK_STATE_INDEX = 9;

struct PiRedisNodeStruct
{
public:
    // 节点 id
    std::string m_strNodeId;
    // 节点 ip
    std::string m_strIp;
    // 节点端口
    int m_iPort;
    // 节点 cport
    int m_iCport;
    std::string m_strFlags;
    std::string m_strMaster;
    uint64_t m_ui64PingSent;
    uint64_t m_ui64PongRecv;
    std::string m_strConfigEpoch;
    std::string m_strLinkState;
    int m_iSlotBegin;
    int m_iSlotEnd;
};

class PiRedisNode {
public:
    PiRedisNode(const std::string& nodeId, const std::string &ip, int port, int cPort, const std::string &flags,
                const std::string &master, uint64_t pingSent, uint64_t pongRecv, const std::string &configEpoch,
                const std::string &linkState, const std::string &slot = "")
        : m_strNodeId(nodeId), m_strIp(ip), m_iCport(cPort), m_strFlags(flags), m_strMaster(master),
        m_ui64PingSent(pingSent), m_ui64PongRecv(pongRecv), m_strConfigEpoch(configEpoch), m_strLinkState(linkState),
        m_strSlot(slot)
    {

    };

    std::string getNodeId() {
        return m_strNodeId;
    }

    std::string getNodeIp() {
        return m_strIp;
    }

    int getNodePort() {
        return m_iPort;
    }

    int getNodeCport() {
        return m_iCport;
    }

    std::string getNodeFlags() {
        return m_strFlags;
    }

    std::string getNodeMaster() {
        return m_strMaster;
    }

    uint64_t getNodePingSent() {
        return m_ui64PingSent;
    }

    uint64_t getNodePongRecv() {
        return m_ui64PongRecv;
    }

    std::string getNodeConfigEpoch() {
        return m_strConfigEpoch;
    }

    std::string getNodeLinkState() {
        return m_strLinkState;
    }

    std::string getNodeSlot() {
        return m_strSlot;
    }

private:
    // 节点 id
    std::string m_strNodeId;
    std::string m_strIp;
    int m_iPort;
    int m_iCport;
    std::string m_strFlags;
    std::string m_strMaster;
    uint64_t m_ui64PingSent;
    uint64_t m_ui64PongRecv;
    std::string m_strConfigEpoch;
    std::string m_strLinkState;
    std::string m_strSlot;
};

#endif