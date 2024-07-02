#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

/**
 * @brief noncopyable被继承以后，其派生类可以构造和析构，但不能拷贝构造和拷贝赋值。
 */
class noncopyable 
{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif
