#include <iostream>
#include <hiredis.h>

using std::cout;
using std::endl;

int main(void)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    redisContext *c = redisConnectWithTimeout("10.10.243.226", 6379, tv);
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

    return 0;
}
