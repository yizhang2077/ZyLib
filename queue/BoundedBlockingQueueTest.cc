#include "BoundedBlockingQueue.h"
#include <thread>
#include <chrono>
#include <iostream>

ZyLib::BoundedBlockingQueue<int> queue(10);
void Producer(){
    int i=0;
    while(true){
        queue.put(i++);
        std::cout << "put: "<<i-1<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    }
}

void Consumer(){
    while(true){
        int result = queue.take();
        std::cout<<"take: "<<result<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(){
    std::thread t1([&](){Producer();});
    std::thread t2([&](){Consumer();});
    t1.join();
    t2.join();
}