#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP
#include<stdint.h>
enum InstructionType {
  ADD,
  SUB,
  AND,
  OR,
  XOR,
  SLL,
  SRL,
  SRA,
  SLT,
  SLTU,
  ADDI,
  ANDI,
  ORI,
  XORI,
  SLLI,
  SRLI,
  SRAI,
  SLTI,
  SLTIU,
  LB,
  LBU,
  LH,
  LHU,
  LW,
  SB,
  SH,
  SW,
  BEQ,
  BGE,
  BGEU,
  BLT,
  BLTU,
  BNE,
  JAL,
  JALR,
  AUIPC,
  LUI,
  EBREAK,
  ECALL,
};

class Instruction {
public:
  InstructionType type;
  uint32_t rd;
  uint32_t rs1;
  uint32_t rs2;
  int32_t imm; 
  Instruction() = default;
};

#endif