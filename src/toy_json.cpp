#include <iostream>
#include <fstream>

#include "toy_json.hpp"
#include "utils/parsers.hpp"

namespace toy_json {

JsonNode::JsonNode(const JsonNode &rhs) {
  release_union();
  type_ = rhs.type();
  switch (rhs.type()) {
    case JSON_NULL:
      break;
    case JSON_BOOL:
      v_bool_ = rhs.get_bool();
      break;
    case JSON_NUMBER:
      v_number_ = rhs.get_number();
      break;
    case JSON_STRING:
      new(&v_string_) std::unique_ptr<std::string>(new std::string{*rhs.v_string_});
      break;
    case JSON_ARRAY:
      new(&v_array_) std::unique_ptr<array>(new array{*rhs.v_array_});
      break;
    case JSON_OBJECT:
      new(&v_object_) std::unique_ptr<object>(new object{*rhs.v_object_});
      break;
  }
}

JsonNode::JsonNode(JsonNode &&rhs) noexcept {
  release_union();
  type_ = rhs.type();
  switch (rhs.type()) {
    case JSON_NULL:
      break;
    case JSON_BOOL:
      v_bool_ = rhs.get_bool();
      break;
    case JSON_NUMBER:
      v_number_ = rhs.get_number();
      break;
    case JSON_STRING:
      new(&v_string_) std::unique_ptr<std::string>(std::move(rhs.v_string_));
      break;
    case JSON_ARRAY:
      new(&v_array_) std::unique_ptr<array>(std::move(rhs.v_array_));
      break;
    case JSON_OBJECT:
      new(&v_object_) std::unique_ptr<object>(std::move(rhs.v_object_));
      break;
  }
}

void JsonNode::release_union() {
  switch (type_) {
    case JSON_STRING:
      v_string_.~unique_ptr<std::string>();
      break;
    case JSON_ARRAY:
      v_array_.~unique_ptr<array>();
      break;
    case JSON_OBJECT:
      v_object_.~unique_ptr<object>();
      break;
    default:
      break;
  }
}

std::string Json::error_info_;

std::unique_ptr<JsonNode> Json::parse(const std::string &fin) noexcept {
  // TODO: Implement main parsing procedure here, some notices:
  //   1. You may find helper functions in `utils/parsers.hpp` useful. Note that
  //      we did not provide `parse_null`, why?
  //   2. Note that actual data that larger than 4 bytes is hold by `shared_ptr`
  //      you should construct `string`, `array` and `object` types by passing
  //      r-valued `unique_ptr`s;
  //   3. DO NOT throw exceptions out of this function. If an internal exception
  //      occurs, e.g. get invalid input, just catch that exception, modify the
  //      `error_info_` and return a `nullptr`;
  //   4. Expected `error_info` for each kind of parsing errors: (these
  //      messages, without surrounding "`", will be examined in unit tests,
  //      make sure you throw the correct internal errors)
  //      - fail parsing `null`:    `parsing null type failed`
  //      - fail parsing `bool`:    `parsing bool type failed`
  //      - fail parsing `number`:  `parsing number type failed`
  //      - fail parsing `string`:  `parsing string type failed`
  //      - fail parsing `array`:   `parsing array type failed`
  //      - fail parsing `object`:  `parsing object type failed`
  
  // read the input file into a string named str
  std::ifstream ifs(fin);
  std::string str( (std::istreambuf_iterator<char>(ifs) ),
                   (std::istreambuf_iterator<char>()    ) );
  // remove all white spaces out of strings
  for (int i = 0, double_token_count = 0; i < str.length(); ) {
    if ( str[i] == '\"' ) {
      double_token_count += 1; // count the number of the double token
      i += 1;
    }
    else if ( str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r' ) {
      if ( double_token_count % 2 == 0 ) // the white space is out of strings, thus should be removed
        str.erase(str.begin() + i);
      else // the white space is inside strings, thus should not be removed
        i += 1;
    }
    else
      i += 1; // skip the char
  }
  // init an iterator for the dealed string
  std::string::const_iterator str_it = str.begin();
  try { // assume the input json file is an object
    // init a JsonNode unique_ptr as return value and parse the object at same time
    std::unique_ptr<JsonNode> obj_node(new JsonNode(std::unique_ptr<JsonNode::object>(new JsonNode::object(parse_object(str_it)))));
    return obj_node; // return the JsonNode unique_ptr
  }
  catch (const std::runtime_error& re) {
    error_info_ = re.what();
    return nullptr;
  }
}

}
