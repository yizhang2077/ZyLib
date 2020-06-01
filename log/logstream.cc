#include "logstream.h"

#include <algorithm>

namespace ZyLib{

namespace{
  template<typename T>
  size_t Convert(char buf[], T value)
  {
      static const char digits[] = "9876543210123456789";
      static const char* zero = digits + 9;
      T i = value;
      char* p = buf;

      do
      {
          int lsd = static_cast<int>(i % 10);
          i /= 10;
          *p++ = zero[lsd];
      } while (i != 0);

      if (value < 0)
      {
          *p++ = '-';
      }
      *p = '\0';
      std::reverse(buf, p);

      return p - buf;
  }
}

template <typename T>
void LogStream::formatInteger(T val){
  if(buffer_.getAvailable()>=MAX_NUMERIC_SIZE){
    size_t len = Convert(buffer_.current(),val);
    buffer_.Lseek(len);
  }
}

LogStream& LogStream::operator<<(bool val){
  buffer_.Append(val?"1":"0",1);
  return *this;
}

LogStream& LogStream::operator<<(short val){
  *this << static_cast<int>(val);
  return *this;
}

LogStream& LogStream::operator<<(int val){
  formatInteger(val);
  return *this;
}

LogStream& LogStream::operator<<(long val){
  formatInteger(val);
  return *this;
}

LogStream& LogStream::operator<<(long long val){
  formatInteger(val);
  return *this;
}

LogStream& LogStream::operator<<(float val){
  *this << static_cast<double>(val);
  return *this;
}
    
LogStream& LogStream::operator<<(double val){
  if(buffer_.getAvailable() >= MAX_NUMERIC_SIZE){
    int len = snprintf(buffer_.current(),MAX_NUMERIC_SIZE,"%.12g",val);
    buffer_.Lseek(len);
  }
  return *this;
}
  
LogStream& LogStream::operator<<(char ch){
  buffer_.Append(&ch,1);
  return *this;
}

LogStream& LogStream::operator<<(const char* str){
  if(str){
    buffer_.Append(str,strlen(str));
  }else{
    buffer_.Append("(null)",6);
  }
  return *this;
}

LogStream& LogStream::operator<<(const std::string& str){
  buffer_.Append(str.c_str(),str.length());
  return *this;
}

LogStream& LogStream::operator<<(const Fmt& fmt){
  buffer_.Append(fmt.data(),fmt.length());
  return *this;
}

}