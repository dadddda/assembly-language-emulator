// File: ALEMain.cpp
// Main program of Assembly Language Emulator.

#include <iostream>
#include <chrono>
#include "ALEConstants.hpp"
#include "ALEDatabase.h"
#include "ALEMemory.h"

using namespace std::chrono; 

// Program emulation line-by-line is happening here.
void RunProgram(ALEDatabase* prog_data, ALEMemory* prog_memory, bool print_mode);

// Checks if kBranch array contains string 'a'.
bool Contains(string a);

// If we have branch instruction this code is performed.
int BranchInstr(vector<string> &line_data, ALEDatabase* prog_data, ALEMemory* prog_memory);

// If we have call instruction this code is performed.
int CallInstr(vector<string> &line_data, int saved_index, ALEDatabase* prog_data, ALEMemory* prog_memory);

// If we have jump instruction this code is performed.
int JumpInstr(vector<string> &line_data, ALEDatabase* prog_data, ALEMemory* prog_memory);

// Returns the value of given register or a number(E.g. 'R1', 'SP', '99'...),
// or the value calculated from given arithmetic operation(E.g. 'R1 + -10'),
// or from given instruction's right half of equation(E.g. 'R1 = R2 + 9') and also
// stores that value to the left register.
// Also performs store/load instruction if necessary(E.g. 'M[R1] = 1', 'R2 = M[SP]'...).
int Evaluate(vector<string> &line_data, ALEDatabase* prog_data, ALEMemory* prog_memory);

// Executes store instruction(E.g. 'M[R1] = R2', 'M[R1 + 10] = 9', 'M[R1] =.2 R2'...).
int StoreInstr(string dest, string source, int byte_count, ALEDatabase* prog_data, ALEMemory* prog_memory);

// Executes load instruction(E.g. 'R1 = M[R2]', 'R1 =.2 M[R2 + 9]'...).
int LoadInstr(string dest, string source, int byte_count, ALEDatabase* prog_data, ALEMemory* prog_memory);

// Extracts insides of M[*].
vector<string> GetContent(string component, ALEDatabase* prog_data);

// Evaluates specified arithmetic instruction.
int ALU(string left, string op, string right, ALEMemory* prog_memory);

// Main program.
int main() {
    int exit_status;
    ALEDatabase* prog_data = NULL;
    ALEMemory* prog_memory = NULL;

    try {
        prog_data = new ALEDatabase();
        prog_memory = new ALEMemory();
        bool print_mode = false;

        cout << "> Print lines(1 - yes, 0 - no): ";
        cin >> print_mode;
        
        auto start = high_resolution_clock::now();
        RunProgram(prog_data, prog_memory, print_mode);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "> Execution time: " << duration.count() << "ms" << endl;

        exit_status = EXIT_SUCCESS;
    } catch (string err_msg) {
        cout << err_msg << endl;
        exit_status = EXIT_FAILURE;
    }

    if (prog_data != NULL) delete(prog_data);
    if (prog_memory != NULL) delete(prog_memory);

    system("PAUSE"); // Windows only.
    return exit_status;
}

void RunProgram(ALEDatabase* prog_data, ALEMemory* prog_memory, bool print_mode) {
    int num_of_calls = 0;

    for (int i = 0; i < prog_data->GetLineCount();) {
        prog_memory->PutRegValue(kCurrInstrPointer, i * 4);

        if (print_mode) prog_data->PrintLine(i);

        vector<string> line_data = prog_data->GetLineAt(i);
        string identifier = prog_data->GetLineAt(i)[0];

        if (identifier == kReturn) {
            if (num_of_calls != 0) {
                int curr_stack_pointer = prog_memory->GetRegValue(kStackPointer);
                i = prog_memory->ReadAddr(curr_stack_pointer, sizeof(int)) / 4;
                curr_stack_pointer += 4;
                prog_memory->PutRegValue(kStackPointer, curr_stack_pointer);

                num_of_calls--;
                continue;
            } else {
                if (prog_memory->GetRegValue(kStackPointer) != kSPInitValue) {
                    string err_msg = "> Memory leak detected.";
                    throw err_msg;
                }

                int ret_value = prog_memory->GetRegValue(kRetValue);
                cout << "> Returned value: " << ret_value << endl;
                return;
            }
        } else if (identifier == kStackPointer 
                   || identifier == kRetValue
                   || identifier.find(kRegisterPrefix) != -1
                   || identifier.find(kMemAccessPrefix) != -1)
        {
            Evaluate(line_data, prog_data, prog_memory);
        } else if (identifier == kCall) {
            num_of_calls++;
            i = CallInstr(line_data, i + 1, prog_data, prog_memory);
            continue;
        } else if (identifier == kJump) {
            i = JumpInstr(line_data, prog_data, prog_memory);
            continue;
        } else if (Contains(identifier)) {
            i = BranchInstr(line_data, prog_data, prog_memory);
            continue;
        }
        
        i++;
    }
}

bool Contains(string a) {
    for (int i = 0; i < kBranchSize; i++) {
        if (a == kBranch[i]) return true;
    }

    return false;
}

int BranchInstr(vector<string> &line_data, ALEDatabase* prog_data, ALEMemory* prog_memory) {
    vector<string> first_segment(line_data.begin() + 1, line_data.begin() + 2);
    vector<string> second_segment(line_data.begin() + 2, line_data.begin() + 3);
    vector<string> third_segment(line_data.begin() + 3, line_data.end());

    string op = line_data[0];
    int left = Evaluate(first_segment, prog_data, prog_memory);
    int right = Evaluate(second_segment, prog_data, prog_memory);
    int jump_dest = Evaluate(third_segment, prog_data, prog_memory);
    
    bool result = false;
    
    if (op == kLessThan) {
        result = left < right;
    } else if (op == kLessEqual) {
        result = left <= right;
    } else if (op == kEqual) {
        result = left == right;
    } else if (op == kNotEqual) {
        result = left != right;
    } else if (op == kGreaterThan) {
        result = left > right;
    } else if (op == kGreaterEqual) {
        result = left >= right;
    }
    
    if (result) return jump_dest / 4;
    else return prog_memory->GetRegValue(kCurrInstrPointer) / 4 + 1;
}

int CallInstr(vector<string> &line_data, int saved_index, ALEDatabase* prog_data, ALEMemory* prog_memory) {
    int curr_stack_pointer = prog_memory->GetRegValue(kStackPointer);
    curr_stack_pointer -= 4;
    prog_memory->PutRegValue(kStackPointer, curr_stack_pointer);
    prog_memory->WriteAddr(saved_index * 4, curr_stack_pointer, sizeof(int));

    int function_index = prog_data->GetFunctionIndex(line_data[1]);
    return function_index;
}

int JumpInstr(vector<string> &line_data, ALEDatabase* prog_data, ALEMemory* prog_memory) {
    line_data.erase(line_data.begin());
    return Evaluate(line_data, prog_data, prog_memory) / 4;
}

int Evaluate(vector<string> &line_data, ALEDatabase* prog_data, ALEMemory* prog_memory) {
    // If we have only a number or a register.
    if (line_data.size() == 1) {
        if (isdigit(line_data[0][0]) || (line_data[0][0] == kALUOperators[1] && isdigit(line_data[0][1]))) {
            return stoi(line_data[0]);
        } else {
            return prog_memory->GetRegValue(line_data[0]);
        }
    }
    // If current line contains only one operator(E.g. '+', '-', '*' or '/').
    else if (line_data.size() == 3 && line_data[1] != "=") {
        string left = line_data[0];
        string op = line_data[1];
        string right = line_data[2];

        return ALU(left, op, right, prog_memory);
    }
    // If current line contains more than one operator(E.g. '=' and '+', '-', '*' or '/').
    else if (line_data.size() == 5) {
        string left = line_data[2];
        string op = line_data[3];
        string right = line_data[4];
        int value = ALU(left, op, right, prog_memory);
        prog_memory->PutRegValue(line_data[0], value);
        return value;
    }
    // If current line contains only one operator('=') or additional ".1" (1 byte) or ".2" (2 bytes).
    else {
        // Store(E.g. 'M[R1] = R2', 'M[R1 + 10] = 9', 'M[R1] =.2 R2'...).
        if (line_data[0].find(kMemAccessPrefix) != -1) {
            if (line_data.size() == 3) {
                return StoreInstr(line_data[0], line_data[2], sizeof(int), prog_data, prog_memory);
            } else {
                int byte_count = line_data[2][1] - '0';
                return StoreInstr(line_data[0], line_data[3], byte_count, prog_data, prog_memory);
            }
        }
        // Load(E.g. 'R1 = M[R2]'...).
        else if (line_data.size() == 3 && line_data[2].find(kMemAccessPrefix) != -1) {
            return LoadInstr(line_data[0], line_data[2], sizeof(int), prog_data, prog_memory);
        }
        // Load(E.g. 'R1 =.1 M[R2]'...).
        else if (line_data.size() == 4 && line_data[3].find(kMemAccessPrefix) != -1) {
            int byte_count = line_data[2][1] - '0';
            return LoadInstr(line_data[0], line_data[3], byte_count, prog_data, prog_memory);
        }
        // E.g 'R1 =.1 R2' or 'R1 =.2 64'.
        else if (line_data.size() == 4) {
            int value;
            if (isdigit(line_data[3][0]) || (line_data[3][0] == kALUOperators[1] && isdigit(line_data[3][1]))) {
                value = stoi(line_data[3]);
            } else {
                value = prog_memory->GetRegValue(line_data[3]);
            }

            prog_memory->PutRegValue(line_data[0], value);

            return value;
        }
        // E.g 'R1 = R2' or 'R1 = 64'.
        else if (isdigit(line_data[2][0]) || (line_data[2][0] == kALUOperators[1] && isdigit(line_data[2][1]))) {
            int value = stoi(line_data[2]);
            prog_memory->PutRegValue(line_data[0], value);

            return value;
        } else {
            int value = prog_memory->GetRegValue(line_data[2]);
            prog_memory->PutRegValue(line_data[0], value);

            return value;
        }
    }
}

int StoreInstr(string dest, string source, int byte_count, ALEDatabase* prog_data, ALEMemory* prog_memory) {
    vector<string> content_data = GetContent(dest, prog_data);
    int content_value = Evaluate(content_data, prog_data, prog_memory);

    int value;
    if (isdigit(source[0]) || (source[0] == kALUOperators[1] && isdigit(source[1]))) value = stoi(source);
    else value = prog_memory->GetRegValue(source);

    prog_memory->WriteAddr(value, content_value, byte_count);

    return value;
}

int LoadInstr(string dest, string source, int byte_count, ALEDatabase* prog_data, ALEMemory* prog_memory) {
    vector<string> content_data = GetContent(source, prog_data);
    int content_value = Evaluate(content_data, prog_data, prog_memory);

    int value = prog_memory->ReadAddr(content_value, byte_count);
    prog_memory->PutRegValue(dest, value);

    return value;
}

vector<string> GetContent(string component, ALEDatabase* prog_data) {
    string content = component.substr(2, component.find(kMemAccessClose) - component.find(kMemAccessOpen) - 1);

    return prog_data->ParseLine(content);
}

int ALU(string left, string op, string right, ALEMemory* prog_memory) {
    int left_value;
    int right_value;
    
    if ((isdigit(left[0]) || (left[0] == kALUOperators[1] && isdigit(left[1]))) 
        && (isdigit(right[0]) || (right[0] == kALUOperators[1] && isdigit(right[1])))) {
        left_value = stoi(left);
        right_value = stoi(right);
    } else if (isdigit(left[0]) || (left[0] == kALUOperators[1] && isdigit(left[1]))) {
        left_value = stoi(left);
        right_value = prog_memory->GetRegValue(right);
    } else if (isdigit(right[0]) || (right[0] == kALUOperators[1] && isdigit(right[1]))) {
        left_value = prog_memory->GetRegValue(left);
        right_value = stoi(right);
    } else {
        left_value = prog_memory->GetRegValue(left);
        right_value = prog_memory->GetRegValue(right);
    }

    if (op[0] == kALUOperators[0]) return left_value + right_value;
    else if (op[0] == kALUOperators[1]) return left_value - right_value;
    else if (op[0] == kALUOperators[2]) return left_value * right_value;
    else return left_value / right_value;
}
