#include "file.h"

#include <stdio.h>
#include <string.h>

namespace ZyLib{

FileUtil::FileUtil(const std::string& file_name){
  if((fp_ = fopen(file_name.c_str(),"ae")) == nullptr){
     printf("log file open failed: errno = %d reason = %s \n", errno, strerror(errno));
  }
  setbuffer(fp_,buffer_,sizeof(buffer_));
}

FileUtil::~FileUtil(){
  fclose(fp_);
}

void FileUtil::Append(const char* str,const size_t len){
  int remain = len;
  int offset = 0;
  while(remain>0){
    size_t tmp = Write(str+offset,len);
    if(tmp == 0){
      int err = ferror(fp_);
      if (err){
          fprintf(stderr, "File::Append() failed !\n");
      }
      break;
    }
    offset += tmp;
    remain -= tmp;
  }
  written_bytes_ += len;
}

void FileUtil::Flush(){
  if(fp_!=nullptr){
    fflush(fp_);
  }
}

size_t FileUtil::Write(const char* str,const size_t len){
  if(fp_!=nullptr){
    return fwrite_unlocked(str,sizeof(char),len,fp_);
  }
  return 0;
}

}