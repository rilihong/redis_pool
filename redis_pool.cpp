//
// Created by rilihong on 2019-12-03.
//

#include "redis_pool.h"
#include "base_mutex.h"

RedisInfo::RedisInfo(std::string ip,int port,std::string auth)
        :_ip(ip),_port(port),_auth(auth){}

RedisInfo::RedisInfo(RedisInfo &other){
    _ip = other._ip;
    _port = other._port;
    _auth = other._auth;
}

RedisPool::RedisPool(RedisInfo & info,int num):_info(info),_init_num(num){
    for(int i = 0;i < _init_num;++i){
        auto client = CreateClient();
        _no_use_set.insert(client);
    }
}

RedisPool::~RedisPool(){
    for(auto itr = _no_use_set.begin()
            ;itr != _no_use_set.end();){
        auto client = *itr;
        itr = _no_use_set.erase(itr);
        delete client;
    }
    for(auto itr = _using_set.begin()
            ;itr != _using_set.end();){
        auto client = *itr;
        itr = _using_set.erase(itr);
        delete client;
    }
    for(auto itr = _tmp_set.begin()
            ;itr != _tmp_set.end();){
        auto client = *itr;
        itr = _tmp_set.erase(itr);
        delete client;
    }
}

RedisConnect * RedisPool::GetClient(){
    auto cli = GetNoUseClient();
    if(cli){
        return cli;
    }else{
        return GetTmpClient();
    }
}

RedisConnect * RedisPool::GetNoUseClient(){
    base_mutex mutex(&_mutex);
    if(!_no_use_set.empty()){
        auto f = _no_use_set.begin();
        RedisConnect * result = *f;
        _no_use_set.erase(f);
        _using_set.insert(result);
        return result;
    }
    return NULL;
}
RedisConnect * RedisPool::GetTmpClient(){
    base_mutex mutex(&_tmp_mutex);
    auto new_client = CreateClient();
    _tmp_set.insert(new_client);
    return new_client;
}

void RedisPool::ReturnClient(RedisConnect * client){
    if(!client){
        return;
    }
    auto r = ReturnNoUseSet(client);
    if(!r){
        ReturnTmpSet(client);
    }
}

bool RedisPool::ReturnNoUseSet(RedisConnect * client){
    base_mutex mutex(&_mutex);
    auto f = _using_set.find(client);
    if(f != _using_set.end()){
        RedisConnect * result = *f;
        _using_set.erase(f);
        _no_use_set.insert(result);
        return true;
    }
    return false;
}
void RedisPool::ReturnTmpSet(RedisConnect * client){
    base_mutex mutex(&_tmp_mutex);
    auto f = _tmp_set.find(client);
    if(f != _tmp_set.end()){
        _tmp_set.erase(f);
    }
    delete client;
}

RedisConnect * RedisPool::CreateClient(){
    RedisConnect * client = new RedisConnect(_info._ip,_info._port,_info._auth);
    client->Init();
    return client;
}
