#include "var_expansion.h"


// Returns:
//  - true if in_str parameter can be expanded (ie has a '$')
//  - false otherwise
bool word_can_be_expanded(const std::string& in_str) {
  bool status;
  // no word can have '$' at the end of it, and an expandable word must have '$' in it
  if ( (in_str.back() == '$') || (in_str.find('$') == std::string::npos) ) {
    status = false;
  } else {
      status = true;
  }
  return status;
}

// Parameters:
//  1. symbol table from Shell (unordered map of strings); const because we
//      definitely don't want to mess with it, ref for speed
//  2. vector of strings from Arguments node. We want to be able to modify it.
//  3. string, the word to be expanded
//  4. integer, position within the arguments vector
// Returns:
//  - true only if the word matched a variable and was succesfully modified
//  - false otherwise
bool word_expand_arg(const std::unordered_map<std::string,std::string>& in_s_t, std::vector<std::string>& in_vec, const std::string& in_str, const int pos) {
  bool status = false;
  size_t position = in_str.find('$');

  // test to be sure that the passed string does, in fact, contain one and only one '$'
  if ( (position == std::string::npos) || (std::count(in_str.begin(),in_str.end(),'$') != 1) ) {
    // status = false;
    std::cout<< "In word_expand_arg, argument has either 0 or >1 '$'s\n";
  } else {
      std::string tmp_pfix = in_str.substr(0,position);
      std::string tmp_str = in_str.substr(position+1);
      for (auto x : in_s_t) {
        // if at least one of the args can be expanded, then return true
        if (tmp_str == x.first) {
          tmp_str = tmp_pfix + x.second;
          in_vec.erase(in_vec.begin()+pos);
          in_vec.insert( (in_vec.begin()+pos), tmp_str);
          status = true;
          // we found a match, no need for more expansion attempts
          break;
        }
      }
  }
  return status;
}


// The analogue of above function except for command words and other strings.
// Parameters:
//  1. symbol table from Shell. Same as above.
//  2. string, to be expanded.
// Returns:
//  - expanded string if the expansion was successfull
//  - empty string otherwise
std::string word_expand_value(const std::unordered_map<std::string,std::string>& in_s_t, const std::string& in_str) {
  size_t position = in_str.find('$');
  std::string ret_str = "";

  // ensures that there is one and only one '$' in the string
  if ( (position == std::string::npos) || (std::count(in_str.begin(),in_str.end(),'$') != 1) ) {
    std::cout<< "In word_expand_value, argument has either 0 or >1 '$'s\n";
  } else {
      std::string tmp_pfix = in_str.substr(0,position);
      std::string tmp_str = in_str.substr(position+1);
      for (auto x : in_s_t) {
        if (tmp_str == x.first) {
          tmp_str = tmp_pfix + x.second;
          ret_str = tmp_str;
        }
      }
  }
  return ret_str;
}
