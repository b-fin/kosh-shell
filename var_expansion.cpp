#include "var_expansion.h"

bool word_can_be_expanded(const std::string& in_str) {
  // no word can have '$' at the end of it, and an expandable word must have '$' in it
  if ( (in_str.back() == '$') || (in_str.find('$') == std::string::npos) ) {
    return false;
  }
  return true;
}
// first vector is const because we don't want to change the symbol table
// second vector is not const because we need to be able to change the arguments vector
// string is const because we don't really need to change it
// int is const cuz eh why not
bool word_expand_arg(const std::unordered_map<std::string,std::string>& in_s_t, std::vector<std::string>& in_vec, const std::string& in_str, const int pos) {
  size_t position = in_str.find('$');

  // test to be sure that the passed string does, in fact, contain one and only one '$'
  if ( (position == std::string::npos) || (std::count(in_str.begin(),in_str.end(),'$') != 1) ) {
    return false;
  }

  std::string tmp_pfix = in_str.substr(0,position);
  std::string tmp_str = in_str.substr(position+1);
  for (auto x : in_s_t) {
    if (tmp_str == x.first) {
      tmp_str = tmp_pfix + x.second;
      in_vec.erase(in_vec.begin()+pos);
      in_vec.insert( (in_vec.begin()+pos), tmp_str);
      return true;
    }
  }
  // variable could not be expanded
  std::cout<< "In word_expand_arg, argument could not be expanded\n";
  return false;
}

std::string word_expand_value(const std::unordered_map<std::string,std::string>& in_s_t, const std::string& in_str) {
  size_t position = in_str.find('$');

  // ensures that there is one and only one '$' in the string
  if ( (position == std::string::npos) || (std::count(in_str.begin(),in_str.end(),'$') != 1) ) {
    return "";
  }
  std::string tmp_pfix = in_str.substr(0,position);
  std::string tmp_str = in_str.substr(position+1);
  for (auto x : in_s_t) {
    if (tmp_str == x.first) {
      tmp_str = tmp_pfix + x.second;
      return tmp_str;
    }
  }
  // end, bad. return empty string
  std::cout<< "In word_expand_value, string could not be expanded\n";
  return "";
}
