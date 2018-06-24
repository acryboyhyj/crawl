#ifndef _UTIL_H_
#define _UTIL_H_
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> Split(const std::string& s, char delim);

std::string Join(const std::vector<std::string>& elements,
                 const char* const separator);

std::string StringToHex(const std::string& input);

bool HexToString(const std::string& input, std::string* output);

#endif  // _UTIL_H_
