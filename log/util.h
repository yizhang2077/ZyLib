#ifndef LOG_UTIL_H_
#define LOG_UTIL_H_
#include <stdio.h>
#include <string.h>
#include <string>

namespace ZyLib{

template<int SIZE>
class FixedBuffer{
  public:
    FixedBuffer():cur_(data_){}
    
    void Append(const char *str,size_t len){
      if(getAvailable() > len){
        memcpy(cur_, str , len);
        cur_ += len;
      }
    }

    void Lseek(size_t len){
      cur_+=len;
    }

    void Clean(){
      bzero(data_,sizeof(data_));
    }
    void Reset(){
      cur_ = data_;
    }
    
    const char* data() const { return data_;}

    char* current() {return cur_;}

    int length() const { return static_cast<int>(cur_ - data_); }
    
    int getAvailable() const { return static_cast<int>(getEnd() - cur_); }

    std::string asString() const { return std::string(data_,cur_ - data_);}

  private:
    
    const char* getEnd() const { return data_ + sizeof(data_); }
    
    char data_[SIZE];
    char* cur_;
};

class Fmt{
  public:
    template <typename T>
    explicit Fmt(const char* fmt,T val){
      length_ = snprintf(buffer_,sizeof(buffer_),fmt,val);
    }
    const char* data() const { return buffer_;}
    size_t length() const { return length_;}
  private:
    char buffer_[64];
    size_t length_ = 0;
};


template Fmt::Fmt(const char* fmt, char);

template Fmt::Fmt(const char* fmt, short);
template Fmt::Fmt(const char* fmt, unsigned short);
template Fmt::Fmt(const char* fmt, int);
template Fmt::Fmt(const char* fmt, unsigned int);
template Fmt::Fmt(const char* fmt, long);
template Fmt::Fmt(const char* fmt, unsigned long);
template Fmt::Fmt(const char* fmt, long long);
template Fmt::Fmt(const char* fmt, unsigned long long);

template Fmt::Fmt(const char* fmt, float);
template Fmt::Fmt(const char* fmt, double);

}
#endif