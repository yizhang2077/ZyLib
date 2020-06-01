#include "logger.h"

#include <chrono>
#include <sstream>
#include <iomanip>
#include <sys/time.h>

namespace ZyLib{

namespace{
const int kMicroSecondsPerSecond = 1000*1000;
const char* loglevel_name[Logger::NUM_LOG_LEVELS] =
{
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

Logger::LogLevel InitLoggerLevel(){
  if (getenv("LOG_DEBUG"))
    return Logger::DEBUG;
  else
    return Logger::INFO;
}

void DefaultOutput(const char* msg, int len)
{
    fwrite(msg, 1, len, stdout);
}

void DefaultFlush()
{
    fflush(stdout);
}

std::string FormatTime(){
  std::stringstream filename_ss;
  char timebuff[32];
  struct timeval tv;
  gettimeofday(&tv, NULL);

  time_t seconds = static_cast<time_t>(tv.tv_sec);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  snprintf(timebuff, sizeof(timebuff), "%4d%02d%02d %02d:%02d:%02d",
      tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
      tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  filename_ss << timebuff;
  return filename_ss.str();
}

}

Logger::OutputFunc Logger::GlobalOutputFunc = Logger::OutputFunc(DefaultOutput);
Logger::FlushFunc Logger::GlobalFlushFunc = Logger::FlushFunc(DefaultFlush);
Logger::LogLevel Logger::GlobalLoggerLevel = InitLoggerLevel();

void Logger::SetOutputFunc(Logger::OutputFunc out)
{
  GlobalOutputFunc = out;
}

void Logger::SetFlushFunc(Logger::FlushFunc flush)
{
  GlobalFlushFunc = flush;
}

void Logger::SetGlobalLevel(Logger::LogLevel level)
{
  GlobalLoggerLevel = level;
}

Logger::Logger(const char* file_name, int line, LogLevel level, const char* func_name)
    : stream_(),
      level_(level),
      filename_(file_name),
      line_(line)
{
    const char* path_sep_pos = strrchr(filename_.c_str(), '/');
    basename_ = (path_sep_pos != NULL) ? path_sep_pos + 1 : filename_;
    std::string time_str = FormatTime();
    stream_ << loglevel_name[level_] <<" "<<time_str<<" "<< func_name << "():";
}

// destructor flush the buffer and judge fatal
Logger::~Logger()
{
    stream_ << " - " << basename_ << ':' << line_ << '\n';

    GlobalOutputFunc(stream_.getBuffer().data(), stream_.getBuffer().length());
    // when fatal abort func
    if (level_ == FATAL)
    {
        GlobalFlushFunc();
        abort();
    }
}

}