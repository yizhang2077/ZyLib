#ifndef NET_EVENTLOOP_H_
#define NET_EVENTLOOP_H_

#include <boost/noncopyable.hpp>
#include <thread>
#include <vector>
#include <memory>
#include <functional>

#include <mutex>

namespace ZyLib{

class Channel;
class Epoller;
class TimerQueue;
class Timestamp;

class EventLoop : boost::noncopyable {
  public:
  EventLoop();
  ~EventLoop();

  void loop();
  void quit();

  void runInLoop(const std::function<void()>& cb);

  void queueInLoop(const std::function<void()>& cb);

  void runAt(const Timestamp& time, const std::function<void()>& cb);

  void runAfter(double delay, const std::function<void()>& cb);
  
  void runEvery(double interval, const std::function<void()>& cb);
  
  void assertInLoopThread(){
    if(!isInLoopThread()){
      abortNotInLoopThread();
    }
  }

  void updateChannel(Channel* channel);
  
  bool isInLoopThread() { return thread_id_ == std::this_thread::get_id();}

  private:
    typedef std::vector<Channel*> ChannelList;

    void abortNotInLoopThread();

    void wakeup();
    void handleWakeupEvent();

    void doPendingFunctors();

    bool looping_;
    bool quit_;
    const std::thread::id thread_id_;
    ChannelList activeChannels_;
    std::unique_ptr<Epoller> epoller_;
    std::unique_ptr<TimerQueue> timer_queue_;
    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;
    std::vector<std::function<void()>> pending_functors_;
    std::mutex pending_mutex_;
    bool calling_pending_functor_;
};

}

#endif