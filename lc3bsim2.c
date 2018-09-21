/*
		REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

		Name 1: Raiyan Chowdhury
		Name 2: Timberlon Gray
		UTEID 1: rac4444
		UTEID 2: tg22698
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
	 MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

	int PC,		/* program counter */
		N,		/* n condition bit */
		Z,		/* z condition bit */
		P;		/* p condition bit */
	int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
	printf("----------------LC-3b ISIM Help-----------------------\n");
	printf("go               -  run program to completion         \n");
	printf("run n            -  execute program for n instructions\n");
	printf("mdump low high   -  dump memory from low to high      \n");
	printf("rdump            -  dump the register & bus values    \n");
	printf("?                -  display this help menu            \n");
	printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

	process_instruction();
	CURRENT_LATCHES = NEXT_LATCHES;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
	int i;

	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating for %d cycles...\n\n", num_cycles);
	for (i = 0; i < num_cycles; i++) {
		if (CURRENT_LATCHES.PC == 0x0000) {
			RUN_BIT = FALSE;
			printf("Simulator halted\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
	if (RUN_BIT == FALSE) {
		printf("Can't simulate, Simulator is halted\n\n");
		return;
	}

	printf("Simulating...\n\n");
	while (CURRENT_LATCHES.PC != 0x0000)
		cycle();
	RUN_BIT = FALSE;
	printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
	int address; /* this is a byte address */

	printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	printf("-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	printf("\n");

	/* dump the memory contents into the dumpsim file */
	fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
	fprintf(dumpsim_file, "-------------------------------------\n");
	for (address = (start >> 1); address <= (stop >> 1); address++)
		fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
	int k; 

	printf("\nCurrent register/bus values :\n");
	printf("-------------------------------------\n");
	printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
	printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	printf("Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	printf("\n");

	/* dump the state information into the dumpsim file */
	fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
	fprintf(dumpsim_file, "-------------------------------------\n");
	fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
	fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
	fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
	fprintf(dumpsim_file, "Registers:\n");
	for (k = 0; k < LC_3b_REGS; k++)
		fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
	fprintf(dumpsim_file, "\n");
	fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
	char buffer[20];
	int start, stop, cycles;

	printf("LC-3b-SIM> ");

	scanf("%s", buffer);
	printf("\n");

	switch(buffer[0]) {
	case 'G':
	case 'g':
		go();
		break;

	case 'M':
	case 'm':
		scanf("%i %i", &start, &stop);
		mdump(dumpsim_file, start, stop);
		break;

	case '?':
		help();
		break;
	case 'Q':
	case 'q':
		printf("Bye.\n");
		exit(0);

	case 'R':
	case 'r':
		if (buffer[1] == 'd' || buffer[1] == 'D')
			rdump(dumpsim_file);
		else {
			scanf("%d", &cycles);
			run(cycles);
		}
		break;

	default:
		printf("Invalid Command\n");
		break;
	}
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
	int i;

	for (i=0; i < WORDS_IN_MEM; i++) {
		MEMORY[i][0] = 0;
		MEMORY[i][1] = 0;
	}
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
	FILE * prog;
	int ii, word, program_base;

	/* Open program file. */
	prog = fopen(program_filename, "r");
	if (prog == NULL) {
		printf("Error: Can't open program file %s\n", program_filename);
		exit(-1);
	}

	/* Read in the program. */
	if (fscanf(prog, "%x\n", &word) != EOF)
		program_base = word >> 1;
	else {
		printf("Error: Program file is empty\n");
		exit(-1);
	}

	ii = 0;
	while (fscanf(prog, "%x\n", &word) != EOF) {
		/* Make sure it fits. */
		if (program_base + ii >= WORDS_IN_MEM) {
			printf("Error: Program file %s is too long to fit in memory. %x\n",
						 program_filename, ii);
			exit(-1);
		}

		/* Write the word to memory array. */
		MEMORY[program_base + ii][0] = word & 0x00FF;
		MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
		ii++;
	}

	if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

	printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
	int i;

	init_memory();
	for ( i = 0; i < num_prog_files; i++ ) {
		load_program(program_filename);
		while(*program_filename++ != '\0');
	}
	CURRENT_LATCHES.Z = 1;  
	NEXT_LATCHES = CURRENT_LATCHES;
		
	RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
	FILE * dumpsim_file;

	/* Error Checking */
	if (argc < 2) {
		printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
					 argv[0]);
		exit(1);
	}

	printf("LC-3b Simulator\n\n");

	initialize(argv[1], argc - 1);

	if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
		printf("Error: Can't open dumpsim file\n");
		exit(-1);
	}

	while (1)
		get_command(dumpsim_file);
		
}

/***************************************************************/
/* Do not modify the above code.
	 You are allowed to use the following global variables in your
	 code. These are defined above.

	 MEMORY

	 CURRENT_LATCHES
	 NEXT_LATCHES

	 You may define your own local/global variables and functions.
	 You may use the functions to get at the control bits defined
	 above.

	 Begin your code here 	  			       */

/***************************************************************/

int sext5(int num) {
	if (num & 0x0010) num |= 0xFFE0;
	return num;
}

int sext6(int num) {
	if (num & 0x0020) num |= 0xFFC0;
	return num;
}

int sext8(int num) {
	if (num & 0x0080) num |= 0xFF00;
	return num;
}

int sext11(int num) {
	if (num & 0x0400) num |= 0xF800;
	return num;
}

int sext(int num, int width) {
	if (num & (1 << (width - 1))) {
		num |= ((0xFFFF << width) & 0xFFFF);
	}
	return num;
}

void setcc(int num) {
	if (Low16bits(num) & 0x8000) {
		NEXT_LATCHES.N = 1;
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 0;
	} else if (Low16bits(num) == 0) {
		NEXT_LATCHES.N = 0;
		NEXT_LATCHES.Z = 1;
		NEXT_LATCHES.P = 0;
	} else {
		NEXT_LATCHES.N = 0;
		NEXT_LATCHES.Z = 0;
		NEXT_LATCHES.P = 1;
	}
}

void process_instruction(){
	/*  function: process_instruction
	 *  
	 *    Process one instruction at a time  
	 *       -Fetch one instruction
	 *       -Decode 
	 *       -Execute
	 *       -Update NEXT_LATCHES
	 */     
	/*MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
	typedef struct System_Latches_Struct{

	int PC,		
		N,		
		Z,	
		P;	
	int REGS[LC_3b_REGS]; 
} System_Latches;


System_Latches CURRENT_LATCHES, NEXT_LATCHES;
	*/
	int pcIdx = CURRENT_LATCHES.PC / 2;
	int inst = MEMORY[pcIdx][0] + (MEMORY[pcIdx][1] << 8); 
	inst = Low16bits(inst);
	NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
	int opcode = inst >> 12;

	int idx;
	for (idx = 0; idx < 8; idx++) {
		NEXT_LATCHES.REGS[idx] = CURRENT_LATCHES.REGS[idx];
	}
	NEXT_LATCHES.N = CURRENT_LATCHES.N; NEXT_LATCHES.Z = CURRENT_LATCHES.Z; NEXT_LATCHES.P = CURRENT_LATCHES.P;

	if (opcode == 1) { /* ADD */
		int dr = (inst >> 9) & 0x7;
		int op1 = CURRENT_LATCHES.REGS[(inst >> 6) & 0x7];
		int op2;
		if (inst & 0x0020) { /* immediate */
			op2 = sext(inst & 0x001F, 5);
		} else {			 /* register */
			op2 = CURRENT_LATCHES.REGS[inst & 0x7];
		}
		NEXT_LATCHES.REGS[dr] =  Low16bits(op1 + op2);
		setcc(NEXT_LATCHES.REGS[dr]);
	}
	else if (opcode == 5) { /* AND */
		int DR = (inst >> 9) & 0x7;
		int SR1 = (inst >> 6) & 0x7;
		if (inst & 0x20) {
			int imm5 = inst & 0x1F;
			NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1]) & Low16bits(sext(imm5, 5)); 
		} else {
			int SR2 = inst & 0x7;
			NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1]) & Low16bits(CURRENT_LATCHES.REGS[SR2]);
		}
		setcc(NEXT_LATCHES.REGS[DR]);
	}
	else if (opcode == 0) { /* BR */
		int n = (inst >> 11) & 0x1;
		int z = (inst >> 10) & 0x1;
		int p = (inst >> 9) & 0x1;
		if ((CURRENT_LATCHES.N && n) || (CURRENT_LATCHES.Z && z) || (CURRENT_LATCHES.P && p)) {
			int offset9 = inst & 0x1FF;
			offset9 = Low16bits((sext(offset9, 9) << 1));
			NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + offset9);
		}
	}
	else if (opcode == 12) { /* JMP or RET */
		int reg = (inst >> 6) & 0x7;
		/* do we need to check if address is odd? <error> */
		NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[reg];
	}
	else if (opcode == 4) { /* JSR or JSRR */
		NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
		if ((inst >> 11) & 0x1) { /* JSR */
			NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (sext(inst & 0x07FF, 11) << 1));
		} else { /* JSRR */
			int reg = (inst >> 6) & 0x7;
			NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[reg];
		}
	}
	else if (opcode == 2) { /* LDB */
		int sr = (inst >> 6) & 0x7;
		int dr = (inst >> 9) & 0x7;
		int addr = Low16bits(CURRENT_LATCHES.REGS[sr] + sext(inst & 0x003F, 6));
		NEXT_LATCHES.REGS[dr] = Low16bits(sext(MEMORY[addr/2][addr%2], 8));
		setcc(NEXT_LATCHES.REGS[dr]);
	} 
	else if (opcode == 6) { /* LDW */
		int baseR = (inst >> 6) & 0x7;
		int DR = (inst >> 9) & 0x7;
		int addr = Low16bits(CURRENT_LATCHES.REGS[baseR] + (sext(inst & 0x003F, 6) << 1));
		NEXT_LATCHES.REGS[DR] = Low16bits(MEMORY[addr/2][1] << 8) + Low16bits(MEMORY[addr/2][0]);
		setcc(NEXT_LATCHES.REGS[DR]);
	}
	else if (opcode == 14) { /* LEA */
		int DR = (inst >> 9) & 0x7;
		int offset9 = inst & 0x1FF;
		NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.PC + (sext(offset9, 9) << 1));
	}
	else if (opcode == 9) { /* XOR */
		int dr = (inst >> 9) & 0x7;
		int sr1 = (inst >> 6) & 0x7;
		if (inst & 0x20) {
			int imm5 = inst & 0x1F;
			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1]) ^ Low16bits(sext(imm5, 5));
		} else {
			int sr2 = inst & 0x7;
			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1]) ^ Low16bits(CURRENT_LATCHES.REGS[sr2]);
		}
		setcc(NEXT_LATCHES.REGS[dr]);
	}
	else if (opcode == 13) { /* shifts */
		int shiftAmt = inst & 0x000F;
		int dr = (inst >> 9) & 0x7;
		int sr = (inst >> 6) & 0x7;
		int temp = (inst >> 4) & 0x3;
		if (temp == 0) { /* LSHF */
			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr] << shiftAmt);
		} else if (temp == 1) { /* RSHFL */
			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr] >> shiftAmt);
		} else { /* RSHFA  -- temp == 3 */
			int out = sext(CURRENT_LATCHES.REGS[sr] >> shiftAmt, 16 - shiftAmt);
			NEXT_LATCHES.REGS[dr] = Low16bits(out);
		}
		setcc(NEXT_LATCHES.REGS[dr]);
	}
	else if (opcode == 3) { /* STB */
		int br = (inst >> 6) & 0x7;
		int sr = (inst >> 9) & 0x7;
		int addr = Low16bits(CURRENT_LATCHES.REGS[br] + sext(inst & 0x003F, 6));
		MEMORY[addr/2][addr%2] = CURRENT_LATCHES.REGS[sr] & 0x00FF;
	} 
	else if (opcode == 7) { /* STW */
		int baseR = (inst >> 6) & 0x7;
		int SR = (inst >> 9) & 0x7;
		int addr = Low16bits(CURRENT_LATCHES.REGS[baseR] + (sext(inst & 0x003F, 6) << 1));
		MEMORY[addr/2][0] = CURRENT_LATCHES.REGS[SR] & 0x00FF;
		MEMORY[addr/2][1] = (CURRENT_LATCHES.REGS[SR] >> 8) & 0x00FF;
	}
	else if (opcode == 15) { /* TRAP */
		NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
		int trapvect8 = (inst & 0xFF) << 1;
		NEXT_LATCHES.PC = Low16bits((sext(MEMORY[trapvect8/2][1], 8) << 8) + sext(MEMORY[trapvect8/2][0], 8));
	}
}

