#include <condition_variable>
#include <thread>
#include <mutex>
#include <boost/noncopyable.hpp>

namespace ZyLib{
class CountDownLatch: boost::noncopyable{
    public:
    explicit CountDownLatch(int num){
        count_ = num;
    }
    void wait(){
        std::unique_lock<std::mutex> lock_guard(mutex_);
        cv_.wait(lock_guard,[&]{ count_>0;});
    }
    void countDown(){
        std::unique_lock<std::mutex> lock(mutex_);
        --count_;
        if (count_ == 0){
            cv_.notify_all();
        }
    }
    int getCount() const{
        std::unique_lock<std::mutex> lock_guard(mutex_);
        return count_;
    }
    private:
    mutable std::mutex mutex_;
    int count_;
    std::condition_variable cv_;
};
}