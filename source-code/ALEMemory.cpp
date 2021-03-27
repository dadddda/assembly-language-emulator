// File: ALEMemory.cpp
// Register data and address space for Assembly Language Emulator.

#include <iostream>
#include "ALEMemory.h"
#include "ALEConstants.hpp"

ALEMemory::ALEMemory() {
    register_data[kStackPointer] = kSPInitValue;
}

ALEMemory::~ALEMemory() {
    // Destructor isn't needed.
}

void ALEMemory::PutRegValue(string reg, int value) {
    register_data[reg] = value;
}

int ALEMemory::GetRegValue(string reg) {
    if (register_data.find(reg) != register_data.end()) {
        return register_data[reg];
    } else {
        string err_msg = "> Register \"" + reg + "\" doesn't exist.";
        throw err_msg;
    }
}

int ALEMemory::ReadAddr(int address, int byte_count) {
    unsigned char byte_array[sizeof(int)];

    for (int i = 0; i < byte_count; i++) {
        if (address <= 0 || address + i >= kSPInitValue) {
            string err_msg = "> Accessed address is out of range.";
            throw err_msg;
        }

        if (address_space.find(address + i) == address_space.end()) {
            string err_msg = "> Accessed address isn't initialized.";
            throw err_msg;
        }

        byte_array[i] = address_space[address + i];
    }

    return *(int*)byte_array;
}

void ALEMemory::WriteAddr(int value, int address, int byte_count) {
    unsigned char byte_array[sizeof(int)];

    *(int*)byte_array = value;

    for (int i = 0; i < byte_count; i++) {
        if (address <= 0 || address + i >= kSPInitValue) {
            string err_msg = "> Accessed address is out of range.";
            throw err_msg;
        }

        address_space[address + i] = byte_array[i];
    }
}
