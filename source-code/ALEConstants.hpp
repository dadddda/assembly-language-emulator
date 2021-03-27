// File: ALEConstants.h
// Basic constants of Assembly Language Emulator.

#ifndef ALEConstants_Interface
#define ALEConstants_Interface

#include <string>
#include <limits.h>

using namespace std;

// For ALEDatabase and ALEMain:
    // Prefix for a comment.
    const string kCommentPrefix = ";";

    // Brackets for function declaration;
    const string kFuncDeclOpen = "<";
    const string kFuncDeclClose = ">";

    // Basic register prefix.
    const string kRegisterPrefix = "R";

    // Special registers.
    const string kCurrInstrPointer = "PC";
    const string kStackPointer = "SP";
    const string kRetValue = "RV";

    // Memory access brackets and prefix.
    const string kMemAccessOpen = "[";
    const string kMemAccessClose = "]";
    const string kMemAccessPrefix = "M" + kMemAccessOpen;

    // Arithmetic operations.
    const string kALUOperators = "+-*/";

    // Branch instructions and divider.
    const string kLessThan = "BLT";
    const string kLessEqual = "BLE";
    const string kEqual = "BEQ";
    const string kNotEqual = "BNE";
    const string kGreaterThan = "BGT";
    const string kGreaterEqual = "BGE";
    const string kBranch[] = {kLessThan, kLessEqual, kEqual, kNotEqual, kGreaterThan, kGreaterEqual};
    const int kBranchSize = 6;

    // Jump instruction.
    const string kJump = "JUMP";

    // Call instruction.
    const string kCall = "CALL";

    // Return instruction.
    const string kReturn = "RET";

    // Delimiters for the tokenizer.
    const string kAllDelims = "+-*/=, ";
    const string kIgnoreDelims = ", ";

// For ALEMemory:
    // Initial value of the register 'SP'.
    const int kSPInitValue = INT_MAX - 3;

#endif
