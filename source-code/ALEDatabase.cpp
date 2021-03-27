// File: ALEDatabase.cpp
// Basic database for Assembly Language Emulator.

#include <iostream>
#include <fstream>
#include "ALEDatabase.h"
#include "ALEConstants.hpp"

ALEDatabase::ALEDatabase() {
    ifstream program_file;
    
    while (true) {
        string file_name;
        cout << "> Program file: ";
        cin >> file_name;
        
        program_file.open(file_name);

        if (program_file) break;
        else cout << "> File not found." << endl;
    }
    
    string line;
    while (getline(program_file, line)) {
        if (InvalidLine(line)) {
            program_file.close();

            string err_msg = "> Compilation error at: \"" + line + "\".";
            throw err_msg;
        }
        
        vector<string> line_data = ProcessLine(line);
        if (line_data.size() == 0) continue;

        program_data.push_back(line_data);
    }

    program_file.close();
}

ALEDatabase::~ALEDatabase() {
    // Destructor isn't needed.
}

int ALEDatabase::GetLineCount() {
    return program_data.size();
}

vector<string> ALEDatabase::GetLineAt(int index) {
    return program_data[index];
}

void ALEDatabase::PrintLine(int index) {
    vector<string> line_data = program_data[index];

    cout << index * 4 << ": ";

    for (int i = 0; i < line_data.size(); i++) {
        cout << line_data[i];
        if (i != line_data.size() - 1) cout << " ";
    }
    cout << endl;
}

int ALEDatabase::GetFunctionIndex(string function_name) {
    if (declared_functions.find(function_name) != declared_functions.end()) {
        return declared_functions[function_name];
    } else {
        string err_msg = "> Function \"" + function_name + "\" doesn't exist.";
        throw err_msg;
    }
}

vector<string> ALEDatabase::ParseLine(string line) {
    vector<string> line_data;

    string component;
    bool open = false;
    for (int i = 0; i < line.length(); i++) {
        if (line.find(kMemAccessPrefix) == i) open = true;

        if (kIgnoreDelims.find(line[i]) != -1 && !open) {
            if (component.length() != 0) line_data.push_back(component);
            component = "";
        } else if (kIgnoreDelims.find(line[i]) != -1) {
            if (component[component.length() - 1] != line[i]) component += line[i];
        } else if (kAllDelims.find(line[i]) != -1 && !(line[i] == kALUOperators[1] && isdigit(line[i + 1])) && !open) {
            if (component.length() != 0) line_data.push_back(component);
            component = line[i];
            line_data.push_back(component);
            component = "";
        } else if (i == line.length() - 1) {
            component += line[i];
            line_data.push_back(component);
        } else {
            component += line[i];
        }

        if (line.find(kMemAccessClose) == i) open = false;
    }

    return line_data;
}

vector<string> ALEDatabase::ProcessLine(string line) {
    line = RemoveComment(line);
    if (line.length() == 0) return vector<string>();
    
    vector<string> line_data = ParseLine(line);
    if (line_data.size() == 0) return vector<string>();

    if (line_data[0].find(kFuncDeclOpen) != -1 && line_data[0].find(kFuncDeclClose) != -1) {
        if (declared_functions.find(line_data[0]) == declared_functions.end()) {
            declared_functions[line_data[0]] = program_data.size();
            return vector<string>();
        } else {
            string err_msg = "> Redeclaration of function \"" + line_data[0] + "\".";
            throw err_msg;
        }
    }

    return line_data;
}

bool ALEDatabase::InvalidLine(string line) {
    if (!CheckForInstrConstraint(line)) {
        return true;
    } else if (line.length() > 0 && isdigit(line[0])) {
        return true;
    }

    return false;
}

bool ALEDatabase::CheckForInstrConstraint(string line) {
    if (line.find(kMemAccessPrefix) != -1 
        && line.find(kMemAccessPrefix, line.find(kMemAccessPrefix) + 1) != -1) return false;
    
    int num_ALUs = 0;
    for (int i = 0; i < line.length(); i++) {
        if (kALUOperators.find(line[i]) != -1 && !(line[i] == kALUOperators[1] && isdigit(line[i + 1]))) num_ALUs++;
    }

    if (num_ALUs > 1) return false;
    else if (num_ALUs == 0) return true;

    if (line.find(kMemAccessPrefix) != -1) {
        int left_bracket_index = line.find(kMemAccessOpen);
        int right_bracket_index = line.find(kMemAccessClose);
        
        if (left_bracket_index < line.find(kALUOperators[0]) 
            && right_bracket_index > line.find(kALUOperators[0])) {
            return true;
        } else if (left_bracket_index < line.find(kALUOperators[1]) 
                   && right_bracket_index > line.find(kALUOperators[1])) {
            return true;
        } else if (left_bracket_index < line.find(kALUOperators[2]) 
                   && right_bracket_index > line.find(kALUOperators[2])) {
            return true;
        } else if (left_bracket_index < line.find(kALUOperators[3]) 
                   && right_bracket_index > line.find(kALUOperators[3])) {
            return true;
        } else {
            return false;
        }
    }
    
    return true;
}

string ALEDatabase::RemoveComment(string line) {
    if (line.find(kCommentPrefix) == -1) return line;
    
    return line.substr(0, line.find(kCommentPrefix));
}
