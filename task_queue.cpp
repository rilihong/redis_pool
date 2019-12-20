//
// Created by rilihong on 2019-12-03.
//

#include "task_queue.h"
#include "base_mutex.h"

TaskQueue::TaskQueue():_exit(false){
    _thread = new std::thread(&TaskQueue::run,this);
}

int TaskQueue::GetSize(){
    base_mutex mutex(&_mutex);
    _task_vec.size();
}

TaskQueue::~TaskQueue(){
    _exit.store(true);
    _thread->join();
    std::vector<taskFunc>::iterator itr = _task_vec.begin();
    for(;itr !=_task_vec.end();++itr){
        (*itr)();
    }
}

bool TaskQueue::addQueue(taskFunc task){
    if(_exit.load()){
        return false;
    }
    base_mutex mutex(&_mutex);
    _task_vec.push_back(task);
    return true;
}

void TaskQueue::run(){
    while(!_exit.load()){
        std::vector<taskFunc> tmp;
        {
            base_mutex mutex(&_mutex);
            tmp.swap(_task_vec);
        }
        if(tmp.size() == 0){
            continue;
        }
        std::vector<taskFunc>::iterator itr = tmp.begin();
        for(;itr !=tmp.end();++itr){
            (*itr)();
        }
    }
}