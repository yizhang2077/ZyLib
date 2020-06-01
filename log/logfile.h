#ifndef LOG_LOG_FILE_H_
#define LOG_LOG_FILE_H_

#include "file.h"

#include <stdio.h>
#include <string>
#include <memory>
#include <mutex>
#include <boost/noncopyable.hpp>

namespace ZyLib{

class LogFile : boost::noncopyable{
  public:
    LogFile(const std::string& base_name,size_t rollSize,int flushInterval,bool threadSafe=false);

    ~LogFile();
    
    void Append(const char* str,size_t len);

    void Flush();

  private:
    void rollFile();
    
    std::string getLogFilename(time_t* now) const;
    void Append_unlocked(const char* str,size_t len);
    
    bool threadSafe_ = false;
    std::mutex mutex_;
    time_t startOfRoll_ = 0;
    time_t lastOfRoll_ = 0;
    time_t lastFlush_ = 0;

    const std::string base_name_;
    const size_t rollSize_; // if the log file size is larger than roll size, roll a new file
    const int flushInterval_;
    const int kCheckTimeout_ = 1024;  
    const int kRollInterval_ = 60*60*24; // roll a new file if the time interval is larger than this var

    int check_timeout_count_ = 0;
    std::shared_ptr<FileUtil> file_ptr_;
};
}
#endif