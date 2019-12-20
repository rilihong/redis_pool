#include <iostream>
#include "redis_connect.h"
#include "redis_pool.h"
#include <functional>
#include <unistd.h>

const static int TNum = 8;
const static std::string S1 = "co";
const static std::string S2 = "do";

const static std::string local_ip = "127.0.0.1";
const static std::string local_auth = "";
const static int local_port = 6379;
const static std::string remote_ip = "127.0.0.1";
const static std::string remote_auth = "";
const static int remote_port = 6793;

typedef std::unordered_map<std::string ,HashValue> HashMap;

std::string get_now_time(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t now = tv.tv_sec * 1000 + tv.tv_usec/1000;
    return std::to_string(now);
}

void getDiffValue(RedisConnect * local_client,RedisConnect * remote_client){
    SetValue t_1;
    SetValue t_2;
    local_client->Keys("*hello*",t_1);
    remote_client->Keys("*hello*",t_2);
    std::sort (t_1.begin(),t_1.end());
    std::sort (t_2.begin(),t_2.end());
    SetValue t_3(t_1.size());
    auto lend = std::set_difference(t_1.begin(),t_1.end(),t_2.begin(),t_2.end(),t_3.begin());
    t_3.resize(lend-t_3.begin());
    HashValue hMap;
    for(auto itr = t_3.begin();itr != t_3.end();++itr){
        std::string value = "";
        local_client->Get(*itr,value);
        hMap[*itr] = value;
    }
    for(auto itr = hMap.begin();itr != hMap.end();++itr){
        remote_client->Set(itr->first,itr->second);
    }
}

void getDiffHash(RedisConnect * local_client,RedisConnect * remote_client){
    SetValue t_1;
    SetValue t_2;
    local_client->Keys("*hash*",t_1);
    remote_client->Keys("*hash*",t_2);
    std::sort (t_1.begin(),t_1.end());
    std::sort (t_2.begin(),t_2.end());
    SetValue t_3(t_1.size());
    auto lend = std::set_difference(t_1.begin(),t_1.end(),t_2.begin(),t_2.end(),t_3.begin());
    t_3.resize(lend-t_3.begin());
    HashMap hMap;
    for(auto itr = t_3.begin();itr != t_3.end();++itr){
        HashValue value;
        local_client->HGetAll(*itr,value);
        hMap[*itr] = value;
    }
    for(auto itr = hMap.begin();itr != hMap.end();++itr){
        remote_client->HMSet(itr->first,itr->second);
    }
}


int main(int argc ,char ** args) {
    if(argc != 4){
        return -1;
    }
    RedisInfo local_info(local_ip,local_port,local_auth);
    RedisPool * local_pool = new RedisPool(local_info,TNum);

    RedisInfo remote_info(remote_ip,remote_port,remote_auth);
    RedisPool * remote_pool = new RedisPool(remote_info,TNum);

    auto local_client = local_pool->GetClient();
    auto remote_client = remote_pool->GetClient();

    getDiffValue(local_client, remote_client);
    getDiffHash(local_client, remote_client);

    local_pool->ReturnClient(local_client);
    local_pool->ReturnClient(remote_client);
    return 0;
}