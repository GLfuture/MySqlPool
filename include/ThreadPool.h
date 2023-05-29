#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <functional>
#include<future>
#endif

class ThreadPool
{
private:
    class Task
    {
    public:
        std::function<void()> __Func; // 任务函数
    };

    void Create(uint32_t num){
        terminate.store(0,std::memory_order_relaxed); 
        threads.assign(0,NULL);
        Threads_Create(num);
    }

    void Run()
    {
        while (1)
        {
            std::unique_lock<std::mutex> lock(mtx);
            cond.wait(lock,[this](){
                return !tasks.empty() || terminate.load() == 1;
            });
            //std::cout<<"Run"<<std::endl;
            if(terminate.load()) break;
            std::shared_ptr<Task> task=tasks.front();
            tasks.pop();
            lock.unlock();
            task->__Func();
        }
        
    }


    void Threads_Create(uint32_t num){
        for (int i = 0; i < num; i++)
        {
            std::shared_ptr<std::thread> th=std::make_shared<std::thread>(&ThreadPool::Run,this);
            //std::cout<<"Threads_Create"<<std::endl;
            threads.push_back(th);
        }
    }

    void WaitForAllDone(){//等待任务全部结束
        for(int i=0;i<threads.size();i++){
            if(threads[i]!=NULL&&threads[i]->joinable()){//判断thread是否为空，防止destroy后调用析构函数重复销毁
                threads[i]->join();
            }
        }
    }

    void Threads_Destroy(){
        for(int i=0;i<threads.size();i++)
        {
            if(threads[i]!=NULL)
                threads[i].reset();
            //std::cout<<"Thread_Destory"<<std::endl;
        }
    }

public:
    ThreadPool(uint32_t num = 4)
    {
        Create(num);
    }

    template<typename F,typename ...Args>
    auto exec(F&&f,Args&&...args)->std::future<decltype(f(args...))>
    {
        //std::cout<<"exec"<<std::endl;
        using RetType=decltype(f(args...));
        std::shared_ptr<std::packaged_task<RetType()>> func=std::make_shared<std::packaged_task<RetType()>>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
        std::shared_ptr<Task> task=std::make_shared<Task>();
        task->__Func=[func](){
            (*func)();
        };
        std::unique_lock<std::mutex> lock(mtx);
        tasks.push(task);
        lock.unlock();
        cond.notify_one();
        return func->get_future();
    }

    void Destroy(){
        terminate.store(1,std::memory_order_relaxed);
        cond.notify_all();
        WaitForAllDone();
        Threads_Destroy();
    }

    ~ThreadPool(){
        Destroy();
    }
protected:

    std::queue<std::shared_ptr<Task>> tasks;                            // 任务队列
    std::vector<std::shared_ptr<std::thread>> threads; // 工作线程
    std::mutex mtx;
    std::condition_variable cond; // 条件变量
    std::atomic<int> terminate;//结束线程池

};
