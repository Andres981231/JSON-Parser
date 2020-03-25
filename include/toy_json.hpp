#ifndef TOYJSON_TOY_JSON_HPP
#define TOYJSON_TOY_JSON_HPP

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace toy_json {

enum JsonType {
  JSON_NULL, JSON_BOOL, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT
};

class JsonNode {
public:
  using array = std::vector<JsonNode>;
  using object = std::unordered_map<std::string, JsonNode>;

  JsonNode() : type_(JSON_NULL) {}

  JsonNode(const bool b) : type_(JSON_BOOL), v_bool_(b) {}

  JsonNode(const double n) : type_(JSON_NUMBER), v_number_(n) {}

  JsonNode(std::unique_ptr<std::string> &&s) :
      type_(JSON_STRING), v_string_(std::move(s)) {}

  JsonNode(std::unique_ptr<array> &&a) :
      type_(JSON_ARRAY), v_array_(std::move(a)) {}

  JsonNode(std::unique_ptr<object> &&o) :
      type_(JSON_OBJECT), v_object_(std::move(o)) {}

  JsonNode(const JsonNode &rhs);

  JsonNode(JsonNode &&rhs) noexcept;

  ~JsonNode() { release_union(); }

  JsonType type() const { return type_; }

  bool is_null() const { return type_ == JSON_NULL; }

  bool get_bool() const {
    if (type_ != JSON_BOOL) {
      throw std::runtime_error("get_bool on non-bool node");
    } else {
      return v_bool_;
    }
  }

  double get_number() const {
    if (type_ != JSON_NUMBER) {
      throw std::runtime_error("get_number on non-number node");
    } else {
      return v_number_;
    }
  }

  std::string &get_string() const {
    if (type_ != JSON_STRING) {
      throw std::runtime_error("get_string on non-string node");
    } else {
      return *v_string_;
    }
  }

  JsonNode &operator[](const size_t idx) const {
    if (type_ != JSON_ARRAY) {
      throw std::runtime_error("indexing on non-array node");
    } else {
      return (*v_array_)[idx];
    }
  }

  JsonNode &operator[](const std::string &key) const {
    if (type_ != JSON_OBJECT) {
      throw std::runtime_error("query on non-object node");
    } else {
      return (*v_object_)[key];
    }
  }

protected:
  void release_union();

  JsonType type_;
  union {
    bool v_bool_;
    double v_number_;
    std::unique_ptr<std::string> v_string_;
    std::unique_ptr<array> v_array_;
    std::unique_ptr<object> v_object_;
  };
};

class Json {
public:
  Json() = delete;

  ~Json() = delete;

  static std::unique_ptr<JsonNode> parse(const std::string &fin) noexcept;

  static std::string &get_error_info() { return error_info_; }

protected:
  static std::string error_info_;
};
}

#endif