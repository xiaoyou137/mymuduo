#ifndef INETADDRESS_H
#define INETADDRESS_H

#include "copyable.h"

#include <netinet/in.h>
#include <string>

/**
 * @brief 封装ip port的类
 */
class InetAddress : public copyable
{
public:
    explicit InetAddress(uint16_t port = 0, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in addr)
        : addr_(addr)
    {
    }

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;

    const sockaddr_in *getSockAddr() const { return &addr_; }
    void setSockAddr(const sockaddr_in &addr) { addr_ = addr; }

private:
    sockaddr_in addr_; // 地址结构
};

#endif
