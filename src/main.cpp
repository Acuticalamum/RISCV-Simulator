#include<iostream>
#include<thread>
#include<chrono>
#include"cpu.hpp"
int main() {
  CPU cpu;
  uint8_t output;
  while(cpu.run(output));
  std::cout << (int32_t)output << std::endl;
  return 0;
}
