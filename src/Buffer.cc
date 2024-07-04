#include "Buffer.h"

#include <sys/uio.h>
#include <unistd.h>

/**
* @brief 从fd上读数据，poller工作在LT模式
*       Buffer缓冲区是有大小的，但从TCP缓冲区读数据时，却不知道数据最终大小        
*/
ssize_t Buffer::readFd(int fd, int *saveErrno)
{
    char extrabuf[65535] = {0}; // 栈上的内存空间，64K大小
    size_t writeable = writeableBytes();

    struct iovec vec[2];
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writeable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writeable < sizeof extrabuf) ? 2 : 1;
    ssize_t n = ::readv(fd, vec, iovcnt);

    if(n < 0)
    {
        *saveErrno = errno;
    } 
    else if(n <= writeable)
    {
        writeIndex_ += n;
    }
    else
    {
        writeIndex_ = buffer_.size();
        append(extrabuf, n - writeable);
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int *saveErrno)
{
   ssize_t n = ::write(fd, peek(), readableBytes());
   if(n < 0)
   {
    *saveErrno = errno;
   }
   return n;
}
