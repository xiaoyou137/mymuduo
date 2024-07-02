#ifndef COPYABLE_H
#define COPYABLE_H

/**
* @brief 其派生类可以被拷贝构造和拷贝赋值
*/
class copyable 
{
protected:
    copyable() = default;
    ~copyable() = default;
};

#endif
