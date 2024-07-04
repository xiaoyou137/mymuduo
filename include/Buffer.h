#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <algorithm>
#include <string>

/**
 * @brief 网络库底层的缓冲区定义
 */
class Buffer
{
public:
    static const int kCheapPrepend = 8;
    static const int kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize), readIndex_(kCheapPrepend), writeIndex_(kCheapPrepend)
    {
    }

    size_t readableBytes() const 
    {
        return writeIndex_ - readIndex_;
    }

    size_t writeableBytes() const
    {
        return buffer_.size() - writeIndex_;
    }

    size_t prependableBytes() const
    {
        return readIndex_;
    }

    // 返回缓冲区中的可读起始地址
    const char* peek() const
    {
        return begin() + readIndex_;
    }

    void retrieve(size_t len)
    {
        if(len < readableBytes())
        {
            readIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveAll()
    {
        readIndex_ = writeIndex_ = kCheapPrepend;
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len)
    {
        std::string res(peek(), len);
        retrieve(len);
        return res;
    }

    void ensureWriteableBytes(size_t len)
    {
        if(writeableBytes() < len)
        {
            makeSpace(len);
        }
    }

    char* beginWrite()
    {
        return begin() + writeIndex_;
    }

    const char* beginWrite() const
    {
        return begin() + writeIndex_;
    }

    // 把[data, data+len]上的数据，拷贝到writeable缓冲区中
    void append(const char* data, size_t len)
    {
        ensureWriteableBytes(len);
        std::copy(data, data+len, beginWrite());
        writeIndex_ += len;
    }

    // 从fd上读取数据到writable缓冲区
    ssize_t readFd(int fd, int* saveErrno);

    // 从readable缓冲区向fd上写入数据
    ssize_t writeFd(int fd, int* saveErrno);



private:
    char *begin()
    {
        return &*buffer_.begin(); // vector底层数组的首元素地址，即vector底层数组的地址
    }

    const char *begin() const
    {
        return &*buffer_.begin();
    }

    void makeSpace(size_t len)
    {
        if(writeableBytes() + prependableBytes() < len + writeIndex_)
        {
            buffer_.resize(len + writeIndex_);
        }
        else
        {
            size_t readable = readableBytes();
            std::copy(begin() + readIndex_, begin() + writeIndex_, begin() + kCheapPrepend);
            readIndex_ = kCheapPrepend;
            writeIndex_ = kCheapPrepend + readable;
        }
    }

    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;
};

#endif
