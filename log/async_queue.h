#ifndef LOG_ASYNC_QUEUE_H_
#define LOG_ASYNC_QUEUE_H_

#include "logfile.h"
#include "common/blockingqueue.h"
#include "common/boundedblockingqueue.h"

#include <thread>
#include <assert.h>
#include <string.h>
#include <functional>
#include <boost/noncopyable.hpp>

namespace ZyLib{

class LogMessage{
  public:
  LogMessage(const char* msg,size_t len){
    assert(len <= sizeof(data_));
    memcpy(data_,msg,len);
    length_ = len;
  }
  LogMessage(const LogMessage& msg){
    assert(msg.length() <= sizeof(data_));
    ::memcpy(data_,msg.data(),msg.length());
    length_ = msg.length();
  }
  LogMessage(){
    length_ = 0;
  }
  LogMessage& operator=(const LogMessage& msg){
    assert(msg.length() <= sizeof(data_));
    ::memcpy(data_,msg.data(),msg.length());
    length_ = msg.length();
  }

  size_t length() const{return length_;}
  const char* data() const {return data_;}
  bool empty() const {return length_ == 0;}
  private:
    char data_[4000];
    size_t length_;
};

template <typename MSG,template <typename> class QUEUE>
class AsyncLogQueue:boost::noncopyable{
  public:
    AsyncLogQueue(const std::string& basename,int rollsize)
      :basename_(basename),
      rollsize_(rollsize){
    }
    AsyncLogQueue(const std::string& basename,int rollsize,int queue_size)
      :basename_(basename),
      rollsize_(rollsize),
      queue_(queue_size){
    }
    ~AsyncLogQueue(){
      if(!stop_){
        Stop();
      }
    }
    void Append(const char* msg, size_t len){
      MSG log_message(msg,len);
      queue_.put(log_message);
    }
    
    void Start(){
      stop_ = false;
      thread_ = std::thread(std::bind(&AsyncLogQueue::threadFunc,this));
    }

    void Stop(){
      stop_ = true;
      queue_.put(MSG("\0",1));
      if(thread_.joinable()){
        thread_.join();
      }
    }
    bool isStop()const{
      return stop_;
    }
  private:
    void threadFunc(){
      assert(stop_ == false);
      LogFile output(basename_,rollsize_,flushinterval_);
      while(true){
        MSG msg(queue_.take());
        if(msg.length()==1&&msg.data()[0]=='\0'){
          assert(stop_==true);
          break;
        }
        output.Append(msg.data(), msg.length());
      }
      output.Flush();
    }
    std::string basename_;
    size_t rollsize_;
    int flushinterval_ = 3;
    bool stop_ = false;
    std::thread thread_;
    QUEUE<MSG> queue_;

};

typedef AsyncLogQueue<std::string, ZyLib::BlockingQueue> AsyncLogUnboundedQueue;
typedef AsyncLogQueue<std::string, ZyLib::BoundedBlockingQueue> AsyncLogBoundedQueue;
typedef AsyncLogQueue<ZyLib::LogMessage, ZyLib::BlockingQueue> AsyncLogUnboundedQueueL;
typedef AsyncLogQueue<ZyLib::LogMessage, ZyLib::BoundedBlockingQueue> AsyncLogBoundedQueueL;

}
#endif