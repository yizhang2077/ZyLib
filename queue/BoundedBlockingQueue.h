#ifndef QUEUE_BOUNDED_BLOCKING_QUEUE_H_
#define QUEUE_BOUNDED_BLOCKING_QUEUE_H_
#include <deque>
#include <mutex>
#include <condition_variable>
#include <boost/noncopyable.hpp>
#include <assert.h>
// thread safe bounded blocking queue
namespace ZyLib
{
    template <typename T>
    class BoundedBlockingQueue : boost::noncopyable
    {
    public:
        BoundedBlockingQueue(int max_size) 
        : mutex_(), 
          queue_(std::deque<T>(max_size+1)), 
          max_size_(max_size+1),
          non_empty_(), 
          non_full_() {
              assert(max_size>0);
        }

        void put(T t)
        {
            std::unique_lock<std::mutex> lock_guard(mutex_);
            non_full_.wait(lock_guard,[&]{return (tail_ + 1)%max_size_ != head_;});
            queue_[tail_] = t;
            tail_ = (tail_ + 1)%max_size_;
            non_empty_.notify_one();
        }
        T take()
        {
            std::unique_lock<std::mutex> lock_guard(mutex_);
            non_empty_.wait(lock_guard, [&]{ return head_ != tail_; });
            T front = queue_[head_];
            head_ = (head_ + 1)%max_size_;
            non_full_.notify_one();
            return front;
        }

        bool empty() const{
            std::lock_guard<std::mutex> lock_guard(mutex_);
            return head_ == tail_;
        }
        
        bool full() const{
            std::lock_guard<std::mutex> lock_guard(mutex_);
            return (tail_ + 1)%max_size_ == head_;
        }

        size_t size() const
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            return tail_>=head_ ? tail_-head_:(max_size_-(head_-tail_));
        }

        size_t capacity() const{
            std::lock_guard<std::mutex> lock_guard(mutex_);
            return max_size_-1;
        }

    private:
        std::mutex mutex_;
        std::deque<T> queue_;
        int max_size_;
        int head_ = 0;
        int tail_ = 0;
        std::condition_variable non_empty_;
        std::condition_variable non_full_;
    };
} // namespace ZyLib
#endif