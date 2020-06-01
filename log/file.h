#ifndef LOG_FILE_H_
#define LOG_FILE_H_

#include <string>

namespace ZyLib{
class FileUtil{
  public:
  explicit FileUtil(const std::string& file_name);
  
  ~FileUtil();
  
  void Append(const char* str,const size_t len);

  void Flush();

  size_t getWrittenBytes() const{return written_bytes_;}
  private:
    size_t Write(const char* str,const size_t len);
    size_t written_bytes_ = 0;
    FILE* fp_;
    char buffer_[1024*64];
};
}
#endif