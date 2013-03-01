/*
 *
 * simulator.c
 *
 * Main simulator class
 *
 * author: nayefc
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "simulator.h"
#include "mipsInstructionMap.h"

int main(int argc, char *argv[]) {

	if (argc != 2) {
			printf("Incorrect number of arguments\n");
			exit(1);
	}

	else {

		// Open Input file
		FILE *fp;
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			printf("Error opening input file.\n");
			exit(1);
		}

		// Initialize registers values' to 0x0
		for (int i = 0; i < 32; i++)
			registers[i] = 0x0;

		// Malloc memory
		memory  = (int *)malloc(3072 * sizeof(int));
		if (memory == NULL) {
			printf("Not enough memory. Aborting..\n");
			exit(1);
		}

		// Initialize 'memory' array to -1
		for (int i = 0; i < 3072; i++) {
			memory[i] = -1;
		} 

		// Initialize variables for parsing
		char line[MAX_LENGTH+2];
		char *p;
		int i = 0, line_num = 0;
		//int line_num = 0, i = 0, data_line = 0;

		// Copy .text section to memory
		while (fgets(line, MAX_LENGTH+2, fp) != NULL) {
			line_num++;

			// Remove '\n' from 'line'
			p = strchr(line, '\n');
			if (p != NULL)
				*p = '\0';

			memory[i] = getDec(line);

			// If 'nop' found, move to 0x2000 in memory and break
			if (strcmp(line, "00000000000000000000000000000000") == 0) {
				i = 0x800;
				break;
			}

			else
				i++;
		}

		// Seek fp to first instruction in .data
		char data[MAX_LENGTH+2];
		int bytes = 33 * line_num;
		fseek(fp, bytes, SEEK_SET);

		// Copy .data section to memory
		while (fgets(line, MAX_LENGTH+2, fp) != NULL) {

			// Remove '\n' from 'line'
			p = strchr(line, '\n');
			if (p != NULL)
				*p = '\0';

			memory[i] = getDec(line);
			i++;
		}


		// Parse .text section
		while (memory[pc/4] != 0)
			parse_instruction(memory[pc/4]);
		
		advance_pc(4, "nop");

		// Output registers
		output();

		// Dealloc .data
		free(memory);
		memory = NULL;

		// Close File
		fclose (fp);
		return 0;
	}
}

// Function to take in instruciton to parse
void parse_instruction(unsigned int sum) {

	// Extract the opcode
	int x = sum >> 26;

	// R-Type Instruction - op, rs, rt, rd, sa, func
	if (x == RTYPEOP) {
		
		// Bitshift to extract the 'function' part of the instruction
		unsigned int y = sum << 26;
		y = y >> 26;

		// jr rs
		if (y == JR) {
			int reg = (sum << 6);
			reg = (sum >> 6);
			pc = reg;
		}

		else {
			// Get rs, rt, rd

			int rs = (sum >> 21) & 0x1F;
			int rt = (sum >> 16) & 0x1F;
			int rd = (sum >> 11) & 0x1F;
			int sa = (sum >> 6) & 0x1F;		

			isZero(rd);

			// add rd, rs, rt
			if (y == ADD) {
				registers[rd] = registers[rs] + registers[rt];
				advance_pc(4, "add");
			}

			// sub rd, rs, rt
			else if (y == SUB) {
				registers[rd] = registers[rs] - registers[rt];
				advance_pc(4, "sub");
			}

			// and rd, rs, rt
			else if (y == AND) {
				registers[rd] = registers[rs] & registers[rt];
				advance_pc(4, "and");
			}

			// or rd, rs, rt
			else if (y == OR) {
				registers[rd] = registers[rs] | registers[rt];		
				advance_pc(4, "or");
			}

			// slt rd, rs, rt
			else if (y == SLT) {
				if (registers[rs] < registers[rt])
					registers[rd] = 0x1;
				else
					registers[rd] = 0x0;
				advance_pc(4, "slt");
			}

			// sll rd, rt, sa
			else if (y == SLL) {
				registers[rd] = registers[rt] << sa;
				advance_pc(4, "sll");
			}

			// srl rd, rt, sa
			else if (y == SRL) {
				registers[rd] = registers[rt] >> sa; 
				advance_pc(4, "srl");
			}
		}
	}

	// If instruction is I-Type or J-Type
	// I-Type: op, rs, rt, imm
	else {

		// Get rs, rt, immediate
		int rs = (sum >> 21) & 0x1F;
		int rt = (sum >> 16) & 0x1F;
		unsigned short imm = sum & 0xFFFF;

		// lw rt, immediate(rs)
		if (x == LW) {
			isZero(rt); 
			int addr = registers[rs] + imm;
			registers[rt] = memory[addr/4];
			advance_pc(4, "lw");
		}

		// sw rt, immediate(rs)
		else if (x == SW) {
			int addr = registers[rs] + imm;
			memory[addr/4] = registers[rt];
			advance_pc(4, "sw");
		}

		// andi rt, rs, immediate
		else if (x == ANDI) {
			isZero(rt);
			int addr = rs + imm;
			registers[rt] = registers[rs] & imm;
			advance_pc(4, "andi");
		}

		// ori rt, rs, immediate
		else if (x == ORI) {
			isZero(rt);
			registers[rt] = registers[rs] | imm;
			advance_pc(4, "ori");
		}

		// slti rt, rs, immediate
		else if (x == SLTI) {
			isZero(rt);
			if (registers[rs] < imm)
				registers[rt] = 0x1;
			else
				registers[rt] = 0x0;
			advance_pc(4, "slti");
		}

		// addi rt, rs, immediate
		else if (x == ADDI) {
			isZero(rt);
			registers[rt] = registers[rs] + imm;
			advance_pc(4, "addi");
		}


		// beq rs, rt, label
		else if (x == BEQ) {
			if (registers[rs] == registers[rt]) {
				short shortImm = (short)imm;
				advance_pc((shortImm << 2) + 4, "beq");
			}
			else
				advance_pc(4, "beq");
		}

		else if (x == BNE) {
			if (registers[rs] != registers[rt]) {
				short shortImm = (short)imm;
				advance_pc((shortImm << 2) + 4, "bne");
			}
			
			else
				advance_pc(4, "bne");
		}

		// lui rt, immediate
		else if (x == LUI) {
			registers[rt] = imm << 16;
			advance_pc(4, "lui");
		}

		// J-Type instructions
		else {

			rs = sum & 0x3FFFFFF;

			// j immediate
			if (x == J)
				pc = (pc & 0xF0000000) | (rs << 2);
			
			// jal immediate
			else if (x== JAL) {
				pc = (pc & 0xF0000000) | (rs << 2);
				registers[31] = pc + 4;
			}
		}
	}
}

// Convert a binary string to a decimal value
int getDec(char *bin) {

	int  b, k, m, n;
	int  len, sum = 0;

	// Length - 1 to accomodate for null terminator
	len = strlen(bin) - 1;

	// Iterate the string
	for(k = 0; k <= len; k++) {

		// Convert char to numeric value
		n = (bin[k] - '0');

		// Check the character is binary
		if ((n > 1) || (n < 0))  {
			return 0;
		}

		for(b = 1, m = len; m > k; m--)
			b *= 2;

		// sum it up
		sum = sum + n * b;
	}

	return sum;
}

// Check register is not zero
void isZero(int reg) {
	/*if (reg == 0x0) {
		printf("Error. Cannot overwrite $zero register\n");
		exit(1);
	}*/
}

// Advance PC
void advance_pc(int step, char *instruction) {
	pc += step;
}

// Output reigsters
void output() {
	printf("1	$zero		0x%08x\n", registers[0]);
	printf("2	$at		0x%08x\n", registers[1]);
	printf("3	$v0		0x%08x\n", registers[2]);
	printf("4	$v1		0x%08x\n", registers[3]);
	printf("5	$a0		0x%08x\n", registers[4]);
	printf("6	$a1		0x%08x\n", registers[5]);
	printf("7	$a2		0x%08x\n", registers[6]);
	printf("8	$a3		0x%08x\n", registers[7]);
	printf("9	$t0		0x%08x\n", registers[8]);
	printf("10	$t1		0x%08x\n", registers[9]);
	printf("11	$t2		0x%08x\n", registers[10]);
	printf("12	$t3		0x%08x\n", registers[11]);
	printf("13	$t4		0x%08x\n", registers[12]);
	printf("14	$t5		0x%08x\n", registers[13]);
	printf("15	$t6		0x%08x\n", registers[14]);
	printf("16	$t7		0x%08x\n", registers[15]);
	printf("17	$s0		0x%08x\n", registers[16]);
	printf("18	$s1		0x%08x\n", registers[17]);
	printf("19	$s2		0x%08x\n", registers[18]);
	printf("20	$s3		0x%08x\n", registers[19]);
	printf("21	$s4		0x%08x\n", registers[20]);
	printf("22	$s5		0x%08x\n", registers[21]);
	printf("23	$s6		0x%08x\n", registers[22]);
	printf("24	$s7		0x%08x\n", registers[23]);
	printf("25	$t8		0x%08x\n", registers[24]);
	printf("26	$t9		0x%08x\n", registers[25]);
	printf("27	$k0		0x%08x\n", registers[26]);
	printf("28	$k1		0x%08x\n", registers[27]);
	printf("29	pc		0x%08x\n", pc);
}
