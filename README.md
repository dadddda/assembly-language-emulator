# Assembly Language Emulator

ALE v1.0 is implemented using C++ and supports assembly language dialect used in Stanford CS107 - Programming Paradigms course.
The emulator offers few things which are covered below.

----

### ALE v1.0 supports:
* Only integer numbers(Both positive and negative).
* Negative numbers just using the minus symbol right before the number(E.g. ‘R1 + -180’). Note that if instruction contains only one ‘-‘ symbol it must be delimited with space from the number(E.g. ‘R1-10’ will not work, you must write ‘R1 - 10’ instead).
* 4 byte registers: R1, R2, R3...
* Special registers: PC, SP and RV.
* Memory access instruction: M[\*], where ‘\*’ can be a constant, register or arithemic operation(E.g. ‘SP + 4’).
* ALU arithmetic operations: “+-*/” which only work on whole numbers.
* Branch instructions: BLT, BLE, BEQ, BNE, BGT, BGE which compare two given values(constant or a register) and jump on the specified address if the comparison returns true.
* JUMP instruction followed by a constant, register or arithmetic operation(E.g. ‘PC - 32’).
* CALL insturction followed by a function name in angle brackets(E.g. ‘CALL \<function\>’). Note that the function must be declared in the file the same way, i.e. with angle brackets.
* User comments with the prefix semicolon: ‘;’(E.g. ‘; My comment.’).

### ALE v1.0 features:
* Emulator checks for instruction constraints. That means that ALU, load and store instructions can’t be 
used together, error will be thrown.
* Emulator also throws an error when using multiple ALU operators(E.g. ‘R1 = R2 + R3 - R4’).
* Emulator throws an error when the instructions starts with a number(E.g. ‘100 = R1’).
* Emulator throws an error if a register is used without initialization.
* Emulator throws an error if we use store or load instruction on an uninitialized address.
* Emulator throws an error if allocated memory is not deallocated before final RET instruction.

### ALE v1.0 error messages:
* *Compilation error at: “line”.* - “line” contains invalid code.
* *Function “\<function\>” doesn't exist.* - “\<function\>” isn’t declared in the program.
* *Redeclaration of function “\<function\>”.* - “\<function\>” is declared twice or more in the program.
* *Register “reg” doesn't exist.* - “reg” isn’t initialized in the program.
* *Accessed address is out of range.* - If the address is negative or less than the value stored in SP register.
* *Accessed address isn't initialized.* - If there is nothing written on the accessed address.
* *Memory leak detected.* - If the allocated memory isn’t deallocated fully before the final RET instruction.

----

### Build instructions:
`cd` to the 'source-code' folder and execute:
```cmd
> g++ *.cpp
```
