#include <iostream>
#include <vector>
#include <algorithm>
#include <hiredis.h>
#include <cstdint>
#include "piredis.h"
#include "utils/utils.h"
#include "utils/redis_utils.h"

#include "node.h"

using std::cout;
using std::endl;

enum ReplyCode {
    Success,
    ReplyNull
};

int main(void)
{
    // struct timeval tv;
    // tv.tv_sec = 0;
    // tv.tv_usec = 100000;
    PiRedis piRedis("101.37.20.126", 6381);
    
    if (piRedis.init()) {
        cout << "init success" << endl;
    }

    // cout << piRedis.sendCommandDirectly("ping") << endl;
    if (piRedis.setClusterMode()) {
        cout << "set cluster mode success" << endl;
    }

    // piRedis.connectPiRedisClusterNode("101.37.20.126", 6380);
    cout << piRedis.getFromCluster("foo1").replyString << endl;
    cout << piRedis.appendToCluster("foo1", "ddd").replyString << endl;
    // cout << piRedis.set("test_set", "100").replyString << endl;
    // cout << piRedis.setexToCluster("zzzz", 20, "value").replyString << endl;

    // cout << piRedis.mset(entries).replyString << endl;
    cout << piRedis.decrbyToCluster("abc", 10).replyString << endl;
    cout << piRedis.incrToCluster("abc").replyString << endl;
    cout << piRedis.decrToCluster("abc").replyString << endl;

    cout << piRedis.getsetToCluster("foo", "newfoo").replyString << endl;

    std::vector<std::string> values{"a", "b", "c"};
    cout << piRedis.lpushToCluster("foo_list", values).replyString << endl;
    // redisReply *reply = (redisReply *)redisCommand(c, "PING");
    // if (reply == NULL) {
    //     printf("reply is null[%s]\n", c->errstr);
    //     freeReplyObject(reply);
    //     return -1;
    // }

    // printf("PING: %s\n", reply->str);
    // freeReplyObject(reply);

    // reply = (redisReply *) redisCommand(c, "CLUSTER NODES");
    // if (reply == NULL) {
    //     printf("reply is null[%s]\n", c->errstr);
    //     freeReplyObject(reply);
    //     return -1;
    // }

    // std::vector<PiRedisNodeStruct> piRedisNodes;
    // if (reply->type == REDIS_REPLY_STRING)
    // {
    //     std::string lrangeReply{reply->str};
    //     std::string delimiter = "\n";
    //     size_t pos = 0;
    //     std::vector<std::string> vs;
    //     vs = MyUtils::SplitString(lrangeReply, delimiter);
        
    //     for_each(vs.begin(), vs.end(), [&](const auto& str) {
    //         PiRedisNodeStruct temp;
    //         temp = RedisUtils::nodeStringSplit(str);
    //         piRedisNodes.push_back(temp);
    //     });
    //     cout << piRedisNodes.size() << endl;
    // }
    // freeReplyObject(reply);

    std::string mystr = "foo1";
    std::cout << MyUtils::GetSlotValue(mystr) << std::endl;

    return 0;
}
