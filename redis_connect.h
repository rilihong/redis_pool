//
// Created by rilihong on 2019-12-03.
//

#ifndef REDIS_QUEUE_REDIS_CONNECT_H
#define REDIS_QUEUE_REDIS_CONNECT_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <vector>
#include <hiredis/hiredis.h>
#include <unordered_map>

typedef std::unordered_map<std::string ,std::string> HashValue;
typedef std::vector<std::string> SetValue;

class RedisConnect{
public:
    RedisConnect(std::string ip,int port,std::string auth);
    bool Init();
    ~RedisConnect();
public:
    bool Get(std::string key,std::string & value);
    void Set(std::string key,std::string value);
    void Keys(std::string key,SetValue &value);
    void Del(std::string key);
    void Select(int db_num);
public:
    bool SDiff(std::string s1,std::string s2,SetValue & value);
    void SAdd(std::string key,std::string value);
    bool SInter(std::string s1,std::string s2,SetValue & value);
public:
    bool HGetAll(std::string key,HashValue &value);
    void HMSet(std::string key,HashValue &value);
private:
    bool connect();
    bool auth();
private:
    std::string _ip;
    int         _port;
    std::string _auth;
    redisContext * _context;
};

#endif //REDIS_QUEUE_REDIS_CONNECT_H
