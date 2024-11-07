#ifndef MACHINE_H
#define MACHINE_H

#include <bits/stdc++.h>
using namespace std;

// Register: Holds data for temporary computation
class Register {
private:
    string reg[16];
    const int size = 16;

public:
    Register();
    string getCell(int address);
    void setCell(int address, const string &value);
    void displayRegister();
};

// ALU: Handles arithmetic and logical operations
class ALU {
public:
    int hexToDec(const string &hex);
    string decToHex(int dec);
    void addInteger(int reg1, int reg2, int reg3, Register &reg);
    void addFloatingPoint(int reg1, int reg2, int reg3, Register &reg);
};

// Memory: Holds program data and instructions
class Memory {
private:
    string memory[256];
    const int size = 256;

public:
    Memory();
    string getCell(int address);
    void setCell(int address, const string &value);
    void displayMemory();
};

// CU (Control Unit): Decodes and executes instructions
class CU {
public:
    void loadFromMemory(int idxReg, int intMem, Register &reg, Memory &mem);
    void loadImmediate(int idxReg, const string &val, Register &reg);
    void store(int idxReg, int idxMem, Register &reg, Memory &mem);
    void move(int idxRegSrc, int idxRegDest, Register &reg);
    void addInteger(int idxReg, int idxSrc1, int idxSrc2, Register &reg, ALU &alu);
    void addFloatingPoint(int idxReg, int idxSrc1, int idxSrc2, Register &reg, ALU &alu);
    void jumpIfEqual(int idxReg, int addr, Register &reg, int &programCounter);
    void halt();
};

// CPU: Executes the instructions step-by-step
class CPU {
private:
    int programCounter;
    string instructionRegister;
    Register reg;
    ALU alu;
    CU cu;

public:
    CPU();
    void fetch(Memory &memory);
    void decodeAndExecute(Memory &memory);
    void displayState();
};

// Machine: Main simulator that loads programs and executes them
class Machine {
private:
    CPU processor;
    Memory memory;

public:
    void loadProgramFile(const string &filename);
    void run();
    void outputState();
};

// Main UI: Menu-driven interface for the simulator
class MainUI {
private:
    Machine machine;

public:
    void displayMenu();
    void start();
};

#endif // MACHINE_H