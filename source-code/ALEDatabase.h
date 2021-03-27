// File: ALEDatabase.h
// Basic database for Assembly Language Emulator.

#ifndef ALEDatabase_Class
#define ALEDatabase_Class

#include <string>
#include <vector>
#include <map>

using namespace std;

class ALEDatabase {
    public:
        // Stores program data from given file and checks for compilation errors.
        ALEDatabase();

        // Destructor isn't needed.
        ~ALEDatabase();

        // Returns total number of instructions.
        int GetLineCount();

        // Returns instruction at index'th line as a vector. Each element in a vector
        // is individual component(E.g. 'R1', '=' or 'M[R2 + 3]').
        vector<string> GetLineAt(int index);

        // Prints index'th instruction.
        void PrintLine(int index);

        // Returns first instruction index of given function if it exists.
        int GetFunctionIndex(string function_name);

        // Parses given line and stores it in a vector.
        vector<string> ParseLine(string line);
    private:
        // Processes given line and returns it's vector representation. Or empty
        // vector if this line is not needed.
        vector<string> ProcessLine(string line);

        // Checks for instruction constraints and other compilation errors.
        bool InvalidLine(string line);

        // Using ALU, LOAD or STORE together is forbidden.
        bool CheckForInstrConstraint(string line);

        // Checks if the given line contains a comment and removes it from the string
        // and returns it, or returns the same string.
        string RemoveComment(string line);

        vector<vector<string>> program_data; // Instructions' storage.
        map<string, int> declared_functions; // Function names and their indices in given program.
};

#endif
