#ifndef RS_HPP
#define RS_HPP
#include"instruction.hpp"
class RSItem {
  public:
    bool busy;
    InstructionType op;
    uint32_t vj, vk, qj, qk;
    uint32_t dest;
    uint32_t imm;
};
class RS {
  
};
#endif