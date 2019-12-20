//
// Created by rilihong on 2019-12-03.
//

#include "base_mutex.h"

base_mutex::base_mutex(std::mutex *mutex) {
    _mutex = mutex;
    _mutex->lock();
}

base_mutex::~base_mutex() {
    _mutex->unlock();
}
