#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "libiqxmlrpc/value.h"
#include "libiqxmlrpc/value_parser.h"
#include "libiqxmlrpc/request_parser.h"
#include "libiqxmlrpc/response_parser.h"

using namespace boost::unit_test;
using namespace iqxmlrpc;

//
// values
//

Value parse_value(const std::string& s)
{
  Parser p(s);
  ValueBuilder b(p);
  b.build();
  return Value(b.result());
}

void test_parse_scalar()
{
  BOOST_CHECK_EQUAL(parse_value("<int>123</int>").get_int(), 123);
  BOOST_CHECK_EQUAL(parse_value("<i4>123</i4>").get_int(), 123);
  BOOST_CHECK_EQUAL(parse_value("<boolean>0</boolean>").get_bool(), false);
  BOOST_CHECK_EQUAL(parse_value("<double>123.45</double>").get_double(), 123.45);
  BOOST_CHECK_EQUAL(parse_value("<string>str</string>").get_string(), "str");
  BOOST_CHECK_EQUAL(parse_value("<base64>V2h5IHNob3VsZCBJIGJsYW1lIGhlcg==</base64>").get_binary().get_data(), "Why should I blame her");
  BOOST_CHECK_EQUAL(parse_value("<dateTime.iso8601>19980717T14:08:55</dateTime.iso8601>").get_datetime().to_string(), "19980717T14:08:55");
  BOOST_CHECK(parse_value("<nil/>").is_nil());
}

void test_parse_array()
{
  Array v = parse_value(
    "<array>"
      "<data>"
      "<value><i4>123</i4></value>"
      "<value><double>123.456</double></value>"
      "<value><string>str</string></value>"
      "<value>str2</value>"
      "<value><boolean>1</boolean></value>"
      "<value>"
          "<struct>"
          "<member><name>v1</name><value>str</value></member>"
          "<member><name>v2</name><value><int>123</int></value></member>"
          "</struct>"
      "</value>"
      "</data>"
    "</array>").the_array();

  BOOST_CHECK_EQUAL(v.size(), 6);
  BOOST_CHECK_EQUAL(v[0].get_int(), 123);
  BOOST_CHECK_EQUAL(v[1].get_double(), 123.456);
  BOOST_CHECK_EQUAL(v[2].get_string(), "str");
  BOOST_CHECK_EQUAL(v[3].get_string(), "str2");
  BOOST_CHECK_EQUAL(v[4].get_bool(), true);
  BOOST_CHECK(v[5].is_struct());
  BOOST_CHECK_EQUAL(v[5].the_struct().size(), 2);
  BOOST_CHECK_EQUAL(v[5].the_struct()["v2"].get_int(), 123);
}

void test_parse_unknown_type()
{
  BOOST_CHECK_THROW(parse_value("<abc>0</abc>"), XML_RPC_violation);
}

void test_parse_bad_xml()
{
  BOOST_CHECK_THROW(parse_value("not valid <xml>"), Parse_error);
  BOOST_CHECK_THROW(parse_value("<doc></abc></doc>"), Parse_error);
}

void test_parse_simple_struct()
{
  Struct s = parse_value(
    "<struct>"
      "<member><name>v1</name><value>str</value></member>"
      "<member><name>v2</name><value><int>123</int></value></member>"
      "<member><name>v3</name><value><string>str2</string></value></member>"
      "<member><name/><value><string>str2</string></value></member>"
    "</struct>").the_struct();

  BOOST_CHECK_EQUAL(s.size(), 4);
  BOOST_CHECK_EQUAL(s["v1"].get_string(), "str");
  BOOST_CHECK_EQUAL(s["v2"].get_int(), 123);
  BOOST_CHECK_EQUAL(s["v3"].get_string(), "str2");
  BOOST_CHECK_EQUAL(s[""].get_string(), "str2");
}

void test_parse_nested_struct()
{
  Struct s = parse_value(
    "<struct>"
      "<member>"
        "<name>nested</name>"
        "<value>"
          "<struct>"
            "<member><name>v1</name><value>str</value></member>"
            "<member><name>v2</name><value><int>123</int></value></member>"
            "<member><name>v3</name><value><string>str2</string></value></member>"
          "</struct>"
        "</value>"
      "</member>"
      "</struct>").the_struct();

  BOOST_CHECK_EQUAL(s.size(), 1);
  BOOST_CHECK(s["nested"].is_struct());
  Struct s2 = s["nested"].the_struct();
  BOOST_CHECK_EQUAL(s2["v1"].get_string(), "str");
  BOOST_CHECK_EQUAL(s2["v2"].get_int(), 123);
  BOOST_CHECK_EQUAL(s2["v3"].get_string(), "str2");
}

void test_parse_formatted()
{
  Value v = parse_value(" \
<struct> \
  <member> \
    <name>name</name> \
    <value> \
      <int>123</int> \
    </value> \
  </member> \
</struct>");

  BOOST_CHECK(v.is_struct());
}

void test_parse_emptiness()
{
  BOOST_CHECK_THROW(parse_value("<int></int>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<int/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<i4></i4>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<i4/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<double></double>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<double/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<boolean></boolean>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<boolean/>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<datetime.iso8601></datetime.iso8601>"), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<dateTime.iso8601/>"), XML_RPC_violation);

  BOOST_CHECK_EQUAL(parse_value("<string/>").get_string(), "");
  BOOST_CHECK_EQUAL(parse_value("<string></string>").get_string(), "");
  BOOST_CHECK_EQUAL(parse_value("<base64/>").get_binary().get_data(), "");
  BOOST_CHECK_EQUAL(parse_value("<base64></base64>").get_binary().get_data(), "");

  //
  // Arrays with empty values
  BOOST_CHECK_EQUAL(parse_value("<array/>").the_array().size(), 0);
  BOOST_CHECK_EQUAL(parse_value("<array></array>").the_array().size(), 0);
  BOOST_CHECK_EQUAL(parse_value("<array><data></data></array>").the_array().size(), 0);

  Array a1 = parse_value("<array><data><value/></data></array>").the_array();
  BOOST_CHECK_EQUAL(a1.size(), 1);
  BOOST_CHECK_EQUAL(a1[0].get_string(), "");

  Array a2 = parse_value("<array><data><value></value></data></array>").the_array();
  BOOST_CHECK_EQUAL(a2.size(), 1);
  BOOST_CHECK_EQUAL(a2[0].get_string(), "");

  //
  // Structs with empty values
  BOOST_CHECK_EQUAL(parse_value("<struct/>").the_struct().size(), 0);
  BOOST_CHECK_EQUAL(parse_value("<struct></struct>").the_struct().size(), 0);
  BOOST_CHECK_THROW(parse_value("<struct><member></member></struct>").the_struct(), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<struct><member><value/></member></struct>").the_struct(), XML_RPC_violation);
  BOOST_CHECK_THROW(parse_value("<struct><member><name/></member></struct>").the_struct(), XML_RPC_violation);

  Struct s1 = parse_value("<struct><member><name/><value>123</value></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s1.size(), 1);
  BOOST_CHECK_EQUAL(s1[""].get_string(), "123");

  Struct s2 = parse_value("<struct><member><name></name><value/></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s2.size(), 1);
  BOOST_CHECK_EQUAL(s2[""].get_string(), "");

  Struct s3 = parse_value("<struct><member><name></name><value></value></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s3.size(), 1);
  BOOST_CHECK_EQUAL(s3[""].get_string(), "");

  Struct s4 = parse_value("<struct><member><name/><value></value></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s4.size(), 1);
  BOOST_CHECK_EQUAL(s4[""].get_string(), "");

  Struct s5 = parse_value("<struct><member><name/><value/></member></struct>").the_struct();
  BOOST_CHECK_EQUAL(s5.size(), 1);
  BOOST_CHECK_EQUAL(s5[""].get_string(), "");
}

//
// request
//

void test_parse_request()
{
  std::string r = "<methodCall> \
  <methodName>get_weather</methodName> \
  <params> \
    <param> \
      <value> \
        <string>Krasnoyarsk</string> \
      </value> \
    </param> \
    <param> \
      <value>now</value> \
    </param> \
  </params> \
</methodCall>";

  std::auto_ptr<Request> req(parse_request(r));
  BOOST_CHECK_EQUAL(req->get_name(), "get_weather");
  BOOST_CHECK_EQUAL(req->get_params().size(), 2);
  BOOST_CHECK_EQUAL(req->get_params().front().get_string(), "Krasnoyarsk");
  BOOST_CHECK_EQUAL(req->get_params().back().get_string(), "now");
}

void test_parse_request_no_params()
{
  std::string r = "<methodCall><methodName>do_something</methodName></methodCall>";
  std::auto_ptr<Request> req(parse_request(r));
  BOOST_CHECK_EQUAL(req->get_name(), "do_something");
  BOOST_CHECK_EQUAL(req->get_params().size(), 0);
}

//
// response
//

void test_parse_ok_response()
{
  std::string r = "<methodResponse> \
  <params> \
    <param> \
      <value> \
       <struct> \
        <member> \
          <name>temp</name> \
          <value><double>15.5</double></value> \
        </member> \
       </struct> \
      </value> \
    </param> \
  </params> \
</methodResponse>";

  Response res = parse_response(r);
  BOOST_CHECK(res.value().is_struct());
  BOOST_CHECK_EQUAL(res.value().the_struct().size(), 1);
  BOOST_CHECK_EQUAL(res.value()["temp"].get_double(), 15.5);
}

void test_parse_fault_response()
{
  std::string r = "<methodResponse> \
    <fault> \
      <value> \
       <struct> \
        <member> \
          <name>faultCode</name> \
          <value><int>143</int></value> \
        </member> \
        <member> \
          <name>faultString</name> \
          <value>Out of beer</value> \
        </member> \
       </struct> \
      </value> \
    </fault> \
</methodResponse>";

  Response res = parse_response(r);
  BOOST_CHECK(res.is_fault());
  BOOST_CHECK_EQUAL(res.fault_code(), 143);
  BOOST_CHECK_EQUAL(res.fault_string(), "Out of beer");
}


bool init_tests()
{
  test_suite& test = framework::master_test_suite();
  test.add( BOOST_TEST_CASE(&test_parse_scalar) );
  test.add( BOOST_TEST_CASE(&test_parse_array) );
  test.add( BOOST_TEST_CASE(&test_parse_simple_struct) );
  test.add( BOOST_TEST_CASE(&test_parse_nested_struct) );
  test.add( BOOST_TEST_CASE(&test_parse_unknown_type) );
  test.add( BOOST_TEST_CASE(&test_parse_bad_xml) );
  test.add( BOOST_TEST_CASE(&test_parse_formatted) );
  test.add( BOOST_TEST_CASE(&test_parse_emptiness) );

  test.add( BOOST_TEST_CASE(&test_parse_request) );
  test.add( BOOST_TEST_CASE(&test_parse_request_no_params) );
  test.add( BOOST_TEST_CASE(&test_parse_ok_response) );
  test.add( BOOST_TEST_CASE(&test_parse_fault_response) );

  return true;
}

int main( int argc, char* argv[] )
{
  boost::unit_test::unit_test_main( &init_tests, argc, argv );
}

// vim:ts=2:sw=2:et
