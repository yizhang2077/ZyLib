#include <sstream>
#include <thread>
#include <string>

namespace ZyLib{
  std::string ThreadId2Int(const std::thread::id& id){
    std::stringstream sin;
    sin << id;
    return sin.str();
  }
}