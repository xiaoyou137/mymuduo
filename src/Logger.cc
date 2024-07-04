#include <iostream>

#include "Logger.h"
#include "Timestamp.h"

// 获取单例对象
Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

// 设置日志级别
void Logger::setLogLevel(int level)
{
    logLevel_ = level;
}

// 输出日志
void Logger::log(std::string msg)
{

    std::cout << Timestamp::now().ToString() << " " << msg << std::endl;
    
}