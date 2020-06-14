#ifndef NET_CHANNEL_H_
#define NET_CHANNEL_H_
#include <sys/epoll.h>
#include <functional>
#include <boost/noncopyable.hpp>

namespace ZyLib{

class EventLoop;

class Channel : boost::noncopyable{
  public:
    enum Status{
      DELETED,
      ADDED,
      NEW
    };
    Channel(EventLoop* ownerloop,int fd);
    ~Channel(){};
    void setReadCallback(const std::function<void()>& callback){ readCallback_ = callback;}
    void setWriteCallback(const std::function<void()>& callback){ writeCallback_ = callback;}
    void setErrorCallback(const std::function<void()>& callback){ errorCallback_ = callback;}
    void setCloseCallback(const std::function<void()>& callback){ closeCallback_ = callback;}
    
    int getFd() const { return fd_;}
    int getEvent() const {return fd_event_;}
    void EnableReading() { fd_event_ |= kReadEvent; update(); }
    void EnableWritting() { fd_event_ |= kWriteEvent; update(); }
    void DisableReading() { fd_event_ &= ~kReadEvent; update(); }
    void DisableWritting() { fd_event_ &= ~kWriteEvent; update(); }
    bool isNoneEvent() const { return fd_event_ == kNoneEvent; }

    void HandleEvent();
    
    Status getStatus() const{ return status_;}
    void setStatus(Status status) { status_ = status;}
    void setRevents(int revent) { revent_ = revent;}
    
    EventLoop* ownerLoop() { return owner_loop_;}
  private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    void update();

    int fd_;
    int fd_event_;
    int revent_;
    EventLoop* owner_loop_;    
    std::function<void()> readCallback_;
    std::function<void()> writeCallback_;
    std::function<void()> errorCallback_;
    std::function<void()> closeCallback_;
    
    Status status_;
};
}
#endif