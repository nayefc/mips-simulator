/*
 *
 * mipsInstructionMap.h
 * 
 * Defines instructions as a hex value for comparison
 *
 * author: Nayef Copty
 * date: 11/11/11
 *
 */

// R-Type Instructions
#define RTYPEOP 0x0
#define ADD	0x20
#define SUB	0x22
#define AND	0x24
#define OR	0x25
#define SLL	0x0
#define SLT	0x2A
#define SRL	0x2
#define JR	0x8

// I-Type Instructions
#define LW	0x23
#define SW	0x2B
#define ANDI	0xC
#define ORI	0xD
#define LUI	0xF
#define BEQ	0x4
#define BNE	0x5
#define SLTI	0xA
#define ADDI	0x8

// J-Type Instructions
#define J	0x2
#define JAL	0x3
