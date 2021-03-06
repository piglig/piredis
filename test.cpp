// #include <iostream>
// #include <algorithm>

// #include <cstdint>
// #include "piredis.h"

// using std::cout;
// using std::endl;

// enum ReplyCode {
//     Success,
//     ReplyNull
// };

// int main(void)
// {
//     // struct timeval tv;
//     // tv.tv_sec = 0;
//     // tv.tv_usec = 100000;
//     PiRedis piRedis("101.37.20.126", 6381);
    
//     if (piRedis.init()) {
//         cout << "init success" << endl;
//     }

//     // cout << piRedis.sendCommandDirectly("ping") << endl;
//     if (piRedis.setClusterMode()) {
//         cout << "set cluster mode success" << endl;
//     }

//     PiRedisReply reply;

//     // piRedis.connectPiRedisClusterNode("101.37.20.126", 6380);
//     cout << piRedis.getFromCluster("foo1").replyString << endl;
//     // cout << piRedis.appendToCluster("foo1", "ddd").replyString << endl;
//     // cout << piRedis.set("test_set", "100").replyString << endl;
//     // cout << piRedis.setexToCluster("zzzz", 20, "value").replyString << endl;

//     // cout << piRedis.mset(entries).replyString << endl;
//     cout << piRedis.decrbyToCluster("abc", 10).replyString << endl;
//     cout << piRedis.incrToCluster("abc").replyString << endl;
//     cout << piRedis.decrToCluster("abc").replyString << endl;

//     cout << piRedis.getsetToCluster("foo", "newfoo").replyString << endl;

//     std::vector<std::string> values{"a", "b", "c"};
//     // cout << piRedis.lpushToCluster("foo_list", values).replyString << endl;
//     // cout << piRedis.lpopToCluster("foo_list").replyString << endl;
//     // cout << piRedis.rpushToCluster("foo_list", values).replyString << endl;
//     // cout << piRedis.rpopToCluster("foo_list").replyString << endl;

//     cout << piRedis.llenToCluster("foo_list").replyString << endl;

//     reply = piRedis.lindexToCluster("foo_list", 10);
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     reply = piRedis.lpushxToCluster("zxvcz", values);
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     reply = piRedis.rpushxToCluster("zxvcz", values);
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     reply = piRedis.lrangeToCluster("foo_list", 0, -1);
//     if (reply.errorCode == REDIS_OK) {
//         for (const auto& item : reply.replyElements) {
//             cout << item << endl;
//         }
        
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     reply = piRedis.lsetToCluster("foo_list", 0, "lset_value");
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     struct ListOptionalArgs args;
//     args.option = ListOptions::COUNT;
//     args.optionValue = 2;

//     std::vector<ListOptionalArgs> options = { 
//         {ListOptions::COUNT, 0},
//         {ListOptions::RANK, 2},
//         {ListOptions::MAXLEN, 1}
//     };
//     reply = piRedis.lposToCluster("mylist", "c", args);
//     if (reply.errorCode == REDIS_OK) {
//         for (const auto& item : reply.replyElements) {
//             cout << item << endl;
//         }
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     reply = piRedis.lposToCluster("mylist", "c", options);
//     if (reply.errorCode == REDIS_OK) {
//         for (const auto& item : reply.replyElements) {
//             cout << item << endl;
//         }
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     reply = piRedis.saddToCluster("myset", {"abc", "bcd"});
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // scard test
//     reply = piRedis.scardToCluster("myset");
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // smembers test
//     reply = piRedis.smembersToCluster("myset");
//     if (reply.errorCode == REDIS_OK) {
//         for (const auto& item : reply.replyElements) {
//             cout << item << endl;
//         }
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // sismember test
//     reply = piRedis.sismember("myset", "abc");
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hset test
//     const std::map<std::string, std::string>& pairs{
//         {"field1", "value1"},
//         {"field2", "value2"}
//     };
//     reply = piRedis.hset("myhash", pairs);
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hget test
//     reply = piRedis.hget("myhash", "field1");
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hdel test
//     // reply = piRedis.hdel("myhash", "field1");
//     // if (reply.errorCode == REDIS_OK) {
//     //     cout << reply.replyString << endl;
//     // } else {
//     //     cout << reply.errorStr << endl;
//     // }

//     // hexists test
//     reply = piRedis.hdel("myhash", "field1");
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hgetall test
//     reply = piRedis.hgetall("myhash");
//     if (reply.errorCode == REDIS_OK) {
//         for (const auto& item : reply.replyElements) {
//             cout << item << endl;
//         }
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hincrby test
//     reply = piRedis.hincrby("myhash", "number1", 1);
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hincrbyfloat test
//     reply = piRedis.hincrbyfloat("myhash", "float1", 1.813);
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hkeys test
//     reply = piRedis.hkeys("myhash");
//     if (reply.errorCode == REDIS_OK) {
//         for (const auto& item : reply.replyElements) {
//             cout << item << endl;
//         }
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // hincrbyfloat test
//     reply = piRedis.hlen("myhash");
//     if (reply.errorCode == REDIS_OK) {
//         cout << reply.replyString << endl;
//     } else {
//         cout << reply.errorStr << endl;
//     }

//     // reply = piRedis.ltrimToCluster("mylist", 1, 3);
//     // if (reply.errorCode == REDIS_OK) {
//     //     cout << reply.replyString << endl;
//     // } else {
//     //     cout << reply.errorStr << endl;
//     // }

//     // redisReply *reply = (redisReply *)redisCommand(c, "PING");
//     // if (reply == NULL) {
//     //     printf("reply is null[%s]\n", c->errstr);
//     //     freeReplyObject(reply);
//     //     return -1;
//     // }

//     // printf("PING: %s\n", reply->str);
//     // freeReplyObject(reply);

//     // reply = (redisReply *) redisCommand(c, "CLUSTER NODES");
//     // if (reply == NULL) {
//     //     printf("reply is null[%s]\n", c->errstr);
//     //     freeReplyObject(reply);
//     //     return -1;
//     // }

//     // std::vector<PiRedisNodeStruct> piRedisNodes;
//     // if (reply->type == REDIS_REPLY_STRING)
//     // {
//     //     std::string lrangeReply{reply->str};
//     //     std::string delimiter = "\n";
//     //     size_t pos = 0;
//     //     std::vector<std::string> vs;
//     //     vs = MyUtils::SplitString(lrangeReply, delimiter);
        
//     //     for_each(vs.begin(), vs.end(), [&](const auto& str) {
//     //         PiRedisNodeStruct temp;
//     //         temp = RedisUtils::nodeStringSplit(str);
//     //         piRedisNodes.push_back(temp);
//     //     });
//     //     cout << piRedisNodes.size() << endl;
//     // }
//     // freeReplyObject(reply);



//     return 0;
// }
