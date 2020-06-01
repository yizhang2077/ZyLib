#include "async_queue.h"
#include "logger.h"
#include <memory>
#include <chrono>
#include <iostream>

FILE* fp;

void* g_asyncLog = NULL;

template<typename ASYNC>
void asyncOutput(const char* msg, int len)
{
  ASYNC* log = static_cast<ASYNC*>(g_asyncLog);
  log->Append(msg, len);
}


template<typename ASYNC>
void benchmark(ASYNC* log){
  g_asyncLog = log;
  ZyLib::Logger::SetOutputFunc(asyncOutput<ASYNC>);
  log->Start();
  constexpr int batch = 1000;

  int cnt = 0;
  const bool kLongLog = true;
  std::string empty = " ";
  std::string longStr(500, 'X');
  longStr += " ";


  std::cout <<"start log..." << std::endl;
  for(int t=0;t<30;t++){
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < batch; ++i){
      LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
              << (kLongLog ? longStr : empty)
              << cnt;
      ++cnt;
    }
    auto end = std::chrono::system_clock::now();
    auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>( end-start ).count();
    std::cout  <<"consume " << miliseconds/1000 <<"s, " << batch / miliseconds * 1000<<" msg/s " <<std::endl;
  }
  std::cout<<"end log"<<std::endl;
}
int main(int argc, char* argv[]){
  int which = argc > 1 ? atoi(argv[1]) : 1;
  int kRollSize = 500*1000*1000;
  ZyLib::AsyncLogUnboundedQueue  log1("log1", kRollSize);
  ZyLib::AsyncLogBoundedQueue    log2("log2", kRollSize, 1024);
  ZyLib::AsyncLogUnboundedQueueL log3("log3", kRollSize);
  ZyLib::AsyncLogBoundedQueueL   log4("log4", kRollSize, 1024);
  switch(which){
    case 1:
      benchmark(&log1);
      break;
    case 2:
      benchmark(&log2);
      break;
    case 3:
      benchmark(&log3);
      break;
    case 4:
      benchmark(&log4);
      break;
  }
  
}