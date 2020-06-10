#include "net/eventloop.h"
#include "log/logger.h"
#include "thread/util.h"
#include <assert.h>

namespace ZyLib{
thread_local EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop():
  looping_(false),
  thread_id_(std::this_thread::get_id()){
  if (t_loopInThisThread){
    LOG_FATAL << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << ThreadId2Int(thread_id_);
  }
  else{
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop(){
  assert(!looping_);
  t_loopInThisThread = NULL;
}

void EventLoop::loop(){
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  while(!quit_){

  }
  looping_ = false;
}

void EventLoop::abortNotInLoopThread(){
  LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << ThreadId2Int(thread_id_)
            << ", current thread id = " <<  ThreadId2Int(std::this_thread::get_id());
}

}