; Test program.

SP = SP - 8
M[SP + 4] = -180
M[SP] = 7
CALL <remainder>
M[SP + 4] = RV
M[SP] = 0
R1 = M[SP]
BGE R1, 10000, PC + 32
R1 = M[SP + 4]
R1 = R1 + 1
M[SP + 4] = R1
R1 = M[SP]
R1 = R1 + 1
M[SP] = R1
JUMP PC - 32
RV = M[SP + 4]
SP = SP + 8
RET

<remainder>
R1 = M[SP + 8]
R2 = M[SP + 4]
R3 = R1 / R2
R2 = R2 * R3
RV = R1 - R2
RET
