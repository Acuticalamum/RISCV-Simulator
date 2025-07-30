#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP
#include<stdint.h>
#include<iostream>
#include<string>
enum InstructionType {
  ADD,//0
  SUB,//1
  AND,//2
  OR,//3
  XOR,//4
  SLL,//5
  SRL,//6
  SRA,//7
  SLT,//8
  SLTU,//9
  ADDI,//10
  ANDI,//11
  ORI,//12
  XORI,//13
  SLLI,//14
  SRLI,//15
  SRAI,//16
  SLTI,//17
  SLTIU,//18
  LB,//19
  LBU,//20
  LH,//21
  LHU,//22
  LW,//23
  SB,//24
  SH,//25
  SW,//26
  BEQ,//27
  BGE,//28
  BGEU,//29
  BLT,//30
  BLTU,//31
  BNE,//32
  JAL,//33
  JALR,//34
  AUIPC,//35
  LUI,//36
  EBREAK,//37
  ECALL,//38
  OTHER,
  HALT,
};

std::string to_string[] = {
  "ADD",//0
  "SUB",//1
  "AND",//2
  "OR",//3
  "XOR",//4
  "SLL",//5
  "SRL",//6
  "SRA",//7
  "SLT",//8
  "SLTU",//9
  "ADDI",//10
  "ANDI",//11
  "ORI",//12
  "XORI",//13
  "SLLI",//14
  "SRLI",//15
  "SRAI",//16
  "SLTI",//17
  "SLTIU",//18
  "LB",//19
  "LBU",//20
  "LH",//21
  "LHU",//22
  "LW",//23
  "SB",//24
  "SH",//25
  "SW",//26
  "BEQ",//27
  "BGE",//28
  "BGEU",//29
  "BLT",//30
  "BLTU",//31
  "BNE",//32
  "JAL",//33
  "JALR",//34
  "AUIPC",//35
  "LUI",//36
  "EBREAK",//37
  "ECALL",//38
  "OTHER",
  "HALT",
};

class Instruction {
public:
  InstructionType type;
  uint32_t rd;
  uint32_t rs1;
  uint32_t rs2;
  int32_t imm; 
  Instruction() = default;
  bool isR() {
    return type <= 9;
  }
  bool isI() {
    return 10 <= type && type <= 23;
  }
  bool isS() {
    return 24 <= type && type <= 26;
  }
  bool isB() {
    return 27 <= type && type <= 32;
  }
  void debug() {
    std::cout << "Type:" << to_string[type] << " rd:x" << rd << " rs1:x" << rs1 << " rs2:x" << rs2 << " imm:" << imm << "\n";
  }
};

#endif