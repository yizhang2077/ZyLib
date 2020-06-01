#ifndef TIME_SIMPLEST_TIMER_H_
#define TIME_SIMPLEST_TIMER_H_
#include <iostream>
#include <chrono>
#include <thread>

namespace ZyLib{
class Timer{
    public:
        template<typename Function>
        void setTimeout(Function func,int interval);
        
        template<typename Function>
        void setTimeInterval(Function func,int interval);

        void Stop();
    private:
        bool stop_ = false;
};


template<typename Function>
void Timer::setTimeout(Function func,int interval){
    this->stop_ = false;
    std::thread t([=](){
        if(this->stop_) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        if(this->stop_) return;
        func();
    });
    t.detach();
}


template<typename Function>
void Timer::setTimeInterval(Function func,int interval){
    this->stop_ = false;
    std::thread t([=](){
        while(true){
            if(this->stop_) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(this->stop_) return;
            func();
        }
    });
    t.detach();
}

void Timer::Stop(){
    stop_ = true;
}
}

#endif