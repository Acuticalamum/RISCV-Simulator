#ifndef MEMORY_HPP
#define MEMORY_HPP
#include<stdint.h>
#include<string>
#include<iostream>
#include"utility.hpp"
#include"instruction.hpp"
#include"rob.hpp"

class Memory {
  public: 
    class Input {
      public:
        InstructionType type;
        uint32_t dest;
        uint32_t addr;
        uint32_t val;
        Input() = default;
        Input(InstructionType type_,  uint32_t dest_, uint32_t addr_,  uint32_t val_) : 
        type(type_), dest(dest_), addr(addr_), val(val_) {}
    };

    class WireIn {
      public:
        Input input;
        int8_t clk = -1;
        void wire(Input& input_) {
          if(clk == -1) {
            input = input_;
            clk = 0;
          }
        }
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

    uint8_t mem[1048576];
    bool reset = false;
    Memory() = default;

    void readIn() {
      std::string line;
      uint32_t addr;
      while(std::cin >> line) {
        if(line[0] == '@') addr = hexConvert(line);
        else mem[addr++] = (uint8_t)(hexConvert(line));
      }
    }

    void wire(Input input_) {
      wire_in.wire(input_);
    }

    void run() {
      if(reset) {
        reset = false;
        wire_in.clk = -1;
        return;
      }
      if(wire_in.clk == -1) return;
      if(wire_in.clk <= 1) {
        wire_in.clk++;
        return;
      }

      uint32_t addr = wire_in.input.addr;
      InstructionType type = wire_in.input.type;
      uint32_t val = wire_in.input.val;
      uint32_t dest = wire_in.input.dest;
      int32_t result = 0;

      //std::cout << to_string[type] << std::endl;
      //std::cout << dest << ' ' << addr << std::endl;

      switch (type) {
        case LB: case LBU: {
          result = mem[addr];
          if(type == LB && result & 0x80) result |= 0xffffff00;
          break;
        }
        case LH: case LHU: {
          result = ((int32_t)mem[addr + 1] << 8) | mem[addr];
          if(type == LH && result & 0x8000) result |= 0xffff0000;
          break;
        }
        case LW: {
          //std::cout << "\n\nADDR=" << addr << "\n\n";
          for(int i = 3; i >= 0; i--)   {
            result = (result << 8) | mem[addr + i];
            //std::cout << "result" << result << '\n';
          }
          /*puts("LWRESULT");
          std::cout << "dest:" << dest << std::endl;
          std::cout << result << std::endl;*/
          break;
        }
        case SB: {
          mem[addr] = val & 0xff;
          break;
        }
        case SH: {
          mem[addr] = val & 0xff;
          mem[addr + 1] = (val >> 8) & 0xff;
          break;
        }
        case SW: {
          mem[addr] = val & 0xff;
          mem[addr + 1] = (val >> 8) & 0xff;
          mem[addr + 2] = (val >> 16) & 0xff;
          mem[addr + 3] = (val >> 24) & 0xff;
          break;
        }
      }

      wire_out.wired = true;
      wire_out.output.dest = dest;
      if(19 <= type && type <= 23) wire_out.output.val = result;
      else wire_out.output.val = wire_in.input.val;
      wire_in.clk = -1;
    }

    void flush(ROB* rob) {
      if(wire_out.wired) {
        rob -> buffer[wire_out.output.dest].value = wire_out.output.val;
        rob -> buffer[wire_out.output.dest].state = kCommit;
        wire_out.wired = false;
      }
    }

    void resetMem() {
      reset = true;
      wire_in.clk = -1;
      wire_out.wired = false;
    }
};
#endif