#ifndef CPU_HPP
#define CPU_HPP
#include "alu.hpp"
#include "memory.hpp"
#include "decoder.hpp"
#include "register.hpp"
#include "rob.hpp"
class CPU {
  private:
    ALU alu;
    Memory memory;
    RegisterFile regs;
    Decoder decoder;
    ROB rob;
    uint32_t pc = 0;
    uint32_t pc_next = 0;
    uint32_t clk = 0;
  public:
    CPU() {
      memory.readIn();
    }
    bool run(uint32_t &output) {
      ++clk;
      decoderLaunch();
      alu.flush(&rob);
    }
  private:
    uint32_t instructionFetch() {
      uint32_t raw_code = memory.mem[pc + 3];
      raw_code = (raw_code << 8) | memory.mem[pc + 2];
      raw_code = (raw_code << 8) | memory.mem[pc + 1];
      raw_code = (raw_code << 8) | memory.mem[pc];
      return raw_code;
    }
    void decoderLaunch() {
      Instruction instruction = decoder.decode(instructionFetch());
      if(!rob.full()) {

      }
    }
};
#endif