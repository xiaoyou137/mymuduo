#include "Timestamp.h"

#include <time.h>

Timestamp::Timestamp() : microsenconds_since_epoch_(0)
{
}

Timestamp::Timestamp(int64_t microsenconds_since_epoch) : microsenconds_since_epoch_(microsenconds_since_epoch)
{
}

Timestamp Timestamp::now()
{
    return Timestamp(time(nullptr));
}

std::string Timestamp::ToString() const
{
    tm* tm_local = localtime(&microsenconds_since_epoch_);
    char buf[128] = {0};
    snprintf(buf, 128, "%4d/%02d/%02d %02d:%02d:%02d",
        tm_local->tm_year + 1900,
        tm_local->tm_mon + 1,
        tm_local->tm_mday,
        tm_local->tm_hour,
        tm_local->tm_min,
        tm_local->tm_sec);

    return buf;
}
