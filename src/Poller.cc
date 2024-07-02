#include "Poller.h"

Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop)
{
}

bool Poller::hasChannel(Channel *channel) const
{
    auto it = channelMap_.find(channel->fd());
    return it != channelMap_.end() && it->second == channel;
}
