// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

// R2 is result, set initially to zero
@R2
M=0 

// first step: number has to be greater than 0 otherwise it will always = 0
// checks contents of R0 is > 0, jumps to adding section
@R0 	
D=M
@STEP
D;JGT

// number < 0, ends loop before it starts
@END
0;JMP

// Multiplication is adding same number to itself so many times
// 10*3 is the same as 10+10+10 or 3+3+3+3+3+3+3+3+3+3
// if R0 is 10, R1 is 3, R2 is currently set to 0
// add R1 to R2 subtract 1 from R0
 
(STEP)
// get current value of R2
@R2
D=M

// add R1 to R2
@R1
D=D+M

// store back in R2
@R2
M=D

// subtract 1 from R0
@R0
D=M-1
M=D

// if R0 > 0 go again
@STEP
D;JGT

(END)
@END
0;JMP