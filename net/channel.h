#ifndef NET_CHANNEL_H_
#define NET_CHANNEL_H_
#include <sys/epoll.h>
#include <functional>
#include <boost/noncopyable.hpp>

namespace ZyLib{

class EventLoop;

class Channel : boost::noncopyable{
  public:
    Channel(EventLoop* ownerloop,int fd);
    void setReadCallback(const std::function<void()>& callback){ readCallback_ = callback;}
    void setWriteCallback(const std::function<void()>& callback){ writeCallback_ = callback;}
    void setErrorCallback(const std::function<void()>& callback){ errorCallback_ = callback;}
    int getFd() const { return fd_;}
    int getEvent() const {return fd_event_;}
    void EnableReading() { }
    void HandleEvent();
  private:
    void update();
    int fd_;
    int fd_event_;
    int revent_;
    EventLoop* owner_loop_;    
    std::function<void()> readCallback_;
    std::function<void()> writeCallback_;
    std::function<void()> errorCallback_;  
};
}
#endif