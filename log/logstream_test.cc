#include "logstream.h"

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <string>

BOOST_AUTO_TEST_CASE(test_logstream_bool){
  ZyLib::LogStream os;
  const auto& buffer = os.getBuffer();
  BOOST_CHECK_EQUAL(buffer.asString(),"");
  os<<true;
  BOOST_CHECK_EQUAL(buffer.asString(),"1");
  os<<false;
  BOOST_CHECK_EQUAL(buffer.asString(),"10");
}

BOOST_AUTO_TEST_CASE(test_logstream_int){
  ZyLib::LogStream os;
  const auto& buffer = os.getBuffer();
  BOOST_CHECK_EQUAL(buffer.asString(),"");
  os<<123;
  BOOST_CHECK_EQUAL(buffer.asString(),"123");
  os<<456;
  BOOST_CHECK_EQUAL(buffer.asString(),"123456");
  os<<-1;
  BOOST_CHECK_EQUAL(buffer.asString(),"123456-1");
  os.Reset();
  os<<123<<" "<<456<<"x"<<0x64;
  BOOST_CHECK_EQUAL(buffer.asString(),"123 456x100");
}


BOOST_AUTO_TEST_CASE(test_logstream_floats)
{
  ZyLib::LogStream os;
  const auto& buf = os.getBuffer();

  os << 0.0;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0"));
  os.Reset();

  os << 1.0;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("1"));
  os.Reset();

  os << 0.1;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0.1"));
  os.Reset();

  os << 0.05;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0.05"));
  os.Reset();

  os << 0.15;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0.15"));
  os.Reset();

  double a = 0.1;
  os << a;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0.1"));
  os.Reset();

  double b = 0.05;
  os << b;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0.05"));
  os.Reset();

  double c = 0.15;
  os << c;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0.15"));
  os.Reset();

  os << a+b;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("0.15"));
  os.Reset();

  BOOST_CHECK(a+b != c);

  os << 1.23456789;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("1.23456789"));
  os.Reset();

  os << 1.234567;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("1.234567"));
  os.Reset();

  os << -123.456;
  BOOST_CHECK_EQUAL(buf.asString(), std::string("-123.456"));
  os.Reset();
}

BOOST_AUTO_TEST_CASE(testLogStreamFmts)
{
  ZyLib::LogStream os;
  const auto& buf = os.getBuffer();

  os << ZyLib::Fmt("%4d", 1);
  BOOST_CHECK_EQUAL(buf.asString(), std::string("   1"));
  os.Reset();

  os << ZyLib::Fmt("%4.2f", 1.2);
  BOOST_CHECK_EQUAL(buf.asString(), std::string("1.20"));
  os.Reset();

  os << ZyLib::Fmt("%4.2f", 1.2) << ZyLib::Fmt("%4d", 43);
  BOOST_CHECK_EQUAL(buf.asString(), std::string("1.20  43"));
  os.Reset();
}