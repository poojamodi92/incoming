#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>

#define MAX_LABEL_LENGTH 20
#define MAX_SYMBOL_COUNT 255
#define MAX_LINE_LENGTH 255
#define VALID_INSTR_CNT 31
#define PSEUDO_OP_COUNT 3

const int op_codes[VALID_INSTR_CNT] = {  
    1,
    5, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    0, 
    15, 
    12, 
    4, 
    4, 
    2, 
    6, 
    14, 
    0, 
    9, 
    12, 
    13, 
    13, 
    13, 
    8, 
    3, 
    7, 
    15, 
    9,
	50,
	51,
	52
};

const char valid_op_codes[VALID_INSTR_CNT][8] = {  
    "add", /* 0 */
    "and", /* 1 */
    "brn", /* 2 */ 
    "brz", /* 3  */
    "brp", /* 4 */
    "brnz", /*5  */
    "brnp", /* 6  */
    "brzp", /* 7  */
    "br", /* 8  */
    "brnzp", /* 9  */
    "halt", /* 10  */
    "jmp", /* 11 */
    "jsr", /* 12  */
    "jsrr", /* 13 */
    "ldb", /* 14 */
    "ldw", /* 15  */
    "lea", /* 16 */
    "nop", /* 17 */
    "not",  /*18*/
    "ret", /* 19 */
    "lshf", /* 20 */
    "rshfl", /*21*/
    "rshfa", /* 22  */
    "rti", /* 23  */
    "stb",  /* 24 */
    "stw", /* 25  */
    "trap", /* 26  */
    "xor",/* 27  */
	".orig", /* 28 */
	".end", /* 29  */
	".fill" /* 30 */
};

const char pseudo_op_codes[PSEUDO_OP_COUNT][8] = {
    ".orig", 
    ".end", 
    ".fill"
};

enum {
	DONE,
	OK,
	EMPTY_LINE 
};


typedef struct{
	uint16_t address;
	char label[MAX_LABEL_LENGTH + 1];
} TableEntry;

TableEntry symbolTable[MAX_SYMBOL_COUNT];
int next_symbol_index = 0;

/*
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
*/

FILE* iFilePtr  = NULL;
FILE* oFilePtr  = NULL;
uint32_t curr_symbol_count = 0;

/*
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
*/

uint32_t make_pass_one (char*);
uint32_t make_pass_two (char*, char*);
uint32_t process_instruction (char *, char *, char *, 
			      char *, char *, uint16_t);
uint32_t readAndParse (FILE *, char *, char **, char **, 
		       char **, char **, char **, char **);
int32_t is_opcode(char *);
void die_with_error(char*, int);
int toNum( char * pStr );
int	str_to_reg_num(char *);
int get_symbol_loc(char*);

int type_1 (int opcode, int dr, int sr1, int sr2); 
int type_2(int opcode, int dr, int sr1, int imm5) ;
int type_3(int opcode, int baseR1) ;
int type_4 (int opcode, int dr, int baseR1, int offset) ;
int type_5 (int opcode, int dr, int sr1, int bit1, int bit2, int offset) ;
int type_6 (int opcode, int, int offset) ;
int type_7 (int opcode, int dr, int sr1, int bit, int imm) ;
int type_10 (int opcode, int trapvector) ;
int type_11 (int opcode, int n, int z, int p, int offset) ;
int type_12 (int opcode, int trapvector) ;
int contains_char(char* str, char c, int);
/*
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
*/

int main (int argc, char ** argv) {
	int retval=0;
	/* argc must be 3 for correct execution */ 
	if ( argc != 3 ) { 
		fprintf (stderr, "Usage: %s <source.asm> <output.obj>\n", argv[0]);
		retval = 4;
		exit (retval);
	}
	
	iFilePtr = fopen(argv[1], "r");
	if (!iFilePtr) {
		fprintf(stderr, "Error: Cannot open input file %s\n", argv[1]);
		retval = 4;
		exit (retval);
	}

	oFilePtr = fopen(argv[2], "w");
	if (!oFilePtr) {
		fprintf(stderr, "Error: Cannot open output file %s\n", argv[2]);
		retval = 4;
		exit (retval);
	}
  
  	fclose (oFilePtr);
  	fclose (iFilePtr); 
	retval = make_pass_one (argv[1]);
  
	retval = make_pass_two (argv[1], argv[2]);
	
	exit(0);	
  	return (retval);
}


/*
//////////////////////////////////////////////////////////////////////////
//  Create Symbol Table in Pass 1
//////////////////////////////////////////////////////////////////////////
*/

uint32_t make_pass_one (char* file_name) {
	uint32_t retval;
	char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
	        *lArg2, *lArg3, *lArg4;
	int  lRet;
	FILE * lInfile;
	int address = 0, op_index;
	retval = 0;
	lInfile = fopen( file_name, "r" );	/* open the input file */
	do {
		lRet = readAndParse( lInfile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet == DONE) {
			break;
		} 
		else if(lRet == EMPTY_LINE){
			continue;
		}
		/*printf("Line: %s\n", lLine); 
		printf("Label: %s\n", lLabel);
		printf("Opcode: %s\n", lOpcode); 
		printf("Arg1: %s\n", lArg1); 
		printf("Arg2: %s\n", lArg2); 
		printf("Arg3: %s\n", lArg3); 
		printf("Arg4: %s\n", lArg4); */
		op_index = is_opcode(lOpcode);
		if(op_index == -1) {
		/*	printf("Opcode: %s\n", lOpcode);*/
			printf("Invalid Opcode, line 216\n and the instruction is%s\n", lLine);
			exit(2);
		}
		if(op_index  == 28) {
			if(strcmp(lArg1, "")==0) {
				printf("Missing Operand at .orig, line 221\n and the instruction is%s\n", lLine);
				exit(4);
			}
			if (toNum(lArg1) % 2 != 0) {
				printf("Odd constant for .orig, line 225\n and the instruction is%s\n", lLine);
				exit(3);
			}
			continue;
		}
		else if(op_index == 29) {
			symbolTable[next_symbol_index].address = address;
			strcpy(symbolTable[next_symbol_index].label, lLabel);
		/*	printf("%d,%s\n", symbolTable[next_symbol_index].address, &symbolTable[next_symbol_index].label); */
			next_symbol_index++;
			break;
		}
	/*	else if (isalnum(lLabel)==0 || lLabel[0]=='x'||lLabel[0]=='1'|| lLabel[0]=='2'|| lLabel[0]=='3'|| lLabel[0]=='4'|| lLabel[0]=='5'|| lLabel[0]=='6'|| lLabel[0]=='7'|| lLabel[0]=='8'|| lLabel[0]=='9')
        {
            printf("Invalid Label\n");
            exit(1);
        }*/
       	else if (strcmp(lArg4, "") != 0)
        {
            printf("Extra Operand, line 240\n and the instruction is%s\n", lLine);
            exit(4);
        }
		else if (strcmp(lLabel, "") != 0){
		/*	if (isalnum(*lLabel)!=0 || lLabel[0]=='x'||lLabel[0]>='1'|| lLabel[0] <= '9'){
            	printf("Invalid Label, line 246\n");
            	exit(1);
        	}*/

			symbolTable[next_symbol_index].address = address;
			strcpy(symbolTable[next_symbol_index].label, lLabel);
		/*	printf("%d,%s\n", symbolTable[next_symbol_index].address, &symbolTable[next_symbol_index].label); */
			next_symbol_index++;
		}
		address += 2;
	} while( lRet != DONE );
	/*	die_with_error("No instructions to process, line 257\n", 4);*/
	

/*	for(i = 0; i < next_symbol_index; i++) {
		printf("%d,%s\n", symbolTable[i].address, &symbolTable[i].label);
	}*/
	fclose(lInfile);
	return retval;
}

/*
//////////////////////////////////////////////////////////////////////////
//  Generate Machine Instructions in Pass2
//////////////////////////////////////////////////////////////////////////
*/

uint32_t make_pass_two (char* infile, char* outfile) {
	uint32_t retval;
	int instruction;
	int op_index, dr, sr1, sr2, imm5, n,z,p, pc_offset_9_1, pc_offset_9_2,  base_r_1, base_r_2, offset_6, amount4, trapvect_8;
	int NZP_len = 0;
	int startpoint = 0;
	int the_bit;
	char lLine[MAX_LINE_LENGTH + 1], *label, *opcode, *arg1,
	        *arg2, *arg3, *arg4;
	int lRet, pc, fill_arg, op_code;
	FILE * lInfile, *oFilePtr;
	pc = 0;
	retval = 0;
	oFilePtr = fopen(outfile, "w");
	if(!oFilePtr) {
		die_with_error("FILE IS BAD!!!\n", 3);
	}
	lInfile = fopen( infile, "r" );	/* open the input file */
	do {
		lRet = readAndParse( lInfile, lLine, &label,
			&opcode, &arg1, &arg2, &arg3, &arg4 );
		if( lRet == DONE) {
			break;
		} 
		else if(lRet == EMPTY_LINE){
			continue;
		}
	/*	printf("Line: %s\n", lLine); 
		printf("Label: %s\n", label);
		printf("Opcode: %s\n", opcode); 
		printf("Arg1: %s\n", arg1); 
		printf("Arg2: %s\n", arg2); 
		printf("Arg3: %s\n", arg3); 
		printf("Arg4: %s\n", arg4); */
		op_index = is_opcode(opcode);
	/*	printf("Is_opcode returns: %d\n", op_index);*/
		if(op_index  == 28) {
			if(strcmp(arg1, "")==0) {
				printf("Missing Operand at .orig, line 310 \n  and the instruction is%s\n", lLine);
				exit(4);
			}
			if (toNum(arg1) % 2 != 0) {
				printf("Odd constant for .orig, line 314\n and the instruction is%s\n", lLine);
				exit(3);
			}
			startpoint = toNum(arg1);
		fprintf(oFilePtr, "0x%04hx\n", startpoint);
			continue;
		} else if(op_index == 29) {
			break;
		}
		dr = str_to_reg_num(arg1);
		sr1 = str_to_reg_num(arg2);
		sr2 = str_to_reg_num(arg3);
		pc_offset_9_1 = ((get_symbol_loc(arg1) - pc - 2) & 0x3FF) / 2;
		pc_offset_9_2 = ((get_symbol_loc(arg2) - pc - 2) & 0x3FF) / 2;
		base_r_1 = str_to_reg_num(arg1);
		pc += 2;
		base_r_2 = str_to_reg_num(arg2);
		op_code = op_codes[op_index];
		the_bit = (str_to_reg_num(arg3) == -1);
	/*	printf("OP_INDEX: %d\n", op_index);
		printf("Opcode value: %d\n", op_code);*/
		switch (op_index) {
			case 2:
			case 3:
			case 4:
				NZP_len = 3;
				break;
			case 5:
			case 6: 
			case 7:
				NZP_len = 4;
				break;
		/*	case 8:
				NZP_len = 5;
				break;*/
			case 9:
				NZP_len = 5;
				break;
		}
		switch(op_index) {
			case 0:
			case 1:
			case 27:
				/* add*/
			if(dr == -1) {
			
			printf("Unexpected OR missing operand, line 360\n and the instruction is%s\n", lLine);
			exit(4);
			}
			if(sr1 == -1) {
			printf("Unexpected or missing operand, line 364\n and the instruction is%s\n", lLine);
			exit(4);
			}
			if(!the_bit) {
					instruction = type_1(op_code, dr, sr1, sr2);
				} else {
					imm5 = toNum(arg3);
					if(imm5 > 15 || imm5 < -16) {
						printf("Invalid constan, line 372imm5: %d\n", imm5);
						exit(3);
					}
				/*	printf("ADD OPCDOE TYPE2: %d\n", op_code) ;*/
					instruction = type_2(op_code, dr, sr1, imm5 & 0x1f);
				/*	printf("type_2 returns: %.4X\n", instruction);*/
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
		/*	case 8:*/
			case 9:
				if(strcmp(arg1,"")==0) {
				printf("Missing operand, line 390\n");
					exit(4);				
				}
				else if	(get_symbol_loc(arg1)== -1)	{
				printf("Invalid Label, line 394\n");
				exit(1);
				}
				/*printf("OPCODE BEFORE CONTAINS: %s\n", opcode);*/
				n = contains_char(opcode, 'n', NZP_len);
			/*	printf("N: %d\n", n);*/
				z = contains_char(opcode, 'z', NZP_len);
				p = contains_char(opcode, 'p', NZP_len);
			/*	printf("Z: %d\n", z);
				printf("P: %d\n", p);
			printf("PC: %d", pc);*/
				/* all sorts of brs */
				instruction = type_11(op_code, n, z, p, pc_offset_9_1);
				break;
			case 8:
				n = 1;
				z = 1;
				p = 1;
				instruction = type_11(op_code,n, z, p, pc_offset_9_1);
				break;
			case 10:
				instruction = type_10(op_code, 0x25);
				break;
			case 11:
			case 13:
			if(base_r_1 == -1) {
			printf("Missing operand, line 420\n");
			exit(4);
			}
			
				instruction = type_3(op_code, base_r_1); 
				break;
			case 12:
				if(strcmp(arg1,"")==0) {
				printf("Missing operand, line 428\n");
					exit(4);				
				}
				else if	(get_symbol_loc(arg1)== -1)	{
				printf("Invalid Label, line 432\n");
				exit(1);
				}

				instruction = type_11(op_code, 1, 0, 0, pc_offset_9_1);
				break;
			case 14:
			case 15:
			case 24:
			case 25:
			if(base_r_1 == -1) {
			printf("Missing operand, line 443\n");
			exit(4);
			}
				if(dr == -1) {
			
				printf("Unexpected OR missing operand, line 448\n");
				exit(4);
				}

				offset_6 = toNum(arg3);
				if(offset_6 > 31 || offset_6 < -32)	{
					printf("Invalid constant, line 455\n and offset is %d\n", offset_6);
					exit(3);					
				}					

				instruction = type_4(op_code, dr,base_r_2, offset_6 & 0x3F);
				break;
			case 16:
			if((get_symbol_loc(arg2)== -1)) {
			printf("Invalid Label, line 463\n");
			exit(1);} 
				instruction = type_6(op_code, dr, pc_offset_9_2);
				break;
			case 17:
				instruction = 0;
				break;
			case 18:
			if(dr == -1) {
			printf("Unexpected OR missing operand, line 472\n");
			exit(4);
			}
			if(sr1 == -1) {
			printf("Unexpected OR missing operand, line 476\n");
			exit(4);
			}


				instruction = type_7(op_code, dr, sr1,1, 0x1F);
				break; 
			case 19:
				instruction = type_7(op_code, 0, 7, 0, 0);
				break;
			case 20:
				if(dr == -1) {
					printf("Unexpected OR missing operand, line 488\n");
					exit(4);
				}
				if(sr1 == -1) {
					printf("Unexpected OR missing operand, line 492\n");
					exit(4);
				}
				amount4 = toNum(arg3);
				
				if(amount4 > 7 || amount4 < -8)	{
					printf("Invalid constant, line 500\n and offset is %d\n", amount4);
					exit(3);					
				}					
				instruction = type_5(op_code, dr, sr1, 0, 0, amount4);
				break;
			case 21:
				if(dr == -1) {
					printf("Unexpected OR missing operand, line 500\n");
					exit(4);
				}
				if(sr1 == -1) {
					printf("Unexpected OR missing operand, line 504\n");
					exit(4);
				}
				amount4 = toNum(arg3);
								
				if(amount4 > 7 || amount4 < -8)	{
					printf("Invalid constant, line 500\n and offset is %d\n", amount4);
					exit(3);					
				}					
				instruction = type_5(op_code, dr, sr1, 0, 1, amount4);
				break;
			case 22:
			if(dr == -1) {
			printf("Unexpected OR missing operand, line 512\n");
			exit(4);
			}
			if(sr1 == -1) {
			printf("Unexpected OR missing operand, line 516\n");
			exit(4);
			}
			amount4 = toNum(arg3);
			if(amount4 > 7 || amount4 < -8)	{
					printf("Invalid constant, line 500\n and offset is %d\n", amount4);
					exit(3);					
				}					
				instruction = type_5(op_code, dr, sr1, 1, 1, amount4);
				break;
			case 23:
				instruction = type_7(op_code, 0, 0, 0, 0);
				break;
			case 26:	
				if(strcmp(arg1,"")==0){
					printf("Missing operand, line 527\n");
					exit(4);
				}

				trapvect_8 = toNum(arg1);
				
				if(trapvect_8 <0){
					printf("Error3, negative line 535\n");
					exit(3);
				} 
				else if(trapvect_8 != (trapvect_8 & 0xFF)) {
					printf("ERROR3, BEYOND BOUND\n");
					exit(3);
				}
				instruction = type_10(op_code, trapvect_8 );
				break;
			case 30:
				if(strcmp(arg1,"")==0){
					printf("Missing operand, line 536\n");
					exit(4);
				}
				
				fill_arg = toNum(arg1);
				if(fill_arg> 32767 || fill_arg < -32768){
					printf("Invalid constant, line 500\n and offset is %d\n", fill_arg);
					exit(3);			
				}					
				instruction = fill_arg;
				break;
		}
		fprintf(oFilePtr, "0x%04hx\n", instruction);
	} while( lRet != DONE );
	fclose(lInfile);
	fclose(oFilePtr);

	return retval;
}

/*
//////////////////////////////////////////////////////////////////////////
//  Write helper functions here (keep code modular)
//////////////////////////////////////////////////////////////////////////
*/




int toNum( char * pStr ) {
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' ){				/* decimal */ 
     	pStr++;
     	if( *pStr == '-' ){				/* dec is negative */
       		lNeg = 1;
       		pStr++;
     	}
     	t_ptr = pStr;
    	 t_length = strlen(t_ptr);
     	for(k=0;k < t_length;k++) {
       		if (!isdigit(*t_ptr)) {
	 			printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 			exit(4);
      		}
       		t_ptr++;
     	}
     	lNum = atoi(pStr);
     	if (lNeg)
       		lNum = -lNum;
     	return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}

uint32_t readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4 ) {
	   char  * lPtr;
	   unsigned int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' ) 
		lPtr++;

	   *lPtr = '\0';
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
		return( EMPTY_LINE );

	   if( is_opcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
	   {
		*pLabel = lPtr;
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
           *pOpcode = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

           *pArg1 = lPtr;

           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr;
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr;

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr;

	   return( OK );
}

int32_t is_opcode(char * TestOpcode) {
	int32_t return_value = -1;
	int i, rv;
	for (i = 0; i < VALID_INSTR_CNT; i++) {
		rv = strcmp(valid_op_codes[i], TestOpcode);
		/*printf("%s, %s, %d\n", TestOpcode, valid_op_codes[i], rv);*/
		if(rv  == 0) {
			return_value = i;
			break;
		}
	}
	return return_value;
}

void die_with_error(char * message, int exit_code) {
	printf("ERROR: %s", message);
	exit(exit_code);
}

int str_to_reg_num ( char * argument) {
	if((argument[0] != 'r') || (argument[0] == '\0')) {
		return -1;
	}
	return (argument[1]) - '0';
}

int get_symbol_loc(char* label) {
	int i;
	for(i = 0; i < next_symbol_index; i++) {
		if(strcmp(symbolTable[i].label, label) == 0) {
		/*	printf("Symbol table lookup: %d", symbolTable[i].address);*/
			return symbolTable[i].address;
		}
	}
	return -1;
}

void check_register(int regnum);
void check_register(int regnum) {
	if(regnum >= 8 || regnum < 0) {
	/*	printf("REG: %d", regnum);*/
		die_with_error("Register number is out of range!, line 710\n", 4);
	}
}

void check_offset(int offset, int nbits);
void check_offset(int offset, int nbits) {
	if(offset < (-1 * ((1 << (nbits)) - 1)) || offset > (1 << nbits)) {
		die_with_error("Offset is out of range, line 717\n", 4);
	}
}


void check_opcode(int op);
void check_opcode(int op) {
	if(op > 15 && op < 0) {
		die_with_error("Opcode is invalid, line 725\n", 4);
	}
}

int type_1 (int opcode, int dr, int sr1, int sr2) {
	check_opcode(opcode);
	check_register(dr);
/*	printf("sr1\n");*/
	check_register(sr1);
/*	printf("sr2\n");*/
	check_register(sr2);
	return (opcode  << 12) | (dr << 9) | (sr1 << 6) | sr2;
}

int type_2(int opcode, int dr, int sr1, int imm5) {
	check_opcode(opcode);
	check_register(dr);
	check_register(sr1);
	return (opcode << 12) | (dr<<9) | (sr1 <<6) | (1 << 5) | imm5;
}

int type_3(int opcode, int baseR1) {
	check_opcode(opcode);
	check_register(baseR1);
	return (opcode << 12) | (baseR1 <<6 );
}

int type_4 (int opcode, int dr, int baseR1, int offset) {
	check_opcode(opcode);
	check_register(dr);
	check_register(baseR1);
	return (opcode  << 12) | (dr << 9) | (baseR1 << 6) | offset;
}

int type_5 (int opcode, int dr, int sr1, int bit1, int bit2, int offset) {
	check_opcode(opcode);
	check_register(dr);
	check_register(sr1);
	return (opcode  << 12) | (dr << 9) | (sr1 << 6) | (bit1<<5) | (bit2 <<4) | (offset);
}

int type_6 (int opcode, int dr, int offset) {
	check_opcode(opcode);
	return (opcode  << 12) | dr << 9 | (offset);
}


int type_7 (int opcode, int dr, int sr1, int bit, int imm) {
	check_opcode(opcode);
	check_register(dr);
	check_register(sr1);
	return (opcode  << 12) | (dr << 9) | (sr1 << 6) | (bit<<5) | (imm & 0x1f);
}

int type_10 (int opcode, int trapvector) {
	check_opcode(opcode);
			/*	printf("Trap Vector: %d\n", trapvector);
				printf("op_code value: %d\n", opcode);*/
	return (opcode << 12) | (trapvector);
}

int type_11 (int opcode, int n, int z, int p, int offset) {
	check_opcode(opcode);
	/*printf("N: %d\n", n);
	printf("Z: %d\n", z);
	printf("P: %d\n", p);*/
	return (opcode  << 12) | (n << 11) | (z << 10) | (p << 9) | offset;
}

int type_12 (int opcode, int trapvector) {
	check_opcode(opcode);
	return (opcode  << 12) | (trapvector);
}

int contains_char(char* str, char c, int l) {
	int rv = 0;
	int i = 2;
	while(i < l) {
		if((str[i++]) == c) {
			rv = 1;
			break;
		}
	}
	return rv;
}





