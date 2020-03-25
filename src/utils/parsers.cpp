//
// Created by lirundong on 2019-03-25.
//

#include <sstream>
#include "utils/parsers.hpp"

namespace toy_json {

// TODO: Implement parsing helpers in this file. If any parsing error occurs,
//   e.g., meet with invalid input string, just throw `std::runtime_error` with
//   bellowing error information (without surrounding ` ):
//   - fail parsing `bool`:    `parsing bool type failed`
//   - fail parsing `number`:  `parsing number type failed`
//   - fail parsing `string`:  `parsing string type failed`
//   - fail parsing `array`:   `parsing array type failed`
//   - fail parsing `object`:  `parsing object type failed`

bool parse_bool(std::string::const_iterator &str_it) {
  bool result = false;
  // TODO: implement parsing and advance iterator
  std::string if_bool_str; // use this to store the string which is likely to be a bool
  while ( *str_it != ',' && *str_it != ']' && *str_it != '}' && *str_it != '\0' ) {
    if_bool_str.push_back(*str_it);
    str_it += 1;
  }
  if ( if_bool_str == "true" ) // it is a vaild bool value
    result = true;
  else if ( if_bool_str == "false" ) // it is a vaild bool value
    result = false;
  else { // it is an invalid bool value
    throw std::runtime_error("parsing bool type failed");
  }
  return result;
}

double parse_number(std::string::const_iterator &str_it) {
  double result = 0.0;
  // TODO: implement parsing and advance iterator
  std::string if_number_str; // use this to store the string which is likely to be a number
  std::string::size_type end; // to see where the function stod will end at
  while ( *str_it != ',' && *str_it != ']' && *str_it != '}' && *str_it != '\0' ) {
    if ( *str_it == '+' ) { // if the first char is '+', it vaild to stod, but invalid indeed
      throw std::runtime_error("parsing number type failed");
    }
    else {
      if_number_str.push_back(*str_it);
      str_it += 1;      
    }
  }
  try {
    result = stod(if_number_str, &end); // call stod to translate string to double
  }
  catch (const std::invalid_argument& ia) { // stod raise invalid_argument
    throw std::runtime_error("parsing number type failed");
  }
  catch (const std::out_of_range& oor) { // stod raise out_of_range
    throw std::runtime_error("parsing number type failed");
  }
  if ( !if_number_str.substr(end).empty() ) { // if_number_str is not completely translated by stod, thus invalid
    throw std::runtime_error("parsing number type failed");
  }
  return result;
}

std::string parse_string(std::string::const_iterator &str_it) {
  std::string result;
  // TODO: implement parsing and advance iterator
  // NOTE: you are required to store unicode code-points in UTF-8 encoding
  str_it += 1; // skip the '\"' passed in
  while ( *str_it != '\"' && *str_it != '\0' ) {
    if ( *str_it == '\\' ) { //encounter a '\' char
      str_it += 1; // skip '\' and to see next char
      if ( *str_it == 'u' ) { // next char is 'u', encounter '\u', it is a unicode
        // init variables which will be used later
        int code_point, unicode_num, H, L;
        std::string unicode_str;
        std::string::size_type end;
        // get unicode in string form
        for (int i = 0; i < 4; ++i) {
          str_it += 1;
          unicode_str.push_back(*str_it);
        }
        // translate the form of the unicode through string to number
        unicode_num = stoi(unicode_str, &end, 16);
        // check the validity of the unicode
        if ( !unicode_str.substr(end).empty() ) {
          throw std::runtime_error("parsing string type failed");
        }
        else {
          unicode_str.clear(); // clear the string, prepare for next iteration
        }
        if ( unicode_num >= 0x0000 && unicode_num < 0xD800 ) { // do not need surrogate pair
          code_point = unicode_num;
          if ( code_point >= 0x0000 && code_point <= 0x007F ) {
            int hex1(code_point); // do not need operation to get 0xxxxxxx
            result.push_back(hex1);
          }
          else if ( code_point >= 0x0080 && code_point <= 0x07FF ) {
            int hex1(0xC0 | ((code_point >> 12) & 0x1F)); // 0xC0 = 11000000 0x1F = 00011111, operate to get 110xxxxx
            int hex2(0x80 | ( code_point        & 0x3F)); // 0x80 = 10000000 0x3F = 00111111, operate to get 10xxxxxx
            result.push_back(hex1);
            result.push_back(hex2);
          }
          else if ( code_point >= 0x0800 && code_point <= 0xFFFF ) {
            int hex1(0xE0 | ((code_point >> 12) & 0xF));  // 0xE0 = 11100000 0xF = 00001111,  operate to get 1110xxxx
            int hex2(0x80 | ((code_point >>  6) & 0x3F)); // 0x80 = 10000000 0x3F = 00111111, operate to get 10xxxxxx
            int hex3(0x80 | ( code_point        & 0x3F)); // 0x80 = 10000000 0x3F = 00111111, operate to get 10xxxxxx
            result.push_back(hex1);
            result.push_back(hex2);
            result.push_back(hex3);
          }
        }
        else if ( unicode_num >= 0xD800 && unicode_num <= 0xDBFF ) { // this is a high surrogate pair, need low surrogate pair
          H = unicode_num;
          if ( *(str_it + 1) == '\\' && *(str_it + 2) == 'u' ) { // see whether there follows another unicode
            str_it += 2;
          }
          else { // no unicode follows, invalid
            throw std::runtime_error("parsing string type failed");
          }
          // get the low surrogate unicode in string form
          for (int i = 0; i < 4; ++i) {
            str_it += 1;
            unicode_str.push_back(*str_it);
          }
          // translate the form of the low surrogate unicode through string to number
          L = stoi(unicode_str, &end, 16);
          // check the validity of the low surrogate unicode
          if ( !unicode_str.substr(end).empty() ) {
            throw std::runtime_error("parsing string type failed");
          }
          else {
            unicode_str.clear(); // clear the string, prepare for next iteration
          }
          if ( L >= 0xDC00 && L <= 0xDFFF ) { // valid low surrogate
            code_point = 0x10000 + (H - 0xD800) * 0x400 + (L - 0xDC00);
            int hex1(0xF0 | ((code_point >> 18) & 0x7));  // 0xF0 = 11110000 0x7 = 00000111,  operate to get 11110xxx
            int hex2(0x80 | ((code_point >> 12) & 0x3F)); // 0x80 = 10000000 0x3F = 00111111, operate to get 10xxxxxx
            int hex3(0x80 | ((code_point >> 6)  & 0x3F)); // 0x80 = 10000000 0x3F = 00111111, operate to get 10xxxxxx
            int hex4(0x80 | ( code_point        & 0x3F)); // 0x80 = 10000000 0x3F = 00111111, operate to get 10xxxxxx
            result.push_back(hex1);
            result.push_back(hex2);
            result.push_back(hex3);
            result.push_back(hex4);
          }
          else { // invalid low surrogate, cuz out of range
            throw std::runtime_error("parsing string type failed");
          }
        }
        else { // unicode is out of range, invalid
          throw std::runtime_error("parsing string type failed");
        }
      }
      else { // just an '\' char, just add to the result
        result.push_back(*str_it);
      }
      str_it += 1;
    }
    else { // encounter a normal char, just add to the result
      result.push_back(*str_it);
      str_it += 1;
    }
  }
  if ( *str_it == '\0' ) { // "" not match, invalid string
    throw std::runtime_error("parsing string type failed");
  }
  else {
    str_it += 1; // advance the iterator
  }
  return result;
}

JsonNode::array parse_array(std::string::const_iterator &str_it) {
  JsonNode::array result;
  // TODO: implement parsing and advance iterator
  // NOTE: we did not provide `parse_null` above, however `null` is one of the
  //   valid array values
  str_it += 1; // skip the '["' passed in
  // check whether the case is like '[,'
  while ( *str_it == ' ' ) {
    str_it += 1;
  }
  if ( *str_it == ',' ) {
    throw std::runtime_error("parsing array type failed");
  }
  while ( *str_it != ']' && *str_it != '\0' ) {
    if ( *str_it == 'n' || *str_it == 'N' ) { // parse null
      std::string if_null_str; // use this to store the string which is likely to be null
      while ( *str_it != ',' && *str_it != ']' && *str_it != '}' ) {
        if_null_str.push_back(*str_it);
        str_it += 1;
      }
      if ( if_null_str == "null" ) { // it is a valid null
        JsonNode new_node; // create a new node with type JSON_NULL
        result.push_back(new_node); // push the node into result
      }
      else { // it is an invalid null
        throw std::runtime_error("parsing null type failed");
      }
    }
    else if ( *str_it == 'T' || *str_it == 't' || *str_it == 'F' || *str_it == 'f' ) { // parse bool
      JsonNode new_node(parse_bool(str_it)); // create a new node with type JSON_BOOL
      result.push_back(new_node); // push the node into result
    }
    else if ( *str_it == '+' || *str_it == '-' || std::isdigit(*str_it) != 0 ) { // parse number
      JsonNode new_node(parse_number(str_it)); // create a new node with type JSON_NUMBER
      result.push_back(new_node); // push the node into result
    }
    else if ( *str_it == '\"' ) { // parse string
      std::unique_ptr<std::string> s(new std::string(parse_string(str_it))); // init a string unique_ptr
      JsonNode new_node(std::move(s)); // passing s as rvalue to init function to create a new node with type JSON_STRING
      result.push_back(new_node); // push the node into result
    }
      else if ( *str_it == '[' ) { // parse array
      std::unique_ptr<JsonNode::array> a(new JsonNode::array(parse_array(str_it))); // init an array unique_ptr
      JsonNode new_node(std::move(a)); // passing a as rvalue to init function to create a new node with type JSON_ARRAY
      result.push_back(new_node); // push the node into result
    }
    else if ( *str_it == '{' ) { // parse object
      std::unique_ptr<JsonNode::object> o(new JsonNode::object(parse_object(str_it))); // init a object unique_ptr
      JsonNode new_node(std::move(o)); // passing o as rvalue to init function to create a new node with type JSON_OBJECT
      result.push_back(new_node); // push the node into result
    }
    else if ( *str_it == ',' ) { // check the validity of the array
      str_it += 1;
      while ( *str_it == ' ' ) { // ignore the white space
        str_it += 1;
      }
      if ( *str_it == ',' || *str_it == ']' ) { // no element after ',' invalid
        throw std::runtime_error("parsing array type failed");
      }
    }
    else { // meaning less char, just ignore it
      str_it += 1;
    }
  }
  if ( *str_it == '\0' ) { // [] not match, invalid array
    throw std::runtime_error("parsing array type failed");
  }
  else {
    str_it += 1; // advance the iterator
  }
  return result;
}

JsonNode::object parse_object(std::string::const_iterator &str_it) {
  JsonNode::object result;
  // TODO: implement parsing and advance iterator
  // NOTE: 1. we did not provide `parse_null` above, however `null` is one of the
  //          valid object values
  //       2. object key can be any kind of json string, which means it may
  //          contain spaces, escapes, unicode code-points, etc., so take care!
  str_it += 1; // skip the '{"' passed in
  // check whether the case is like '{,'
  while ( *str_it == ' ' ) {
    str_it += 1;
  }
  if ( *str_it == ',' ) {
    throw std::runtime_error("parsing array type failed");
  }
  std::string key; // init the variable key which will be used in the follow iteration
  while ( *str_it != '}' && *str_it != '\0' ) {
    if ( *str_it == '\"' ) { //parse string
      if ( key.empty() ) { // key has no content by now, so the string encountered should be a key
        key = parse_string(str_it); // assign key to the string encountered
      }
      else { // key has content by now, so the string encountered should be an element
        std::unique_ptr<std::string> s(new std::string(parse_string(str_it))); // init a string unique_ptr
        JsonNode value(std::move(s)); // passing s as rvalue to init function to create a value with type JSON_STRING
        std::pair<std::string, JsonNode> key_value (key, value); // init a pair contain key and value
        result.insert(key_value); // add the pair to the unordered map
        key.clear(); // clear the key, prepare for the next iteration
      }
    }
    else if ( *str_it == 'n' || *str_it == 'N' ) { // parse null
      std::string if_null_str; // use this to store the string which is likely to be null
      while ( *str_it != ',' && *str_it != ']' && *str_it != '}' ) {
        if_null_str.push_back(*str_it);
        str_it += 1;
      }
      if ( if_null_str == "null" ) { // it is a valid null
        JsonNode value; // create a value with type JSON_NULL
        std::pair<std::string, JsonNode> key_value (key, value); // init a pair contain key and value
        result.insert(key_value); // add the pair to the unordered map
        key.clear(); // clear the key, prepare for the next iteration
      }
      else { // it is an invalid null
        throw std::runtime_error("parsing null type failed");
      }
    }
    else if ( *str_it == 'T' || *str_it == 't' || *str_it == 'F' || *str_it == 'f' ) { // parse bool
      JsonNode value(parse_bool(str_it)); // create a value with type JSON_BOOL
      std::pair<std::string, JsonNode> key_value (key, value); // init a pair contain key and value
      result.insert(key_value); // add the pair to the unordered map
      key.clear(); // clear the key, prepare for the next iteration
    }
    else if ( *str_it == '+' || *str_it == '-' || std::isdigit(*str_it) != 0 ) { // parse number
      JsonNode value(parse_number(str_it)); // create a value with type JSON_NUMBER
      std::pair<std::string, JsonNode> key_value (key, value);  // init a pair contain key and value
      result.insert(key_value); // add the pair to the unordered map
      key.clear(); // clear the key, prepare for the next iteration
    }
    else if ( *str_it == '[' ) { // parse array
      std::unique_ptr<JsonNode::array> a(new JsonNode::array(parse_array(str_it))); // init an array unique_ptr
      JsonNode value(std::move(a)); // passing a as rvalue to init function to create a value with type JSON_ARRAY
      std::pair<std::string, JsonNode> key_value (key, value); // init a pair contain key and value
      result.insert(key_value); // add the pair to the unordered map
      key.clear(); // clear the key, prepare for the next iteration
    }
    else if ( *str_it == '{' ) { // parse object
      std::unique_ptr<JsonNode::object> o(new JsonNode::object(parse_object(str_it))); // init an object unique_ptr
      JsonNode value(std::move(o)); // passing o as rvalue to init function to create a value with type JSON_OBJECT
      std::pair<std::string, JsonNode> key_value (key, value); // init a pair contain key and value
      result.insert(key_value); // add the pair to the unordered map
      key.clear(); // clear the key, prepare for the next iteration
    }
    else if ( *str_it == ',' ) { // check the validity of the array
      str_it += 1;
      while ( *str_it == ' ' ) { // ignore the white space
        str_it += 1;
      }
      if ( *str_it == ',' || *str_it == '}' ) { // no element after ',' invalid
        throw std::runtime_error("parsing object type failed");
      }
    }
    else { // meaningless char, just ignore it
      str_it += 1;
    }
  }
  if ( *str_it == '\0' ) { // {} not match, invalid array
    throw std::runtime_error("parsing object type failed");
  }
  else {
    str_it += 1; // advance the iterator
  }
  return result;
}

}