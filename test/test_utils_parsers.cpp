//
// Created by lirundong on 2019-03-25.
//

#include <string>
#include <stdexcept>

#include "gtest/gtest.h"
#include "utils/parsers.hpp"

TEST(TestUtils, TestParseBool) {
  using std::string;
  using toy_json::parse_bool;

  const string t1{"true"}, t2{"True"}, t3{"tRue"};
  auto t1b = t1.cbegin(), t1e = t1.cend(),
      t2b = t2.cbegin(), t3b = t3.cbegin();

  EXPECT_EQ(true, parse_bool(t1b));
  EXPECT_EQ(t1b, t1e) << "did you advance the iterator?";
  EXPECT_THROW(parse_bool(t2b), std::runtime_error);
  EXPECT_THROW(parse_bool(t3b), std::runtime_error);

  const string f1{"false"}, f2{"False"}, f3{"fAlue"};
  auto f1b = f1.cbegin(), f1e = f1.cend(),
      f2b = f2.cbegin(), f3b = f3.cbegin();

  EXPECT_EQ(false, parse_bool(f1b));
  EXPECT_EQ(f1b, f1e) << "did you advance the iterator?";
  EXPECT_THROW(parse_bool(f2b), std::runtime_error);
  EXPECT_THROW(parse_bool(f3b), std::runtime_error);
}

TEST(TestUtils, TestParseNumber) {
  using std::string;
  using toy_json::parse_number;

  const string n1{"3.1415926"}, n2{"-12.333"}, n3{"233e-4"},
      n4{"+666"}, n5{"233.0f"}, n6{"2343.0999l"};
  auto n1b = n1.cbegin(), n1e = n1.cend(),
      n2b = n2.cbegin(), n2e = n2.cend(),
      n3b = n3.cbegin(), n3e = n3.cend(),
      n4b = n4.cbegin(), n5b = n5.cbegin(), n6b = n6.cbegin();

  EXPECT_DOUBLE_EQ(3.1415926, parse_number(n1b));
  EXPECT_EQ(n1b, n1e) << "did you advance the iterator?";
  EXPECT_DOUBLE_EQ(-12.333, parse_number(n2b));
  EXPECT_EQ(n2b, n2e) << "did you advance the iterator?";
  EXPECT_DOUBLE_EQ(233e-4, parse_number(n3b));
  EXPECT_EQ(n3b, n3e) << "did you advance the iterator?";
  EXPECT_THROW(parse_number(n4b), std::runtime_error);
  EXPECT_THROW(parse_number(n5b), std::runtime_error);
  EXPECT_THROW(parse_number(n6b), std::runtime_error);
}

TEST(TestUtils, TestParseString) {
  using std::string;
  using toy_json::parse_string;

  const string s1{"\"basic_string\""},
               s2{"\"string with spaces\""},
               s3{"\"string with\tescapes\n\t\""},
               // s41: 这是一段中文
               // UTF-8: \xe8\xbf\x99\xe6\x98\xaf\xe4\xb8\x80\xe6\xae\xb5\xe4\xb8\xad\xe6\x96\x87
               s41{R"("\u8fd9\u662f\u4e00\u6bb5\u4e2d\u6587")"},
               // s42: 大哥喝啤酒🍺
               // UTF-8: \xe5\xa4\xa7\xe5\x93\xa5\xe5\x96\x9d\xe5\x95\xa4\xe9\x85\x92\xf0\x9f\x8d\xba
               s42{R"("\u5927\u54e5\u559d\u5564\u9152\ud83c\udf7a")"},
               s5{"\"string without closing quote"},
               // s6: invalid unicode
               s6{R"("\u000/")"},
               // s7: invalid unicode surrogate
               s7{R"("\uD800\uDBFF")"};

  auto b = s1.cbegin(), e = s1.cend();
  EXPECT_EQ(string("basic_string"), parse_string(b));
  EXPECT_EQ(b, e) << "did you advance the iterator?";

  b = s2.cbegin(), e = s2.cend();
  EXPECT_EQ(string("string with spaces"), parse_string(b));
  EXPECT_EQ(b, e) << "did you advance the iterator?";

  b = s3.cbegin(), e = s3.cend();
  EXPECT_EQ(string("string with\tescapes\n\t"), parse_string(b));
  EXPECT_EQ(b, e) << "did you advance the iterator?";

  b = s41.cbegin(), e = s41.cend();
  EXPECT_EQ(string("\xe8\xbf\x99\xe6\x98\xaf\xe4\xb8\x80\xe6\xae\xb5\xe4\xb8\xad\xe6\x96\x87"),
            parse_string(b)) << "这是一段中文";
  EXPECT_EQ(b, e) << "did you advance the iterator?";

  b = s42.cbegin(), e = s42.cend();
  EXPECT_EQ(string("\xe5\xa4\xa7\xe5\x93\xa5\xe5\x96\x9d\xe5\x95\xa4\xe9\x85\x92\xf0\x9f\x8d\xba"),
            parse_string(b)) << "大哥喝啤酒🍺";
  EXPECT_EQ(b, e) << "did you advance the iterator?";

  b = s5.cbegin();
  EXPECT_THROW(parse_string(b), std::runtime_error) << "string without closing quote";

  b = s6.cbegin();
  EXPECT_THROW(parse_string(b), std::runtime_error) << "invalid unicode";

  b = s7.cbegin();
  EXPECT_THROW(parse_string(b), std::runtime_error) << "invalid unicode surrogate";
}

TEST(TestUtils, TestParseArray) {
  using std::string;
  using toy_json::parse_array;

  const string a1{R"([12.3333, false, null, "string with space", ["\u5927\u54e5\u559d\u5564\u9152\ud83c\udf7a"]])"},
               a2{"[2.3333, ]"},
               a3{"[, 666.6]"},
               a4{R"(["this array is invalid")"};

  auto b = a1.cbegin(), e = a1.cend();
  auto array_1 = parse_array(b);
  EXPECT_EQ(b, e) << "did you advance the iterator?";
  EXPECT_DOUBLE_EQ(array_1[0].get_number(), 12.3333);
  EXPECT_EQ(array_1[1].get_bool(), false);
  EXPECT_EQ(array_1[2].is_null(), true);
  EXPECT_EQ(array_1[3].get_string(), string("string with space"));
  EXPECT_EQ(array_1[4][0].get_string(), string("\xe5\xa4\xa7\xe5\x93\xa5\xe5\x96\x9d\xe5\x95\xa4\xe9\x85\x92\xf0\x9f\x8d\xba"));

  b = a2.cbegin();
  EXPECT_THROW(parse_array(b), std::runtime_error);
  b = a3.cbegin();
  EXPECT_THROW(parse_array(b), std::runtime_error);
  b = a4.cbegin();
  EXPECT_THROW(parse_array(b), std::runtime_error);
}

TEST(TestUtils, TestParseObject) {
  using std::string;
  using toy_json::parse_object;

  const string o1{R"({"\u4e2d\u6587_key": 2.342534252342134, "key with space": [null, true, false], "nested_object": {"k1": "\u5927\u54e5\u559d\u5564\u9152", "k2": "have fun"}})"},
               o2{R"({"invalid object": )"},
               o3{R"({"another": "invalid object", })"};

  auto b = o1.cbegin(), e = o1.cend();
  auto object_1 = parse_object(b);
  EXPECT_EQ(b, e) << "did you advance the iterator?";
  EXPECT_DOUBLE_EQ(object_1["\xe4\xb8\xad\xe6\x96\x87_key"].get_number(), 2.342534252342134);
  EXPECT_EQ(object_1["key with space"][0].is_null(), true);
  EXPECT_EQ(object_1["key with space"][1].get_bool(), true);
  EXPECT_EQ(object_1["key with space"][2].get_bool(), false);
  EXPECT_EQ(object_1["nested_object"]["k1"].get_string(), string("\xe5\xa4\xa7\xe5\x93\xa5\xe5\x96\x9d\xe5\x95\xa4\xe9\x85\x92"));
  EXPECT_EQ(object_1["nested_object"]["k2"].get_string(), string("have fun"));

  b = o2.cbegin();
  EXPECT_THROW(parse_object(b), std::runtime_error);

  b = o3.cbegin();
  EXPECT_THROW(parse_object(b), std::runtime_error);
}
