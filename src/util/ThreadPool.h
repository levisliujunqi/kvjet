#pragma once
#include<vector>
#include<thread>
#include<queue>
#include<functional>
#include<mutex>
#include<future>
#include<condition_variable>
#include<type_traits>
class ThreadPool{
public:
    //加入一个任务
    template<typename T,typename... Args>
    auto enqueue(T&& f, Args&&... args)->std::future<typename std::invoke_result<T,Args...>::type>;
    //等待所有任务完成，停止
    void shutdown();
    //立即停止
    void shutdownnow();
    ThreadPool(int threadCount);
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    bool stop;
    std::condition_variable cv;
};