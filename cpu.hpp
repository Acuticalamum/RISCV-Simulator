#ifndef CPU_HPP
#define CPU_HPP
#include "alu.hpp"
#include "memory.hpp"
#include "decoder.hpp"
#include "register.hpp"
#include "rob.hpp"
#include "rs.hpp"
#include "lsb.hpp"
#include "predictor.hpp"

class CPU {
  private:
    ALU alu;
    Memory memory;
    RegisterFile rf;
    Decoder decoder;
    ROB rob;
    RS rs;
    LSB lsb;
    Predictor predictor;
    uint32_t tem_pc = 0;
    uint32_t real_pc = 0;
    uint32_t clk = 0;
    bool pre[32];
    uint8_t pres = 0;
    bool hang = false;
    bool halt = false;
  public:

    CPU() {
      memory.readIn();
    }

    bool run(uint8_t &output) {
      ++clk;
      if(halt) {
        output = rf.reg[10] & 0xff;
        return false;
      }
      launch();
      flush();
      commit();
      tick();
      //std::cout << "reg1=" << rf.reg[1] << std::endl;
      //std::cout << "tem_pc=" << tem_pc << std::endl;
      //std::cout << "real_pc=" << real_pc << std::endl;
      return true;
    }

  private:
    uint32_t instructionFetch() {
      uint32_t raw_code = memory.mem[tem_pc + 3];
      raw_code = (raw_code << 8) | memory.mem[tem_pc + 2];
      raw_code = (raw_code << 8) | memory.mem[tem_pc + 1];
      raw_code = (raw_code << 8) | memory.mem[tem_pc];
      //std::cout << raw_code << std::endl;
      return raw_code;
    }
    void launch() {
      if(memory.reset || hang) return;
      Instruction instruction = decoder.decode(instructionFetch());

      //std::cout << std::hex << tem_pc << std::endl;
      //std::cout << std::dec;

      //instruction.debug();

      if(!rob.full()) {
        ROBItem rob_input(instruction, tem_pc);
        uint8_t rob_id = rob.tail();
        
        if(instruction.type == HALT) {
          hang = true;
          rob_input.dest = -1;
          rob_input.state = kCommit;
          rob_input.value = 0;
          rob.wire(rob_input);
        }

        else if(instruction.type == AUIPC) {//No need for rs
          rob_input.dest = instruction.rd;
          rob_input.state = kCommit;
          rob_input.value = tem_pc + instruction.imm;
          tem_pc += 4;
          if(instruction.rd) rf.dependency[instruction.rd] = rob_id;
          rob.wire(rob_input);
        }

        else if(instruction.type == LUI) {
          rob_input.dest = instruction.rd;
          rob_input.state = kCommit;
          rob_input.value = instruction.imm;
          tem_pc += 4;
          if(instruction.rd) rf.dependency[instruction.rd] = rob_id;
          rob.wire(rob_input);
        }

        else if(instruction.type == JAL) {
          rob_input.dest = instruction.rd;
          rob_input.state = kCommit;
          rob_input.value = instruction.imm;
          tem_pc += instruction.imm;
          if(instruction.rd) rf.dependency[instruction.rd] = rob_id;
          rob.wire(rob_input);
        }
        
        else if(!rs.wire_in.wired) {//Need rs
          RSItem rs_input(instruction, rob_id);
          uint32_t rs1 = instruction.rs1;
          uint32_t rs2 = instruction.rs2;
          int32_t imm = instruction.imm;
          uint32_t rd = instruction.rd;

          if(instruction.isR()) {
            rob_input.dest = rd;
            rs_input.imm = imm;
            rs_input.fill(rf, rs1, rs2);
            if(rd) rf.dependency[rd] = rob_id;
            tem_pc += 4;
          }

          if(instruction.isI()) {
            rob_input.dest = rd;
            rs_input.imm = imm;
            rs_input.fill(rf, rs1);
            if(rd) rf.dependency[rd] = rob_id;
            tem_pc += 4;
          }

          if(instruction.isS()) {
            rob_input.dest = -1;
            rs_input.imm = imm;
            rs_input.fill(rf, rs1, rs2);
            tem_pc += 4;
          }

          if(instruction.isB()) {
            rob_input.dest = imm;
            rs_input.imm = imm;
            rs_input.fill(rf, rs1, rs2);
            if(predictor.jump(tem_pc)) {
              pre[pres++] = true;
              tem_pc += imm;
            }
            else {
              pre[pres++] = false;
              tem_pc += 4;
            }
          }

          if(instruction.type == JALR) {
            rob_input.dest = rd;
            rs_input.imm = imm;
            rs_input.fill(rf, rs1);
            if(rd) rf.dependency[rd] = rob_id;
            hang = true;
          }

          rs.wire(rs_input);
          rob.wire(rob_input);
        }
      }
    }

    void flush() {
      rs.flush(&alu, &lsb);
      lsb.flush(&memory);
      alu.flush(&rob);
      memory.flush(&rob);
    }

    void commit() {
      if(!rob.wire_out.wired) return;
      ROB::Output& output = rob.wire_out.output;
      InstructionType type = output.info.instruction.type;
      rob.wire_out.wired = false;

      for(int i = 0; i < 32; i++) {
        if(rf.dependency[i] == output.num) {
          rf.dependency[i] = -1;
        }
      }

      if(rs.wire_in.input.qj == output.num) {
        rs.wire_in.input.qj = -1;
        rs.wire_in.input.vj = 
        (type == JAL || type == JALR) ? real_pc + 4 : output.info.value;
        //std::cout << to_string[type] << std::endl;
        //std::cout << output.info.value << std::endl;
      }

      if(rs.wire_in.input.qk == output.num) {
        rs.wire_in.input.qk = -1;
        rs.wire_in.input.vk = 
        (type == JAL || type == JALR) ? real_pc + 4 : output.info.value;
      }

      for(int i = 0; i < rs.size; i++) {
        if(rs.data[i].qj == output.num) {
          rs.data[i].qj = -1;
          rs.data[i].vj = 
          (type == JAL || type == JALR) ? real_pc + 4 : output.info.value;
        }
        if(rs.data[i].qk == output.num) {
          rs.data[i].qk = -1;
          rs.data[i].vk = 
          (type == JAL || type == JALR) ? real_pc + 4 : output.info.value;
        }
      }

      if(output.info.instruction.isS()) {
        real_pc += 4;
      }

      else if(output.info.instruction.isR() || output.info.instruction.isI()) {
        if(type == LW) {
          //std::cout << "LWFinished!!!  " << output.info.dest << ' ' << output.info.value << std::endl;
          //std::cout << "Mem[131068]=" << (int32_t)memory.mem[131068] << std::endl;
        }
        if(output.info.dest) rf.reg[output.info.dest] = output.info.value;
        real_pc += 4;
      }

      else if(type == LUI) {
        if(output.info.dest) rf.reg[output.info.dest] = output.info.value;
        real_pc += 4;
      }

      else if(type == AUIPC) {
        if(output.info.dest) rf.reg[output.info.dest] = real_pc + output.info.value;
        real_pc += 4;
      }

      else if(type == JAL) {
        if(output.info.dest) rf.reg[output.info.dest] = real_pc + 4;
        real_pc += output.info.value;
        if(output.info.pc == 0x1098) {
          //puts("jal!!!");
          //std::cout << rf.reg[output.info.dest] << std::endl;
        }
        //std::cout << real_pc << std::endl;
      }

      else if(type == JALR) {
        if(output.info.dest) rf.reg[output.info.dest] = real_pc + 4;
        /*if(output.info.pc == 0x1014) {
          puts("jalr!!!!");
          std::cout << output.info.value << std::endl;
        }*/
        real_pc = tem_pc = output.info.value;
        hang = false;
      }

      else if(type == HALT) {
        halt = true;
      }

      else if(output.info.instruction.isB()) {
        if(output.info.value) {
          real_pc += output.info.dest;
          predictor.add(output.info.pc);
          if(pre[0]) {
            predictor.suc(output.info.pc);
            for(int i = 0; i < pres - 1; i++) pre[i] = pre[i + 1];
            --pres;
          } else {
            clear();
          }
        }
        if(!output.info.value) {
          real_pc += 4;
          predictor.add(output.info.pc);
          if(!pre[0]) {
            predictor.suc(output.info.pc);
            for(int i = 0; i < pres - 1; i++) pre[i] = pre[i + 1];
            --pres;
          } else {
            clear();
          }
        }
      }

    }

    void clear() {
      alu.clear();
      rob.clear();
      rs.clear();
      lsb.clear();
      rf.clear();
      memory.resetMem();
      tem_pc = real_pc;
      hang = false;
      pres = 0;
    }

    void tick() {
      if(!memory.reset) {
        alu.run();
        lsb.run();
        rob.run();
        rs.run();
      }
      memory.run();
    }
};
#endif