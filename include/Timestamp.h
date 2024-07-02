#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "copyable.h"

#include <stdint.h>
#include <string>

/**
* @brief UTC时间，精度为微秒
*/
class Timestamp : public copyable
{
public:
    Timestamp();
    explicit Timestamp(int64_t microsenconds_since_epoch);

    static Timestamp now();
    std::string ToString() const;
private:
    int64_t microsenconds_since_epoch_;
};

#endif
