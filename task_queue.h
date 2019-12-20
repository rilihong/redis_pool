//
// Created by rilihong on 2019-12-03.
//

#ifndef REDIS_QUEUE_TASK_QUEUE_H
#define REDIS_QUEUE_TASK_QUEUE_H

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

typedef  std::function<void()> taskFunc;

class TaskQueue{
public:
    TaskQueue();
    bool addQueue(taskFunc task);
    int GetSize();
    ~TaskQueue();
private:
    void run();
private:
    std::thread *   _thread;
    std::vector<taskFunc>   _task_vec;
    std::atomic_bool        _exit;
    std::mutex              _mutex;
};

#endif //REDIS_QUEUE_TASK_QUEUE_H
