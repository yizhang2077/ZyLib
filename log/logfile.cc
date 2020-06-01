#include "logfile.h"

#include <sys/stat.h>
#include <assert.h>
#include <iomanip>
#include <sstream>

namespace ZyLib{

LogFile::LogFile(const std::string& base_name,size_t rollSize,int flushInterval,bool threadSafe)
  :base_name_(base_name),
  rollSize_(rollSize),
  flushInterval_(flushInterval),
  threadSafe_(threadSafe){
    assert(base_name.find('/') == std::string::npos);
    rollFile();
}

LogFile::~LogFile(){
  Flush();
}

void LogFile::Append(const char* str,size_t len){
  if(threadSafe_){
    std::lock_guard<std::mutex> lock_guard(mutex_);
    Append_unlocked(str,len);
  }else{
    Append_unlocked(str,len);
  }
}

void LogFile::Append_unlocked(const char* str,size_t len){
  file_ptr_->Append(str,len);
  if(file_ptr_->getWrittenBytes()>rollSize_){
    rollFile();
  }else{
    if(check_timeout_count_>kCheckTimeout_){
      check_timeout_count_ = 0;
      time_t now = time(NULL);
      if(now - startOfRoll_ > kRollInterval_){
        rollFile();
      } else if(now - lastFlush_ > flushInterval_){
        lastFlush_ = now;
        file_ptr_->Flush();
      }
    }else{
      check_timeout_count_++;
    }
  }
}

void LogFile::Flush(){
  if(threadSafe_){
    std::lock_guard<std::mutex> lock_guard(mutex_);
    file_ptr_->Flush();
  }else{
    file_ptr_->Flush();
  }
}

void LogFile::rollFile(){
  time_t now = 0;
  std::string filename = getLogFilename(&now);
  time_t start = now / kRollInterval_ * kRollInterval_;
  lastFlush_ = now;
  startOfRoll_ = start;
  file_ptr_.reset(new FileUtil(filename));
}

std::string LogFile::getLogFilename(time_t* now) const{
  std::stringstream filename_ss;
  filename_ss << base_name_;  
  char timebuff[32];
  *now = time(NULL);
  filename_ss << std::put_time(std::localtime(now),".%Y%m%d-%H%M%S") << ".log";
  return filename_ss.str();
}

}
