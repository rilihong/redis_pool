//
// Created by rilihong on 2019-12-03.
//

#ifndef REDIS_QUEUE_REDIS_POOL_H
#define REDIS_QUEUE_REDIS_POOL_H

#include "redis_connect.h"
#include <set>

struct RedisInfo{
    std::string _ip;
    int _port;
    std::string _auth;
    RedisInfo(std::string ip,int port,std::string auth);
    RedisInfo(RedisInfo &other);
};

class RedisPool{
public:
    RedisPool(RedisInfo & info,int num = 50);
    RedisConnect * GetClient();
    void ReturnClient(RedisConnect * client);
    ~RedisPool();
private:
    RedisConnect * CreateClient();
    RedisConnect * GetNoUseClient();
    RedisConnect * GetTmpClient();
    bool ReturnNoUseSet(RedisConnect * client);
    void ReturnTmpSet(RedisConnect * client);
private:
    RedisInfo _info;
    int _init_num;
    std::set<RedisConnect *> _no_use_set;
    std::set<RedisConnect *> _using_set;
    std::set<RedisConnect *> _tmp_set;
    std::mutex               _mutex;
    std::mutex               _tmp_mutex;
};

#endif //REDIS_QUEUE_REDIS_POOL_H
