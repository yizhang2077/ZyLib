#ifndef THREAD_UTIL_H_
#define THREAD_UTIL_H_
#include <sstream>
#include <thread>
#include <string>

namespace ZyLib{
  std::string ThreadId2Int(const std::thread::id& id);
}

#endif