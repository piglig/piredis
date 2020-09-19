#ifndef NODE_H
#define NODE_H
#include <string>
#include <vector>

class PiRedisNode {
public:
    PiRedisNode(cont std::string& nodeId, const std::string& ip, int port, int cPort, const std::string& flags,
        const std::string& master, uint64_t pingSent, uint64_t pongRecv, const std::string& configEpoch,
        const std::string& linkState, const std::string& slot)
    : m_strNodeId(nodeId), m_strIp(ip), m_iCport(cPort), m_strFlags(flags), m_strMaster(master),
    m_ui64PingSent(pingSent), m_ui64PongRecv(pongRecv), m_strConfigEpoch(configEpoch), m_strLinkState(linkState),
    m_strSlot(slot) {}

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