#ifndef REGISTER_HPP
#define REGISTER_HPP
#include<stdint.h>
class RegisterFile {
  uint32_t reg[32];
  RegisterFile() = default;
};
#endif