#ifndef LOGGER_H
#define LOGGER_H

#include "noncopyable.h"

#include <string>
#include <iostream>

/**
 * @brief 定义LOG宏
 */

#define LOG_INFO(logmsgfmt, ...)                                                 \
    do                                                                           \
    {                                                                            \
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "; \
        Logger &logger = Logger::instance();                                     \
        logger.setLogLevel(INFO);                                                \
        char buf[1024] = {0};                                                    \
        snprintf(buf, 1024, logmsgfmt, ##__VA_ARGS__);                           \
        logger.log(buf);                                                         \
    } while (0)

#define LOG_ERROR(logmsgfmt, ...)                                                \
    do                                                                           \
    {                                                                            \
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "; \
        Logger &logger = Logger::instance();                                     \
        logger.setLogLevel(ERROR);                                               \
        char buf[1024] = {0};                                                    \
        snprintf(buf, 1024, logmsgfmt, ##__VA_ARGS__);                           \
        logger.log(buf);                                                         \
    } while (0)

#define LOG_FATAL(logmsgfmt, ...)                                                \
    do                                                                           \
    {                                                                            \
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "; \
        Logger &logger = Logger::instance();                                     \
        logger.setLogLevel(FATAL);                                               \
        char buf[1024] = {0};                                                    \
        snprintf(buf, 1024, logmsgfmt, ##__VA_ARGS__);                           \
        logger.log(buf);                                                         \
        exit(-1);                                                                \
    } while (0)

#ifdef MUDEBUG
#define LOG_DEBUG(logmsgfmt, ...)                                                \
    do                                                                           \
    {                                                                            \
        std::cout << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": "; \
        Logger &logger = Logger::instance();                                     \
        logger.setLogLevel(DEBUG);                                               \
        char buf[1024] = {0};                                                    \
        snprintf(buf, 1024, logmsgfmt, ##__VA_ARGS__);                           \
        logger.log(buf);                                                         \
    } while (0)
#else
#define LOG_DEBUG(logmsgfmt, ...)
#endif

/**
 * @brief 定义日志的级别
 */
enum LogLevel
{
    INFO,  // 普通信息
    ERROR, // 错误信息
    FATAL, // core信息
    DEBUG  // debug信息
};

/**
 * @brief 输出一个日志类
 */
class Logger : noncopyable
{
public:
    // 获取单例对象
    static Logger &instance();

    // 设置日志级别
    void setLogLevel(int level);

    // 输出日志
    void log(std::string msg);

private:
    // 构造私有化
    Logger() {}

    int logLevel_;
};

#endif
