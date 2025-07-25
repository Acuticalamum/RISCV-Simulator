#ifndef DECODER_HPP
#define DECODER_HPP
#include "instruction.hpp"
class Decoder {
  public:
  Instruction decode(uint32_t raw_code) {
    Instruction ret;

    uint8_t opcode = raw_code & 0x7f;
    if(opcode == 0x33) { // R type
      uint8_t rd = (raw_code >> 7)  & 0x1f;
      uint8_t funct3 = (raw_code >> 12) & 0x07;
      uint8_t rs1 = (raw_code >> 15) & 0x1f;
      uint8_t rs2 = (raw_code >> 20) & 0x1f;
      uint8_t funct7 = (raw_code >> 25) & 0x7f;

      ret.rd = rd;
      ret.rs1 = rs1;
      ret.rs2 = rs2;

      if(funct7 == 0x00) {
        if(funct3 == 0x00) ret.type = ADD;
        else if(funct3 == 0x01) ret.type = SLL;
        else if(funct3 == 0x02) ret.type = SLT;
        else if(funct3 == 0x03) ret.type = SLTU;
        else if(funct3 == 0x04) ret.type = XOR;
        else if(funct3 == 0x05) ret.type = SRL;
        else if(funct3 == 0x06) ret.type = OR;
        else if(funct3 == 0x07) ret.type = AND;
      }
      else if(funct7 == 0x20) {
        if(funct3 == 0x00) ret.type = SUB;
        else if(funct3 == 0x05) ret.type = SRA;
      }
    }

    else if(opcode == 0x13) {//I & I* type except load
      uint8_t rd = (raw_code >> 7) & 0x1f;
      uint8_t funct3 = (raw_code >> 12) & 0x07;
      uint8_t rs1 = (raw_code >> 15) & 0x1f;
      int32_t imm = (int32_t)(raw_code) >> 20;
      imm = (imm << 20) >> 20;

      ret.rd = rd;
      ret.rs1 = rs1;
      ret.imm = imm;
      
      if(funct3 == 0x00) ret.type = ADDI;
      else if(funct3 == 0x07) ret.type = ANDI;
      else if(funct3 == 0x06) ret.type = ORI;
      else if(funct3 == 0x04) ret.type = XORI;
      else if(funct3 == 0x02) ret.type = SLTI;
      else if(funct3 == 0x03) ret.type = SLTIU;
      else if(funct3 == 0x01) {
        ret.imm = (raw_code >> 20) & 0x1f;
        ret.type = SLLI;
      }
      else if(funct3 == 0x05) {
        ret.imm = (raw_code >> 20) & 0x1f;
        uint8_t funct7 = (raw_code >> 25) & 0x7f;
        if(funct7 == 0x00) ret.type = SRLI;
        else if (funct7 == 0x20) ret.type = SRAI; 
      }
    }

    else if(opcode == 0x03) {//I type load
      uint8_t rd = (raw_code >> 7) & 0x1f;
      uint8_t funct3 = (raw_code >> 12) & 0x07;
      uint8_t rs1 = (raw_code >> 15) & 0x1f;
      int32_t imm = (int32_t)(raw_code) >> 20;
      imm = (imm << 20) >> 20;

      ret.rd = rd;
      ret.rs1 = rs1;
      ret.imm = imm;
      
      if(funct3 == 0x00) ret.type = LB;
      else if(funct3 == 0x04) ret.type = LBU;
      else if(funct3 == 0x01) ret.type = LH;
      else if(funct3 == 0x05) ret.type = LHU;
      else if(funct3 == 0x02) ret.type = LW;
    }

    else if(opcode == 0x23) {//S type
      uint8_t funct3 = (raw_code >> 12) & 0x07;
      uint8_t rs1 = (raw_code >> 15) & 0x1f;
      uint8_t rs2 = (raw_code >> 20) & 0x1f;
      uint8_t imm_4_0 = (raw_code >> 7) & 0x1f;
      uint8_t imm_11_5 = (raw_code >> 25) & 0x7f;
      int32_t imm = (imm_11_5 << 5) | imm_4_0;
      if(imm & 0x800) imm |= 0xfffff000;

      ret.rs1 = rs1;
      ret.rs2 = rs2;
      ret.imm = imm;
      
      if(funct3 == 0x00) ret.type = SB;
      else if(funct3 == 0x01) ret.type = SH;
      else if(funct3 == 0x02) ret.type = SW;
    }

    else if(opcode == 0x63) {//B type
      uint8_t funct3 = (raw_code >> 12) & 0x07;
      uint8_t rs1 = (raw_code >> 15) & 0x1f;
      uint8_t rs2 = (raw_code >> 20) & 0x1f;
      int32_t imm12 = (raw_code >> 31) & 0x01;
      int32_t imm10_5 = (raw_code >> 25) & 0x3f;
      int32_t imm4_1 = (raw_code >> 8) & 0x0f;
      int32_t imm11 = (raw_code >> 7) & 0x01;
      int32_t imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);
      if(imm & 0x1000) imm |= 0xffffe000;
      
      ret.rs1 = rs1;
      ret.rs2 = rs2;
      ret.imm = imm;

      if(funct3 == 0x00) ret.type = BEQ;
      else if(funct3 == 0x05) ret.type = BGE;
      else if(funct3 == 0x07) ret.type = BGEU;
      else if(funct3 == 0x04) ret.type = BLT;
      else if(funct3 == 0x06) ret.type = BLTU;
      else if(funct3 == 0x01) ret.type = BNE;
    }

    else if(opcode == 0x6f) {//Jal
      int32_t imm20 = (raw_code >> 31) & 0x01;
      int32_t imm10_1 = (raw_code >> 21) & 0x03ff;
      int32_t imm11 = (raw_code >> 20) & 0x01;
      int32_t imm19_12 = (raw_code >> 12) & 0xff;
      int32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
      if (imm20) imm |= 0xfff00000;
      uint8_t rd = (raw_code >> 7) & 0x1f;

      ret.rd = rd;
      ret.imm = imm;
      ret.type = JAL;
    }

    else if(opcode == 0x67) {//Jalr
      int32_t imm = (int32_t)(raw_code >> 20);
      imm = (imm << 20) >> 20;
      uint8_t rs1 = (raw_code >> 15) & 0x1f;
      uint8_t funct3 = (raw_code >> 12) & 0x07;
      uint8_t rd = (raw_code >> 7) & 0x1f;
      
      if(funct3 == 0x00) {
        ret.rd = rd;
        ret.rs1 = rs1;
        ret.imm = imm;
        ret.type = JALR;
      }
    }

    else if(opcode == 0x17) {//AUIPC
      uint8_t rd = (raw_code >> 7) & 0x1F;
      int32_t imm = raw_code & 0xfffff000;

      ret.rd = rd;
      ret.imm = imm;
      ret.type = AUIPC;
    }

    else if(opcode == 0x37) {//LUI
      uint8_t rd = (raw_code >> 7) & 0x1F;
      int32_t imm = raw_code & 0xfffff000;
      
      ret.rd = rd;
      ret.imm = imm;
      ret.type = AUIPC;
    }

    else if(opcode == 0x73) {//EBREAK & ECALL
      uint8_t funct3 = (raw_code >> 12) & 0x07;
      uint32_t imm = raw_code >> 20;
      if(funct3 == 0x00) {
        if(imm == 0x00) ret.type = EBREAK;
        if(imm == 0x01) ret.type = ECALL;
      }
    }

    return ret;
  }
};
#endif