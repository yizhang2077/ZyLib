#include "net/eventloop.h"
#include "log/logger.h"
#include "thread/util.h"
#include "net/channel.h"
#include "net/epoller.h"
#include "timerqueue.h"
#include "common/timestamp.h"

#include <assert.h>
#include <sys/eventfd.h>


namespace ZyLib{
thread_local EventLoop* t_loopInThisThread = 0;
const int kEpollTimeMs = 10000;

static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    LOG_FATAL << "Failed in eventfd";
    abort();
  }
  return evtfd;
}

EventLoop::EventLoop():
  looping_(false),
  quit_(false),
  thread_id_(std::this_thread::get_id()),
  epoller_(new Epoller(this)),
  timer_queue_(new TimerQueue(this)),
  calling_pending_functor_(false),
  wakeup_fd_(createEventfd()),
  wakeup_channel_(new Channel(this,wakeup_fd_)){
  if (t_loopInThisThread){
    LOG_FATAL << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << ThreadId2Int(thread_id_);
  }
  else{
    t_loopInThisThread = this;
  }
  wakeup_channel_->setReadCallback(std::bind(&EventLoop::handleWakeupEvent,this));
  wakeup_channel_->EnableReading();
}

EventLoop::~EventLoop(){
  assert(!looping_);
  ::close(wakeup_fd_);
  t_loopInThisThread = NULL;
}

void EventLoop::quit(){
  quit_ = true;
  /**
   * If this loop thread can execute this function
   * It means this thread has been waken up
   **/
  if(!isInLoopThread()){
    wakeup();
  }
}

void EventLoop::loop(){
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  quit_ = false;

  while(!quit_){
    activeChannels_.clear();
    auto poll_return_time = epoller_->Poll(kEpollTimeMs,&activeChannels_);
    for(auto& channel:activeChannels_){
      channel->HandleEvent();
    }
    doPendingFunctors();
  }
  looping_ = false;
}

void EventLoop::abortNotInLoopThread(){
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << ThreadId2Int(thread_id_)
            << ", current thread id = " <<  ThreadId2Int(std::this_thread::get_id());
}

void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  switch (channel->getStatus())
  {
  case Channel::NEW:
    epoller_->addChannel(channel);
    break;
  case Channel::DELETED:
    epoller_->removeChannel(channel);
    break;
  case Channel::ADDED:
    epoller_->updateChannel(channel);
    break;
  default:
    LOG_FATAL<<"wrong channel status!";
  }
}

void EventLoop::wakeup(){
  uint64_t one = 1;
  ssize_t n = ::write(wakeup_fd_,&one,sizeof one);
  if(n != sizeof one){
    LOG_ERROR<<"EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::handleWakeupEvent(){
  uint64_t one = 1;
  ssize_t n = ::read(wakeup_fd_, &one, sizeof one);
  if (n != sizeof one)
  {
    LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}

void EventLoop::runAt(const Timestamp& time, const std::function<void()>& cb){
  timer_queue_->addTimer(cb,time,0.0);
}

void EventLoop::runAfter(double delay, const std::function<void()>& cb){
  Timestamp time = addTime(Timestamp::now(),delay);
  timer_queue_->addTimer(cb,time,0.0);
}

void EventLoop::runEvery(double interval, const std::function<void()>& cb){
  Timestamp time = addTime(Timestamp::now(),interval);
  timer_queue_->addTimer(cb,time,interval);
}

void EventLoop::runInLoop(const std::function<void()>&cb){
  if(isInLoopThread()){
    cb();
  }else{
    queueInLoop(cb);
  }
}

void EventLoop::queueInLoop(const std::function<void()>&cb){
  {
    std::unique_lock<std::mutex> lock_guard();
    pending_functors_.push_back(cb);
  }
  // if it's anther thread queue in loop , we need wake up loop
  // if it's this thread queue in loop, and calling functor is false
  // which means this queueInLoop function is called by channel->HandleEvent();
  // Because the loop has been waken up so we don't need to wake up this loop again
  if(!isInLoopThread()|| calling_pending_functor_){
    wakeup();
  }
}

void EventLoop::doPendingFunctors(){
  std::vector<std::function<void()>> functors;
  calling_pending_functor_ = true;
  {
    std::unique_lock<std::mutex> lock_guard();
    pending_functors_.swap(functors);
  }
  for(size_t i =0;i<functors.size();i++){
    functors[i]();
  }
  calling_pending_functor_ = false;
}

}