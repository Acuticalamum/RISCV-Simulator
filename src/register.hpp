#ifndef REGISTER_HPP
#define REGISTER_HPP
#include<stdint.h>
class RegisterFile {
  public:
  uint32_t reg[32];
  int32_t dependency[32];
  RegisterFile() {
    for(int i = 0; i < 32; i++) {
      reg[i] = 0, dependency[i] = -1;
    }
  }
  void clear() {
    for(int i = 0; i < 32; i++) {
      dependency[i] = -1;
    }
  }
};
#endif