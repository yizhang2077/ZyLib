#ifndef QUEUE_BLOCKING_QUEUE_H_
#define QUEUE_BLOCKING_QUEUE_H_
#include <deque>
#include <mutex>
#include <condition_variable>
#include <boost/noncopyable.hpp>
#include <assert.h>
// thread safe blocking queue
namespace ZyLib{
template<typename T>
class BlockingQueue:boost::noncopyable{
    public:
        BlockingQueue():mutex_(),queue_(),cv_(){}
        void put(T t){
            std::unique_lock<std::mutex> lock_guard(mutex_);
            queue_.push_back(t);
            cv_.notify_one();
        }
        T take(){
            std::unique_lock<std::mutex> lock_guard(mutex_);
            cv_.wait(lock_guard,[&](){ return !queue_.empty();});
            assert(!queue_.empty());
            T front = queue_.front();
            queue_.pop_front();
            return front;
        }
        size_t size() const{
            std::unique_lock<std::mutex> lock_guard(mutex_);
            return queue_.size();
        }
    private:
        std::mutex mutex_;
        std::deque<T> queue_;
        std::condition_variable cv_;
};
}
#endif