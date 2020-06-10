#include "net/channel.h"
#include "log/logger.h"

#include <poll.h>

namespace ZyLib{

Channel::Channel(EventLoop* loop, int fd)
  :owner_loop_(loop),
  fd_(fd),
  fd_event_(0),
  revent_(0)
{
}

void Channel::update(){

}

void Channel::HandleEvent(){
  if (revent_ & POLLNVAL) {
    LOG_WARN << "Channel::handle_event() POLLNVAL";
  }

  if (revent_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_) errorCallback_();
  }
  if (revent_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_) readCallback_();
  }
  if (revent_ & POLLOUT) {
    if (writeCallback_) writeCallback_();
  }
}

}