//
// Created by rilihong on 2019-12-03.
//

#ifndef REDIS_QUEUE_BASE_MUTEX_H
#define REDIS_QUEUE_BASE_MUTEX_H

#include <mutex>

class base_mutex{
public:
    base_mutex(std::mutex * mutex);
    ~base_mutex();
private:
    std::mutex * _mutex;
};

#endif //REDIS_QUEUE_BASE_MUTEX_H
