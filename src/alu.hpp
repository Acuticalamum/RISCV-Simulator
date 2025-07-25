#ifndef ALU_HPP
#define ALU_HPP
#include "instruction.hpp"
#include "rob.hpp"
class ALU {
  public:
    class Input {
      public:
        InstructionType type;
        int32_t val1;
        int32_t val2;
        uint32_t dest;
        Input() = default;
        Input(InstructionType type_,  uint32_t val1_, uint32_t val2_,  uint32_t dest_) : 
        type(type_), val1(val1_), val2(val2_), dest(dest_) {} 
    };

    class WireIn {
      public:
        Input input;
        bool wired = false;
    } wire_in;

    class Output {
      public:
        uint32_t val;
        uint32_t dest;
        Output() = default;
        Output(uint32_t val_, uint32_t dest_) : val(val_), dest(dest_) {} 
    };

    class WireOut {
      public:
        Output output;
        bool wired = false;
    } wire_out;

    Output calculate(Input &in) {
      uint32_t val;
      switch(in.type) {
        case ADD:  val = in.val1 + in.val2; break;
        case SUB:  val = in.val1 - in.val2; break;
        case AND:  val = in.val1 & in.val2; break;
        case OR:   val = in.val1 | in.val2; break;
        case XOR:  val = in.val1 ^ in.val2; break;
        case SLL:  val = ((uint32_t)in.val1) << in.val2; break;
        case SRL:  val = ((uint32_t)in.val1) >> in.val2; break;
        case SLT:  val = in.val1 < in.val2; break;
        case SLTU: val = ((uint32_t)in.val1) < ((uint32_t)in.val2); break;
        case SRA:  val = in.val1 >> in.val2; break;
        case BEQ:  val = in.val1 == in.val2; break;
        case BGE:  val = in.val1 >= in.val2; break;
        case BGEU: val = ((uint32_t)in.val1) >= ((uint32_t)in.val2); break;
        case BLT:  val = in.val1 < in.val2; break;
        case BLTU: val = ((uint32_t)in.val1) < ((uint32_t)in.val2); break;
        case BNE:  val = in.val1 != in.val2; break;
        case JALR: val = in.val1 + in.val2; break;
      }
      return Output(val, in.dest);
    }

    void run() {
      if(wire_out.wired == true) return;
      if(wire_in.wired == false) return;
      wire_in.wired = false;
      wire_out.output = calculate(wire_in.input);
      wire_out.wired = true;
    }

    void flush(ROB* rob) {
      if(wire_out.wired) {
        rob -> buffer[wire_out.output.dest].value = wire_out.output.val;
        rob -> buffer[wire_out.output.dest].state = kCommit;
        wire_out.wired = false;
      }
    }
  
};
#endif