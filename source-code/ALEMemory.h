// File: ALEMemory.h
// Register data and address space for Assembly Language Emulator.

#ifndef ALEMemory_Class
#define ALEMemory_Class

#include <string>
#include <map>

using namespace std;

class ALEMemory {
    public:
        // Initializes register data and address space.
        ALEMemory();

        // Destructor isn't needed.
        ~ALEMemory();

        // Assigns given value to given register.
        void PutRegValue(string reg, int value);

        // Returns a value from given register if it's initialized.
        int GetRegValue(string reg);

        // Returns a 'byte_count' length data from the given address if it's initialized.
        int ReadAddr(int address, int byte_count);

        // Writes 'byte_count' length data of given value into given address.
        void WriteAddr(int value, int address, int byte_count);
    private:
        map<string, int> register_data; // Emulation of register memory.
        map<int, unsigned char> address_space; // Emulation of stack memory.
};

#endif
