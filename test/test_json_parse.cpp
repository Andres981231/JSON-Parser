//
// Created by lirundong on 2019-03-26.
//

#include <memory>
#include <string>
#include <stdexcept>

#include "gtest/gtest.h"
#include "toy_json.hpp"

TEST(TestJson, TestParse) {
  using std::string;
  using toy_json::Json;
  using toy_json::JsonNode;

  const string valid_json_path{"../test/data/valid.json"};
  std::unique_ptr<JsonNode> valid_json_ptr = Json::parse(valid_json_path);
  EXPECT_NE(valid_json_ptr, nullptr);
  EXPECT_DOUBLE_EQ((*valid_json_ptr)["\xe4\xb8\xad\xe6\x96\x87_key"].get_number(), 2.342534252342134);
  EXPECT_EQ((*valid_json_ptr)["key with space"][0].is_null(), true);
  EXPECT_EQ((*valid_json_ptr)["key with space"][1].get_bool(), true);
  EXPECT_EQ((*valid_json_ptr)["key with space"][2].get_bool(), false);
  EXPECT_EQ((*valid_json_ptr)["nested_object"]["k1"].get_string(), string("\xe5\xa4\xa7\xe5\x93\xa5\xe5\x96\x9d\xe5\x95\xa4\xe9\x85\x92"));
  EXPECT_EQ((*valid_json_ptr)["nested_object"]["k2"].get_string(), string("have fun"));

  string invalid_json_path{"../test/data/invalid_null.json"};
  ASSERT_NO_THROW(Json::parse(invalid_json_path));
  std::unique_ptr<JsonNode> invalid_json_ptr = Json::parse(invalid_json_path);
  EXPECT_EQ(invalid_json_ptr, nullptr);
  EXPECT_EQ(Json::get_error_info(), string("parsing null type failed"));

  invalid_json_path = string("../test/data/invalid_bool.json");
  ASSERT_NO_THROW(Json::parse(invalid_json_path));
  invalid_json_ptr = Json::parse(invalid_json_path);
  EXPECT_EQ(invalid_json_ptr, nullptr);
  EXPECT_EQ(Json::get_error_info(), string("parsing bool type failed"));

  invalid_json_path = string("../test/data/invalid_number.json");
  ASSERT_NO_THROW(Json::parse(invalid_json_path));
  invalid_json_ptr = Json::parse(invalid_json_path);
  EXPECT_EQ(invalid_json_ptr, nullptr);
  EXPECT_EQ(Json::get_error_info(), string("parsing number type failed"));

  invalid_json_path = string("../test/data/invalid_string.json");
  ASSERT_NO_THROW(Json::parse(invalid_json_path));
  invalid_json_ptr = Json::parse(invalid_json_path);
  EXPECT_EQ(invalid_json_ptr, nullptr);
  EXPECT_EQ(Json::get_error_info(), string("parsing string type failed"));

  invalid_json_path = string("../test/data/invalid_array.json");
  ASSERT_NO_THROW(Json::parse(invalid_json_path));
  invalid_json_ptr = Json::parse(invalid_json_path);
  EXPECT_EQ(invalid_json_ptr, nullptr);
  EXPECT_EQ(Json::get_error_info(), string("parsing array type failed"));

  invalid_json_path = string("../test/data/invalid_object.json");
  ASSERT_NO_THROW(Json::parse(invalid_json_path));
  invalid_json_ptr = Json::parse(invalid_json_path);
  EXPECT_EQ(invalid_json_ptr, nullptr);
  EXPECT_EQ(Json::get_error_info(), string("parsing object type failed"));
}
