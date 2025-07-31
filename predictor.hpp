#ifndef PREDICTOR_HPP
#define PREDICTOR_HPP
#include<stdint.h>
#include<random>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);

int32_t tot[1048576]{};
int32_t success[1048576]{};

double random_0_1() {
  double random_number = dis(gen);
  return random_number;
}

class Predictor {
  public:
  bool jump(uint32_t pc) {
    if(tot[pc] == 0) return random_0_1() < 0.5;
    else return random_0_1() < (double)success[pc] / (double)tot[pc];
    return true;
  }
  void add(uint32_t pc) {
    ++tot[pc];
  }
  void suc(uint32_t pc) {
    ++success[pc];
  }
};
#endif