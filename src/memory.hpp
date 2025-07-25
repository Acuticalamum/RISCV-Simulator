#ifndef MEMORY_HPP
#define MEMORY_HPP
#include<stdint.h>
#include<string>
#include<iostream>
#include"utility.hpp"
class Memory {
  public: 
    uint8_t mem[65536];
    Memory() = default;
    void readIn() {
      std::string line;
      uint32_t addr;
      while(std::cin >> line) {
        if(line[0] == '@') addr = hexConvert(line);
        else mem[addr++] = (uint8_t)(hexConvert(line));
      }
    }
};
#endif