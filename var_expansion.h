#ifndef __VAR_EXPANSION_H
#define __VAR_EXPANSION_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_map>

bool word_can_be_expanded(const std::string& in_str);

bool word_expand_arg(const std::unordered_map<std::string,std::string>& in_s_t, std::vector<std::string>& in_vec, const std::string& in_str, const int pos);

std::string word_expand_value(const std::unordered_map<std::string,std::string>& in_s_t, const std::string& in_str);

#endif
