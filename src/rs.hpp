#ifndef RS_HPP
#define RS_HPP
#include"instruction.hpp"
#include"register.hpp"
#include"alu.hpp"
#include"lsb.hpp"

class RSItem {
  public:
    bool busy = true;
    InstructionType op;
    uint32_t vj = 0, vk = 0;
    int32_t qj = -1, qk = -1;
    uint32_t dest;
    int32_t imm;

    RSItem() = default;
    RSItem(Instruction& instruction_, uint32_t dest_) : 
    op(instruction_.type), dest(dest_){}

    void fill(const RegisterFile& rf, uint32_t rs1, uint32_t rs2 = -1) {
      if(rf.dependency[rs1] == -1 || rf.dependency[rs1] == dest) vj = rf.reg[rs1];
      else qj = rf.dependency[rs1];
      if(rs2 == -1) return;
      if(rf.dependency[rs2] == -1 || rf.dependency[rs2] == dest) vk = rf.reg[rs2];
      else qk = rf.dependency[rs2];
    }
};
class RS {
  public:
  class WireIn {
      public:
        RSItem input;
        bool wired = false;
        void wire(RSItem& input_) {
          if(!wired) {
            wired = true;
            input = input_;
            //puts("wiring");
            //std::cout << input.qj << ' ' << input.qk << std::endl;
          }
        }
    } wire_in;

  class Output {
    public:
      RSItem info;
      uint8_t num;
      Output() = default;
  };

  class WireOut {
    public:
      Output output;
      bool wired = false;
  } wire_out;

  const static uint8_t RS_SIZE = 8;
  RSItem data[RS_SIZE];
  bool use[RS_SIZE]{};
  uint8_t size = 0;
    
  bool full() {
    return size == RS_SIZE;
  }

  bool empty() {
    return size == 0;
  }

  void wire(RSItem& input_) {
    wire_in.wire(input_);
  }

  void run() {

    if(wire_in.wired && !full()) {
      wire_in.wired = false;
      use[size] = true;
      data[size] = wire_in.input;
      //puts("data put in");
      //std::cout << wire_in.input.qj << wire_in.input.qk << std::endl;
      //std::cout << data[size].qj << data[size].qk << std::endl;
      ++size;
    }
    
    if(!wire_out.wired && !empty()) {
      bool flag = true;
      for(int i = 0; i < size; ++i) {
        if(!flag && (19 <= data[i].op && data[i].op <= 26)) continue;

        if(use[i]) {
          if(data[i].qj == -1 && data[i].qk == -1) {
            wire_out.output.info = data[i];
            wire_out.wired = true;
            for(int j = i; j < size - 1; j++) {
              data[j] = data[j + 1];
              use[j] = use[j + 1];
            }
            --size;
            use[size] = false;
            break;
          }
        }

        if(19 <= data[i].op && data[i].op <= 26) flag = false;
      }
    }

  }

  void clear() {
    wire_in.wired = wire_out.wired = false;
    size = 0;
    for(int i = 0; i < RS_SIZE; ++i) use[i] = false;
  }

  void flush(ALU* alu, LSB* lsb) {
    if(wire_out.wired) {
      Output& output = wire_out.output;
      RSItem& info = output.info;
      if(19 <= info.op && info.op <= 26) {
        if(!lsb -> wire_in.wired) {
          wire_out.wired = false;
          flushToLSB(lsb, output);
        }
      } else {
        if(!alu -> wire_in.wired) {
          wire_out.wired = false;
          flushToALU(alu, output);
        }
      }
    }
  }

  private:
  void flushToLSB(LSB* lsb, Output& output) {
    //std::cout << output.info.vj << " " << output.info.imm << std::endl;
    LSBItem lsb_input(output.info.op, output.info.dest, output.info.vj + output.info.imm, output.info.vk);
    //std::cout << to_string[output.info.op] << ' ' << lsb_input.addr << ' ' << lsb_input.val << std::endl;
    lsb -> wire(lsb_input);
  }
  void flushToALU(ALU* alu, Output& output) {
    ALU::Input alu_input(
      output.info.op, output.info.vj, output.info.vk, output.info.dest);
    if(10 <= output.info.op && output.info.op <= 23) alu_input.val2 = output.info.imm;
    //throw
    alu -> wire(alu_input);
  }
};
#endif