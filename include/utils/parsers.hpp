//
// Created by lirundong on 2019-03-25.
//

#ifndef TOYJSON_PARSERS_HPP
#define TOYJSON_PARSERS_HPP

#include <string>
#include "toy_json.hpp"

namespace toy_json {

bool parse_bool(std::string::const_iterator &str_it);

double parse_number(std::string::const_iterator &str_it);

std::string parse_string(std::string::const_iterator &str_it);

JsonNode::array parse_array(std::string::const_iterator &str_it);

JsonNode::object parse_object(std::string::const_iterator &str_it);

}

#endif //TOYJSON_PARSERS_HPP
