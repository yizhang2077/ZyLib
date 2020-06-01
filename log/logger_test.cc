#include "logger.h"
#include "logfile.h"
#include <memory>
#include <chrono>
#include <iostream>

FILE* fp;
std::shared_ptr<ZyLib::LogFile> log_file;
long total_len = 0;

void WriteOnstdout(const char* msg,size_t len){
  total_len+=len;
  fwrite(msg,1,len,stdout);
}

void WriteDirect(const char* msg,size_t len){
  total_len+=len;
  if(fp){
    fwrite(msg,1,len,fp);
  }
}

void WriteWithLogFile(const char* msg,size_t len){
  total_len+=len;
  if(log_file){
    log_file->Append(msg,len);
  }
}

void benchmark(){
  constexpr int batch = 1000000;
  total_len = 0;
  auto start = std::chrono::system_clock::now();
  std::cout <<"start log..." << std::endl;
  for(int i=0; i<batch;i++){
    LOG_INFO<< i<<" hello world "<<"1234567890"
            <<"abcdefghijklmnopqrstuvwxyz";
  }
  auto end = std::chrono::system_clock::now();
  std::cout<<"end log"<<std::endl;
  auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>( end-start ).count();
  std::cout  <<"consume " << miliseconds/1000 <<"s, "<< total_len<< " bytes " 
            << batch / miliseconds * 1000<<" msg/s " 
            << total_len / miliseconds * 1000 / 1024 / 1024<<"MiB/s" <<std::endl;
}
int main(){
  LOG_INFO<< "this is a test";
  LOG_DEBUG << "debug";
  LOG_INFO << "Hello";
  LOG_WARN << "World";
  LOG_ERROR << "Error";

  char buffer[64*1024];

  fp = fopen("/dev/null", "w");
  setbuffer(fp, buffer, sizeof(buffer));
  ZyLib::Logger::SetOutputFunc(WriteDirect);
  ZyLib::Logger::SetFlushFunc([&]{fflush(fp);});
  std::cout<<"*******log to /dev/null*******"<<std::endl;
  benchmark();
  fclose(fp);

  fp = fopen("/tmp/log", "w");
  setbuffer(fp, buffer, sizeof buffer);
  ZyLib::Logger::SetOutputFunc(WriteDirect);
  ZyLib::Logger::SetFlushFunc([&]{fflush(fp);});
  std::cout<<"*******log to /tmp/log*******"<<std::endl;
  benchmark();
  fclose(fp);
  
}