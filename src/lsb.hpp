#ifndef LSB_HPP
#define LSB_HPP
#include"instruction.hpp"
#include"memory.hpp"
class LSBItem {
  public:
  InstructionType type;
  uint32_t dest;
  uint32_t addr;
  uint32_t val;
  LSBItem() = default;
  LSBItem(InstructionType type_,  uint32_t dest_, uint32_t addr_,  uint32_t val_) : 
  type(type_), dest(dest_), addr(addr_), val(val_) {}
};
class LSB {
  public:
  class WireIn {
    public:
      LSBItem input;
      bool wired = false;
      void wire(LSBItem& input_) {
        if(!wired) {
          wired = true;
          input = input_;
        }
      }
  } wire_in;

  class WireOut {
    public:
      LSBItem output;
      bool wired = false;
  } wire_out;

  static const uint8_t LSB_SIZE = 8;
  LSBItem data[LSB_SIZE];
  uint8_t size = 0;

  LSB() = default;

  bool empty() {
    return size == 0;
  }

  bool full() {
    return size == LSB_SIZE;
  }

  void wire(LSBItem& input_) {
    wire_in.wire(input_);
  }

  void run() {
    if(wire_in.wired && !full()) {
      wire_in.wired = false;
      data[size++] = wire_in.input;
    }

    if(!wire_out.wired && !empty()) {
      wire_out.wired = true;
      wire_out.output = data[0];
      for(int i = 0; i < size - 1; i++) {
        data[i] = data[i + 1];
      }
      --size;
    }
  }

  void clear() {
    wire_in.wired = wire_out.wired = false;
    size = 0;
  }

  void flush(Memory* memory) {
    if(wire_out.wired) {
      if(memory -> wire_in.clk == -1) {
        wire_out.wired = false;
        //puts("Begin lsb");
        //std::cout << to_string[wire_out.output.type] << ' ' 
                  //<< wire_out.output.dest << ' ' << wire_out.output.addr << ' ' << wire_out.output.val << std::endl;
        Memory::Input memory_input(
            wire_out.output.type, wire_out.output.dest, wire_out.output.addr, wire_out.output.val);
        memory -> wire(memory_input);
      }
    }
  }

};
#endif