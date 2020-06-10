#ifndef NET_EVENTLOOP_H_
#define NET_EVENTLOOP_H_
#include <boost/noncopyable.hpp>
#include <thread>

namespace ZyLib{

class EventLoop : boost::noncopyable {
  public:
  EventLoop();
  ~EventLoop();

  void loop();
  
  void assertInLoopThread(){
    if(!isInLoopThread()){
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() { return thread_id_ == std::this_thread::get_id();}

  private:
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const std::thread::id thread_id_;
};

}

#endif