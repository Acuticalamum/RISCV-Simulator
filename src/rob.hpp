#ifndef ROB_HPP
#define ROB_HPP
#include"instruction.hpp"
#include"circularqueue.hpp"
enum State {
  kIssued,
  kExecuting,
  kCommit,
};
class ROBItem {
  public: 
    bool busy;
    Instruction instruction;
    State state = kIssued;
    uint32_t dest;
    uint32_t value;
};
class ROB {
  public:
    class WireIn {
      public:
        ROBItem input;
        bool wired = false;
    } wire_in;

    class Output {
      public:
        ROBItem info;
        uint8_t num;
        Output() = default;
    };

    class WireOut {
      public:
        Output output;
        bool wired = false;
    } wire_out;

    const static uint8_t ROB_SIZE = 8;
    CircularQueue<ROBItem, ROB_SIZE> buffer;
    
    bool full() {
      return buffer.full();
    }

    bool empty() {
      return buffer.empty();
    }

    void run() {
      if(wire_in.wired) {
        if(!full()) {
          wire_in.wired = false;
          buffer.push(wire_in.input);
        }
      }
      if(!wire_out.wired) {
        if(!empty()) {
          wire_out.wired = true;
          wire_out.output.info = buffer.front();
          wire_out.output.num = buffer.head;
          buffer.pop();
        }
      }
    }

    void clear() {
      wire_in.wired = wire_out.wired = false;
      buffer.clear();
    }
};
#endif