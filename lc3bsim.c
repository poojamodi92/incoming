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
*CCCCCCCCCCDDDDDDDDDD
*/

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

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];
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

  printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
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
  printf("PC                : 0x%0.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%0.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
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

int instruction_register;

int shift_right_by( int shift_amnt){
	int bit = instruction_register >> shift_amnt;
	return bit;
}

int get_bit(int shifted_instruction, int range){
	int a = shifted_instruction & ((1<<range) - 1);
	return a;
}

void set_condition_code(int result){
	NEXT_LATCHES.N = 0;
	NEXT_LATCHES.Z = 0;
	NEXT_LATCHES.P = 0;
	if (result == 0){
		NEXT_LATCHES.Z = 1;
	}
	else if (get_bit(shift_right_by(15),1) == 1) {
		NEXT_LATCHES.N = 1;
	}
	else{
		NEXT_LATCHES.P = 1;
	}
}

int sext(int victim, int n){
	int bit_n = get_bit(shift_right_by(n-1),1);
	if(bit_n == 1){	
		int extend = (~((1<<n)-1)) & 0x0000FFFF;
		victim  = victim | extend;
	}
	return victim;
}

void flush_16_bits(){
	int i;
	for(i = 0;i<8;i++){
		NEXT_LATCHES.REGS[i] &= 0xFFFF;
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
	int four_bits;
/* FETCH THE INSTRUCTION : */
	instruction_register = MEMORY[CURRENT_LATCHES.PC>>1][0]|(MEMORY[CURRENT_LATCHES.PC>>1][1] << 8);
/*/INCREMENT PC : */
	CURRENT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC);
	NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 ;
	NEXT_LATCHES.PC = NEXT_LATCHES.PC & 0xFFFF;
	
/* CHECK FIRST FOUR BITS :*/
	int first_four = shift_right_by(12) ;
	int dr = get_bit(shift_right_by(9), 3);
	int sr1 = get_bit(shift_right_by(6),3);
	int sr2 = get_bit(instruction_register, 3);
	int imm_5 = sext(get_bit(instruction_register, 5),5);
	int multiplexer_5 = get_bit(shift_right_by(5),1);
	int baseR = sr1;
	int offset_6 = sext(get_bit(instruction_register, 6),6);
	int add, and, xor,  n, z, p, address, PCoffset_11,amount_4, number, PCoffset_9,bit_15, temp4, shifters;
	switch (first_four) {
		/*/ ADD : */
		case 1:
			add = CURRENT_LATCHES.REGS[sr2];
			if(multiplexer_5 == 1) {
				add = imm_5;
			}
			NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] + add;
			set_condition_code(NEXT_LATCHES.REGS[dr]);			
			break;
			
		/*/AND		*/
		case 5:
			and = CURRENT_LATCHES.REGS[sr2];
			if(multiplexer_5 == 1) {
				and = imm_5;
			}
			NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] & and;
			set_condition_code(NEXT_LATCHES.REGS[dr]);			
			break;
		
		/*/BR */
		case 0:
			n = get_bit(shift_right_by(11),1);
			z = get_bit(shift_right_by(10),1);
			p = get_bit(shift_right_by(9),1);
			PCoffset_9 = sext(get_bit(instruction_register, 9),9);
			if((n && CURRENT_LATCHES.N)||(z & CURRENT_LATCHES.Z)||(p & CURRENT_LATCHES.P)){
				NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + (PCoffset_9 << 1); 
			}			
			break;
		
		/*JMP/RET*/
		case 12:
			NEXT_LATCHES.PC =  CURRENT_LATCHES.REGS[baseR];
			break;
		
		/*JSR /JSRR*/
		case 4:
			PCoffset_11 = sext(get_bit(instruction_register, 11),11);
			NEXT_LATCHES.REGS[7] = CURRENT_LATCHES.PC + 2;
			if(get_bit(shift_right_by(11),1) ==0){
				NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
			}
			else{
				NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + (PCoffset_11 << 1); 
			} 
			break;
		
		/*LDB*/
		case 2:
			address = CURRENT_LATCHES.REGS[baseR] + offset_6;
			number = sext(MEMORY[address>>1][address & 1],8);
			NEXT_LATCHES.REGS[dr] = number;
			set_condition_code(NEXT_LATCHES.REGS[dr]);			
			break;
		
		/*LDW*/
		case 6:
			address = CURRENT_LATCHES.REGS[baseR] + (offset_6<<1);
			number =  MEMORY[address>>1][0]|(MEMORY[address>>1][1] << 8);
			NEXT_LATCHES.REGS[dr] = number;
			set_condition_code(NEXT_LATCHES.REGS[dr]);			
			break;
		
		/*LEA*/
		case 14:
			PCoffset_9 = sext(get_bit(instruction_register, 9),9);
			address = CURRENT_LATCHES.PC + 2 + (PCoffset_9 <<1);
			NEXT_LATCHES.REGS[dr] = address;
			break;
			
		/*XOR/NOT*/
		case 9:
			xor  = CURRENT_LATCHES.REGS[sr2];
			if(multiplexer_5 == 1) {
				xor = imm_5;
			}
			NEXT_LATCHES.REGS[dr] = CURRENT_LATCHES.REGS[sr1] ^ xor;
			set_condition_code(NEXT_LATCHES.REGS[dr]);			
			break;

		/*STB*/
		case 3:
			address = CURRENT_LATCHES.REGS[baseR] + offset_6;
			MEMORY[address>>1][address & 1] = get_bit(CURRENT_LATCHES.REGS[dr],8);
			break;
			
		/*STW*/
		case 7:
			address = CURRENT_LATCHES.REGS[baseR] + (offset_6<<1);
			MEMORY[address>>1][0] = get_bit(CURRENT_LATCHES.REGS[dr],8);
			MEMORY[address>>1][1] = get_bit(CURRENT_LATCHES.REGS[dr]>>8, 8) ;
			break;
				
		/* shift*/
		case 13:	
			amount_4 = get_bit(instruction_register,4);
			if(get_bit(shift_right_by(4),1) == 0){
				NEXT_LATCHES.REGS[dr]= (CURRENT_LATCHES.REGS[sr1]<< amount_4);;
			}
			else{
				if(multiplexer_5 == 0){
					NEXT_LATCHES.REGS[dr]= (CURRENT_LATCHES.REGS[sr1]>> amount_4 );
				} 
				else {
					shifters = CURRENT_LATCHES.REGS[sr1] >> amount_4;
					bit_15 = get_bit(shift_right_by(15),1);
					if(bit_15 == 1) {
							temp4 = (1 << (16 - amount_4)) - 1;
							temp4 = ~(temp4); 
							shifters = shifters | temp4;
					}
										
					NEXT_LATCHES.REGS[dr] = shifters;
				}
			}
			
			set_condition_code(NEXT_LATCHES.REGS[dr]);			
			break;

		/* trap */
		case 15:
			NEXT_LATCHES.PC = 0X0000;
			break;
	}
	flush_16_bits();
}


