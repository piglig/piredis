#include <iostream>
#include <vector>
#include <algorithm>
#include <hiredis.h>
#include "utils/utils.h"

using std::cout;
using std::endl;

int main(void)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    redisContext *c = redisConnectWithTimeout("127.0.0.1", 6379, tv);
    if (c == NULL || c->err) {
    if (c) {
        printf("Error: %s\n", c->errstr);
        // handle error
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
    if (reply->type == REDIS_REPLY_STRING) {
        std::string lrangeReply{reply->str};
        std::string delimiter = "\n";
        size_t pos = 0;
        std::vector<std::string> vs;
        vs = MyUtils::SplitString(lrangeReply, delimiter);
        
        for_each(vs.begin(), vs.end(), [](const auto& str) {
            std::vector<std::string> temp;
            temp = MyUtils::SplitString(str, " ");
            for_each(temp.begin(), temp.end(), [](const auto& str) {
                cout << str << endl;
            });
            cout << "-----------------" << endl;
        });
        
        cout << reply->type << endl;
    }
    
    freeReplyObject(reply);

    return 0;
}
