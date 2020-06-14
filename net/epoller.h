#ifndef NET_EPOLLER_H_
#define NER_EPOLLER_H_

#include "common/timestamp.h"
#include "net/eventloop.h"

#include <vector>
#include <map>
#include <boost/noncopyable.hpp>
#include <sys/epoll.h>

namespace ZyLib{

class Channel;

class Epoller : boost::noncopyable{
  public:
    typedef std::vector<Channel*> ChannelList;
    Epoller(EventLoop* ownerloop);
    ~Epoller();
    
    Timestamp Poll(int timeout_ms,ChannelList* channel_list);
    
    void addChannel(Channel* channel);
    void removeChannel(Channel* channel);
    void updateChannel(Channel* channel);
    
    void assertInLoopThread() { ownerloop_->assertInLoopThread(); }
  private:
    void update(int operation,Channel* update_channel);
    void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;
    typedef std::vector<struct epoll_event> EventList;
    typedef std::map<int, Channel*> ChannelMap;
    int epollfd_;
    EventLoop* ownerloop_;
    EventList event_list_;
    ChannelMap channel_map_;
};

}
#endif