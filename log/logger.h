#ifndef LOG_LOGGER_H_
#define LOG_LOGGER_H_

#include "logstream.h"

#include <functional>

namespace ZyLib{
class Logger{
  public:
    enum LogLevel{
      DEBUG,      
      INFO,       
      WARN,       
      ERROR,
      FATAL,
      NUM_LOG_LEVELS,
    };

    Logger(const char* filename,int line, LogLevel level,const char* func_name);
    ~Logger();
    
    typedef std::function<void(const char* msg,size_t len)> OutputFunc;
    typedef std::function<void()> FlushFunc;
    
    static LogLevel GetGlobalLogLevel(){ return GlobalLoggerLevel;}
    
    static void SetOutputFunc(OutputFunc func);
    static void SetFlushFunc(FlushFunc func);
    static void SetGlobalLevel(LogLevel loglevel);
    
    LogStream& GetLogStream(){return stream_;}

  private:
    int line_;
    std::string filename_;
    std::string basename_;
    LogLevel level_;
    LogStream stream_;

    static OutputFunc GlobalOutputFunc;
    static FlushFunc GlobalFlushFunc;
    static LogLevel GlobalLoggerLevel;
};

#define LOG_DEBUG if (ZyLib::Logger::GetGlobalLogLevel() == ZyLib::Logger::DEBUG)\
 ZyLib::Logger(__FILE__, __LINE__, ZyLib::Logger::DEBUG, __func__).GetLogStream()
#define LOG_INFO  if (ZyLib::Logger::GetGlobalLogLevel() <= ZyLib::Logger::INFO)\
  ZyLib::Logger(__FILE__, __LINE__, ZyLib::Logger::INFO, __func__).GetLogStream()
#define LOG_WARN  ZyLib::Logger(__FILE__, __LINE__, ZyLib::Logger::WARN, __func__).GetLogStream()
#define LOG_ERROR ZyLib::Logger(__FILE__, __LINE__, ZyLib::Logger::ERROR, __func__).GetLogStream()
#define LOG_FATAL ZyLib::Logger(__FILE__, __LINE__, ZyLib::Logger::FATAL, __func__).GetLogStream()

}
#endif