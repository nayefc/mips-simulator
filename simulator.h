/*
  
 * simulator.h
 * 
 * Simulator header file declaring variables and methods 
 *
 * author: Nayef Copty
 * 11/11/11
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

// Define MAX_LENGTH for number of registers & instruction length
#define MAX_LENGTH 32

// Array of size MAX_LENGTH to hold registers
int registers[MAX_LENGTH];

// PC Register
unsigned int pc = 0;

// Memory Allocated 
unsigned int *memory;

void parse_instruction(unsigned int sum);
int getDec(char *bin);
void isZero(int reg);
void advance_pc(int step, char *instruction);
void output();

#endif /* SIMULATOR_H_ */
