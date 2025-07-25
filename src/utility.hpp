#ifndef UTILITY_HPP
#define UTILITY_HPP
#include<string>
#include<stdint.h>
uint32_t hexConvert(std::string str) {
  if(str[0] == '@') {
    str = str.erase(0, 1);
  }
  return std::stoi(str, nullptr, 16);
}
#endif