g++-7 logger_test.cc logger.cc logfile.cc logstream.cc file.cc util.h -o logger_test -std=c++11

g++-7 async_queue_test.cc logstream.cc logger.cc logfile.cc file.cc util.h -I.. -lpthread -O0 -o async_queue_test -std=c++11
