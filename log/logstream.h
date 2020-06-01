#ifndef LOG_LOG_STREAM_H_
#define LOG_LOG_STREAM_H_

#include "util.h"

#include <string.h>
#include <boost/noncopyable.hpp>
#include <string>

namespace ZyLib{

const int SMALL_BUFFER_SIZE = 1024;
const int LARGE_BUFFER_SIZE = 1024 * 1000;

class LogStream: boost::noncopyable{
  public:
    LogStream& operator<<(bool);
    LogStream& operator<<(short);
    LogStream& operator<<(int);
    LogStream& operator<<(long);
    LogStream& operator<<(long long);
    LogStream& operator<<(float);
    LogStream& operator<<(double);
    LogStream& operator<<(char);
    LogStream& operator<<(const char*);
    LogStream& operator<<(const std::string&);
    LogStream& operator<<(const Fmt& fmt);

    void Append(const char* data , size_t len){buffer_.Append(data,len);}
    void Reset(){buffer_.Reset();}
    const FixedBuffer<SMALL_BUFFER_SIZE>& getBuffer() const{ return buffer_;}
  private:    
    template<typename T>
    void formatInteger(T);
    FixedBuffer<SMALL_BUFFER_SIZE> buffer_;
    static const int MAX_NUMERIC_SIZE = 32;
};
}
#endif