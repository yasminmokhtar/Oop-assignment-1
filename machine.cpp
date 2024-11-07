#include "Machine.h"

// Register Methods
Register::Register() {
    for (int i = 0; i < size; i++) {
        reg[i] = "00";
    }
}

string Register::getCell(int address) {
    if (address < 0 || address >= size) {
        throw out_of_range("Address out of range");
    }
    return reg[address];
}

void Register::setCell(int address, const string &value) {
    if (address < 0 || address >= size) {
        throw out_of_range("Address out of range");
    }
    reg[address] = value;
}

void Register::displayRegister() {
    for (int j = 0; j < size; j++) {
        cout << "Register " << hex << j << ": " << getCell(j) << endl;
    }
}

// ALU Methods
int ALU::hexToDec(const string &hex) {
    int decimalValue;
    stringstream ss;
    ss << hex;
    ss >> std::hex >> decimalValue;
    return decimalValue;
}

string ALU::decToHex(int dec) {
    stringstream ss;
    ss << hex << uppercase << dec;
    return ss.str();
}

void ALU::addInteger(int reg1, int reg2, int reg3, Register &reg) {
    int value1 = hexToDec(reg.getCell(reg2));
    int value2 = hexToDec(reg.getCell(reg3));
    int result = value1 + value2;
    reg.setCell(reg1, decToHex(result));
}

void ALU::addFloatingPoint(int reg1, int reg2, int reg3, Register &reg) {
    int value1 = hexToDec(reg.getCell(reg2));
    int value2 = hexToDec(reg.getCell(reg3));
    int result = value1 + value2;
    reg.setCell(reg1, decToHex(result));
}

// Memory Methods
Memory::Memory() {
    for (int i = 0; i < size; i++) {
        memory[i] = "00";
    }
}

string Memory::getCell(int address) {
    if (address < 0 || address >= size) {
        throw out_of_range("Address out of range");
    }
    return memory[address];
}

void Memory::setCell(int address, const string &value) {
    if (address < 0 || address >= size) {
        throw out_of_range("Address out of range");
    }
    memory[address] = value;
}

void Memory::displayMemory() {
    for (int i = 0; i < size / 2; i++) {
        cout << setw(2) << setfill('0') << hex << uppercase << i * 2 << " " << memory[i] << endl;
    }
}

// CU Methods
void CU::loadFromMemory(int idxReg, int intMem, Register &reg, Memory &mem) {
    string val = mem.getCell(intMem);
    reg.setCell(idxReg, val);
}

void CU::loadImmediate(int idxReg, const string &val, Register &reg) {
    reg.setCell(idxReg, val);
}

void CU::store(int idxReg, int idxMem, Register &reg, Memory &mem) {
    string val = reg.getCell(idxReg);
    mem.setCell(idxMem, val);
}

void CU::move(int idxRegSrc, int idxRegDest, Register &reg) {
    string val = reg.getCell(idxRegSrc);
    reg.setCell(idxRegDest, val);
}

void CU::addInteger(int idxReg, int idxSrc1, int idxSrc2, Register &reg, ALU &alu) {
    alu.addInteger(idxReg, idxSrc1, idxSrc2, reg);
}

void CU::addFloatingPoint(int idxReg, int idxSrc1, int idxSrc2, Register &reg, ALU &alu) {
    alu.addFloatingPoint(idxReg, idxSrc1, idxSrc2, reg);
}

void CU::jumpIfEqual(int idxReg, int addr, Register &reg, int &programCounter) {
    if (reg.getCell(idxReg) == reg.getCell(0)) {
        programCounter = addr;
    }
}

void CU::halt() {
    cout << "Execution halted." << endl;
    exit(0);
}

// CPU Methods
CPU::CPU() : programCounter(0), instructionRegister("0000") {}

void CPU::fetch(Memory &memory) {
    instructionRegister = memory.getCell(programCounter);
    programCounter++;
}

void CPU::decodeAndExecute(Memory &memory) {
    if (instructionRegister.length() < 4) {
        cout << "Error: Invalid instruction format (" << instructionRegister << ")\n";
        cu.halt();  // Halt on error
        return;
    }

    string opcode = instructionRegister.substr(0, 1);
    int regIndex = stoi(instructionRegister.substr(1, 1), nullptr, 16);
    int addrOrReg = stoi(instructionRegister.substr(2, 2), nullptr, 16);

    if (opcode == "1") {
        cu.loadFromMemory(regIndex, addrOrReg, reg, memory);
    } else if (opcode == "2") {
        cu.loadImmediate(regIndex, instructionRegister.substr(2, 2), reg);
    } else if (opcode == "3") {
        cu.store(regIndex, addrOrReg, reg, memory);
    } else if (opcode == "4") {
        cu.move(regIndex, addrOrReg, reg);
    } else if (opcode == "5") {
        cu.addInteger(regIndex, stoi(instructionRegister.substr(2, 1), nullptr, 16), stoi(instructionRegister.substr(3, 1), nullptr, 16), reg, alu);
    } else if (opcode == "6") {
        cu.addFloatingPoint(regIndex, stoi(instructionRegister.substr(2, 1), nullptr, 16), stoi(instructionRegister.substr(3, 1), nullptr, 16), reg, alu);
    } else if (opcode == "B") {
        cu.jumpIfEqual(regIndex, addrOrReg, reg, programCounter);
    } else if (opcode == "C") {
        cu.halt();
    } else {
        cout << "Invalid instruction: " << instructionRegister << endl;
    }
}

void CPU::displayState() {
    cout << "Program Counter: " << programCounter << endl;
    cout << "Instruction Register: " << instructionRegister << endl;
    reg.displayRegister();
}

// Machine Methods
void Machine::loadProgramFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file." << endl;
        return;
    }

    int address = 0;
    string instruction;
    while (file >> instruction) {
        memory.setCell(address++, instruction);  // Store each instruction in successive memory locations
    }

    file.close();
}

void Machine::run() {
    while (true) {
        processor.fetch(memory);
        processor.decodeAndExecute(memory);
        processor.displayState();
    }
}

void Machine::outputState() {
    cout << "==== Machine State ====" << endl;
    processor.displayState();
    cout << "Memory State:" << endl;
    memory.displayMemory();
    cout << "========================" << endl;
}

// MainUI Methods
void MainUI::displayMenu() {
    cout << "Vole Machine Simulator" << endl;
    cout << "1. Load Program File" << endl;
    cout << "2. Run Program" << endl;
    cout << "3. Display Machine State" << endl;
    cout << "4. Exit" << endl;
}

void MainUI::start() {
    while (true) {
        displayMenu();
        int choice;
        cin >> choice;
        if (choice == 1) {
            string filename;
            cout << "Enter filename: ";
            cin >> filename;
            machine.loadProgramFile(filename);
        } else if (choice == 2) {
            machine.run();
        } else if (choice == 3) {
            machine.outputState();
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice." << endl;
        }
    }
}
