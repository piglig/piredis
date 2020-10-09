#include <iostream>
#include <vector>
#include <algorithm>
#include <hiredis.h>
#include <cstdint>
#include <iomanip>
#include "utils/utils.h"
#include "utils/redis_utils.h"

#include "node.h"

using std::cout;
using std::endl;

int main(void)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    redisContext *c = redisConnectWithTimeout("101.37.20.126", 6379, tv);
    if (c == NULL || c->err) {
        if (c) {
            printf("Error: %s\n", c->errstr);
        } else {
            printf("Can't allocate redis context\n");
        }
    }

    redisReply *reply = (redisReply *)redisCommand(c, "PING");
    if (reply == NULL) {
        printf("reply is null[%s]\n", c->errstr);
        freeReplyObject(reply);
        return -1;
    }

    printf("PING: %s\n", reply->str);
    freeReplyObject(reply);

    reply = (redisReply *) redisCommand(c, "CLUSTER NODES");
    if (reply == NULL) {
        printf("reply is null[%s]\n", c->errstr);
        freeReplyObject(reply);
        return -1;
    }
    // abce \n dueiosdfj \n
    std::vector < PiRedisNodeStruct> piRedisNodes;
    if (reply->type == REDIS_REPLY_STRING)
    {
        std::string lrangeReply{reply->str};
        std::string delimiter = "\n";
        size_t pos = 0;
        std::vector<std::string> vs;
        vs = MyUtils::SplitString(lrangeReply, delimiter);
        
        for_each(vs.begin(), vs.end(), [&](const auto& str) {
            PiRedisNodeStruct temp;
            temp = RedisUtils::nodeStringSplit(str);
            // PiRedisNode piRedisNode;

            // cout << "port:" << temp.m_iPort << endl;
            // cout << "cport:" << temp.m_iCport << endl;
            // cout << "slotBegin:" << temp.m_iSlotBegin << endl;
            // cout << "slotEnd:" << temp.m_iSlotEnd << endl;
            // cout << "-----------------" << endl;
            piRedisNodes.push_back(temp);
        });
        cout << piRedisNodes.size() << endl;
    }

    freeReplyObject(reply);

    std::string mystr = "foo";
    std::cout << MyUtils::GetSlotValue(mystr) << std::endl;

    return 0;
}
