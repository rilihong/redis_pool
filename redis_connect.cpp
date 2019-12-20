//
// Created by rilihong on 2019-12-03.
//
#include "redis_connect.h"
#include "base_mutex.h"
#include <iostream>

RedisConnect::RedisConnect(std::string ip,int port,std::string auth)
        :_ip(ip),_port(port),_auth(auth){}

RedisConnect::~RedisConnect(){
    if(_context){
        redisFree(_context);
    }
}

bool RedisConnect::Init(){
    if(!connect()){
        return false;
    }
    if(!auth()){
        return false;
    }
    return true;
}

bool RedisConnect::connect(){
    struct timeval timeout = {1, 0};
    _context = redisConnectWithTimeout(_ip.c_str(), _port, timeout);
    if (_context == NULL) {
        return false;
    } else if (_context->err) {
        redisFree(_context);
        return false;
    }
    return true;
}
bool RedisConnect::auth(){
    if (_context == NULL) {
        return false;
    }
    if(_auth == ""){
        return true;
    }
    redisReply *reply = (redisReply*)redisCommand(_context,"AUTH %s",_auth.c_str());
    if (!reply) {
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool RedisConnect::Get(std::string key,std::string & value){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "GET %s",
                                          key.c_str());
        if (reply == NULL) {
            return false;
        }

        if (reply->type != REDIS_REPLY_STRING) {
            std::cout <<"no key"<<std::endl;
            return false;
        }

        if (reply->len <= 0) {
            return false;
        }

        value = std::string(reply->str, reply->len);
    } while(0);

    if (reply != NULL) {
        freeReplyObject(reply);
    }
    return true;
}
void RedisConnect::Set(std::string key,std::string value){
    std::string result = "";
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "SET %s %s",
                                          key.c_str(),
                                          value.c_str());
        if (reply == NULL) {
            break;
        }
    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
}

void RedisConnect::Keys(std::string key,SetValue &value){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "KEYS %s",
                                          key.c_str());
        if (reply == NULL) {
            break;
        }
        if(reply->type != REDIS_REPLY_ARRAY){
            break;
        }
        for(int i = 0;i < reply->elements;i++){
            auto ele = reply->element[i];
            if(ele->type != REDIS_REPLY_STRING){
                continue;
            }
            value.push_back(std::string(ele->str,ele->len));
        }

    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
}

void RedisConnect::Del(std::string key){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "DEL %s",
                                          key.c_str());
        if (reply == NULL) {
            break;
        }
        if(reply->type != REDIS_REPLY_STATUS){
            std::cout << "DEL:" << reply->type << std::endl;
            break;
        }

    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
}

void RedisConnect::Select(int db_num){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "SELECT %d",
                                          db_num);
        if (reply == NULL) {
            break;
        }
        if(reply->type != REDIS_REPLY_STATUS){
            std::cout << "SELECT:" << reply->type << std::endl;
            break;
        }

    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
}

bool RedisConnect::SDiff(std::string s1,std::string s2,SetValue& value){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "SDIFF %s %s",
                                          s1.c_str(),
                                          s2.c_str());
        if (reply == NULL) {
            return false;
        }
        if(reply->type != REDIS_REPLY_ARRAY){
            return false;
        }
        for(int i =0;i < reply->elements;i++){
            auto ele = reply->element[i];
            if(ele->type != REDIS_REPLY_STRING){
                continue;
            }
            value.push_back(std::string(ele->str,ele->len));
        }
    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
    return true;
}

void RedisConnect::SAdd(std::string key,std::string value){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "SADD %s %s",
                                          key.c_str(),
                                          value.c_str());
        if (reply == NULL) {
            break;
        }
    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
}

bool RedisConnect::SInter(std::string s1,std::string s2,SetValue& value){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "SINTER %s %s",
                                          s1.c_str(),
                                          s2.c_str());
        if (reply == NULL) {
            return false;
        }
        if(reply->type != REDIS_REPLY_ARRAY){
            return false;
        }
        for(int i =0;i < reply->elements;i++){
            auto ele = reply->element[i];
            if(ele->type != REDIS_REPLY_STRING){
                continue;
            }
            value.push_back(std::string(ele->str,ele->len));
        }
    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
    return true;
}

bool RedisConnect::HGetAll(std::string key,HashValue &value){
    redisReply *reply = NULL;
    do {
        reply = (redisReply*)redisCommand(_context,
                                          "HGETALL %s",
                                          key.c_str());
        if (reply == NULL) {
            return false;
        }
        if(reply->type != REDIS_REPLY_ARRAY){
            return false;
        }
        for(int i =0;i < reply->elements - 1;i +=2){
            auto ele_key = reply->element[i];
            auto ele_value = reply->element[i+1];
            if(ele_key->type != REDIS_REPLY_STRING
                || ele_value->type != REDIS_REPLY_STRING){
                continue;
            }
            std::string ekey = std::string(ele_key->str);
            value[ekey]= std::string(ele_value->str,ele_value->len);
        }
    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
    return true;
}

void RedisConnect::HMSet(std::string key,HashValue &value){
    redisReply *reply = NULL;
    std::vector<const char *> argv;
    std::vector<size_t> argvlen;
    argv.push_back("HMSET");
    argvlen.push_back(5);
    argv.push_back(key.c_str());
    argvlen.push_back(key.length());
    for(HashValue::iterator itr = value.begin()
            ;itr != value.end();++itr){
        argv.push_back(itr->first.c_str());
        argvlen.push_back(itr->first.length());
        argv.push_back(itr->second.c_str());
        argvlen.push_back(itr->second.length());
    }
    do {
        //reply = (redisReply*)redisCommand(_context,"HMSET %s %s",key.c_str(),arg.c_str());
        reply = (redisReply*)redisCommandArgv(_context,argv.size(),&(argv[0]), &(argvlen[0]) );
        if (reply == NULL) {
            return;
        }
        std::cout << "reply:"<<reply->type << std::endl;
    } while(0);
    if (reply != NULL) {
        freeReplyObject(reply);
    }
}