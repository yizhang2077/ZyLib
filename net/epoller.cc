#include "net/epoller.h"
#include "log/logger.h"
#include "net/channel.h"

#include <assert.h>

namespace ZyLib{
namespace {
  constexpr int kInitEpollSize = 1024;
}

Epoller::Epoller(EventLoop* event_loop):
  ownerloop_(event_loop),
  epollfd_(epoll_create1(EPOLL_CLOEXEC)),
  event_list_(kInitEpollSize)
{
  if(epollfd_<0){
    LOG_FATAL<<"Epoller::Epoller";
  }
}

Epoller::~Epoller(){
  ::close(epollfd_);
}

Timestamp Epoller::Poll(int timeout_ms,ChannelList* channel_list){
  int event_num = epoll_wait(epollfd_,&*event_list_.begin(),
                             static_cast<int>(event_list_.size()),
                             timeout_ms);
  Timestamp now(Timestamp::now());
  if(event_num > 0){
    fillActiveChannels(event_num,channel_list);
    if(event_num == event_list_.size()){
      event_list_.resize(event_list_.size()*2);
    }
  }else if(event_num == 0){
    LOG_INFO<<" nothing happended";
  }else{
    LOG_ERROR<<"Epoller::epoll()";
  }
  return now;
}

void Epoller::fillActiveChannels(int numEvents,ChannelList* activeChannels) const{
  assert(numEvents <= event_list_.size());
  for(int i=0;i<numEvents;i++){
    auto channel = static_cast<Channel*>(event_list_[i].data.ptr);
    channel->setRevents(event_list_[i].events);
    activeChannels->push_back(channel);
  }
}

void Epoller::addChannel(Channel* channel){
  assertInLoopThread();
  auto status = channel->getStatus();
  int fd = channel->getFd();
  assert(channel_map_.find(fd) == channel_map_.end());
  assert(status = Channel::NEW);
  channel_map_[fd] = channel;
  channel->setStatus(Channel::ADDED);
  update(EPOLL_CTL_ADD, channel);
}


void Epoller::removeChannel(Channel* channel){
  assertInLoopThread();
  auto status = channel->getStatus();
  int fd = channel->getFd();
  assert(channel_map_.find(fd) != channel_map_.end());
  assert(channel_map_[fd] == channel);
  assert(status = Channel::ADDED);
  size_t n = channel_map_.erase(fd);
  update(EPOLL_CTL_DEL, channel);
}


void Epoller::updateChannel(Channel* channel){
  assertInLoopThread();
  auto status = channel->getStatus();
  int fd = channel->getFd();
  assert(channel_map_.find(fd) != channel_map_.end());
  assert(channel_map_[fd] == channel);
  assert(status = Channel::ADDED);
  update(EPOLL_CTL_MOD, channel);
}

void Epoller::update(int operation, Channel* channel){
  struct epoll_event event;
  bzero(&event, sizeof event);
  event.events = channel->getEvent();
  event.data.ptr = channel;
  int fd = channel->getFd();
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    if (operation == EPOLL_CTL_DEL) {
      LOG_ERROR << "epoll_ctl op=" << operation << " fd=" << fd;
    } else{
      LOG_FATAL << "epoll_ctl op=" << operation << " fd=" << fd;
    }
  }
}

}