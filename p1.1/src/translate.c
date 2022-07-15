/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li, bge and mv pseudoinstructions. Your pseudoinstruction
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are
   valid, since that will be checked in part two.

   Also for li:
    - make sure that the number is representable by 32 bits. (Hint: the number
        can be both signed or unsigned).
    - if the immediate can fit in the imm field of an addiu instruction, then
        expand li into a single addi instruction. Otherwise, expand it into
        a lui-addi pair.

   And for bge and move:
    - your expansion should use the fewest number of instructions possible.

   venus has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if venus behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
    long tempLong = 0;
    int32_t temp = 0; /* Used to check immediates */
    if (!output || !name || !args)
        return 0;

    /* Deal with pseudo-instructions. */
    if (strcmp(name, "li") == 0) {
        /* YOUR CODE HERE */
        if (num_args != 2) return 0;
        if (!translate_num(&tempLong, args[1], -2147483648, 4294967295)) { /* If the imm can fit 32 bit */
            temp = tempLong;
            /*printf("32: %d, 64: %ld\n", temp, tempLong);*/
            if (temp >= -2048 && temp <= 2047) { /* If can fit in 12bit */
                fprintf(output, "addi %s x0 %s\n", args[0], args[1]);
                return 1;
            } /* Expand to lui-addi pair */
            if ((temp & 0x800) == 0x0) { /* No extending */
                fprintf(output, "lui %s %d\n", args[0], (temp >> 12) & 0xFFFFF);
                fprintf(output, "addi %s %s %d\n", args[0], args[0], temp & 0xFFF);
            } else { /* Sign extend for addi */
                fprintf(output, "lui %s %d\n", args[0], ((temp >> 12) + 1) & 0xFFFFF);
                fprintf(output, "addi %s %s %d\n", args[0], args[0], temp | 0xFFFFF000);
            }
            return 2;
        }
        return 0;
    } else if (strcmp(name, "bgez") == 0) { /* Branch greater equal than zero */
        /* YOUR CODE HERE */
        if (num_args != 2) return 0;
        fprintf(output, "bge %s x0 %s\n", args[0], args[1]); /* Expand with bge */
        return 1;
    } else if (strcmp(name, "mv") == 0) { /* Move */
        /* YOUR CODE HERE */
        if (num_args != 2) return 0;
        fprintf(output, "addi %s %s 0\n", args[0], args[1]); /* Expand with addi */
        return 1;
    } else if (strcmp(name, "beqz") == 0) { /* Branch equal to zero */
        if (num_args != 2) return 0;
        fprintf(output, "beq %s x0 %s\n", args[0], args[1]); /* Expand with beq */
        return 1;
    } else if (strcmp(name, "j") == 0) { /* Jump */
        if (num_args != 1) return 0;
        fprintf(output, "jal zero %s\n", args[0]); /* Expand with jal */
        return 1;
    } else if (strcmp(name, "jal") == 0 && num_args == 1) { /* Jump and link */
        fprintf(output, "jal ra %s\n", args[0]); /* Add ra */
        return 1;
    } else if (strcmp(name, "jr") == 0) { /* Jump register */
        if (num_args != 1) return 0;
        fprintf(output, "jalr zero %s 0\n", args[0]); /* Expand with jalr */
        return 1;
    } else if (strcmp(name, "jalr") == 0 && num_args == 1) { /* Jump and link register */
        fprintf(output, "jalr ra %s 0\n", args[0]); /* Add ra and imm = 0 */
        return 1;
    } else {
        write_inst_string(output, name, args, num_args); /* no need to expand */
        return 1;
    }
}


/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step.

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write
   anything to OUTPUT but simply return -1. venus may be a useful resource for
   this step.

   Note the use of helper functions. Consider writing your own! If the function
   definition comes afterwards, you must declare it first (see translate.h).

   Returns 0 on success and -1 on error.
 */
int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl) {
    if (strcmp(name, "add") == 0) /* rtype */
        return write_rtype(0x33, 0x00, 0x00, output, args, num_args);
    else if (strcmp(name, "or") == 0)
        return write_rtype(0x33, 0x06, 0x00, output, args, num_args);
    else if (strcmp(name, "sll") == 0) /* sll */
        return write_rtype(0x33, 0x01, 0x00, output, args, num_args);
    else if (strcmp(name, "slt") == 0)
        return write_rtype(0x33, 0x02, 0x00, output, args, num_args);
    else if (strcmp(name, "sltu") == 0) /* sltu */
        return write_rtype(0x33, 0x03, 0x00, output, args, num_args);
    /* YOUR CODE HERE */
    else if (strcmp(name, "addi") == 0) /* itype */
        return write_itype(0x13, 0x00, output, args, num_args);
    else if (strcmp(name, "ori") == 0)
        return write_itype(0x13, 0x06, output, args, num_args);
    else if (strcmp(name, "lui") == 0) /* utype */
        return write_utype(0x37, output, args, num_args);
    else if (strcmp(name, "lb") == 0) /* itype */
        return write_itype_inverse(0x03, 0x00, output, args, num_args);
    else if (strcmp(name, "lbu") == 0)
        return write_itype_inverse(0x03, 0x04, output, args, num_args);
    else if (strcmp(name, "lw") == 0) /* lw */
        return write_itype_inverse(0x03, 0x02, output, args, num_args);
    else if (strcmp(name, "sb") == 0) /* stype */
        return write_stype(0x23, 0x00, output, args, num_args);
    else if (strcmp(name, "sw") == 0)
        return write_stype(0x23, 0x02, output, args, num_args);
    else if (strcmp(name, "beq") == 0) /* sbtype */
        return write_sbtype(0x63, 0x00, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "bne") == 0)
        return write_sbtype(0x63, 0x01, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "blt") == 0) /* blt */
        return write_sbtype(0x63, 0x04, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "bge") == 0)
        return write_sbtype(0x63, 0x05, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "jal") == 0) /* ujtype */
        return write_ujtype(0x6F, output, args, num_args, addr, symtbl);
    else if (strcmp(name, "jalr") == 0)
        return write_itype(0x67, 0x0, output, args, num_args);
    return -1;
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t opcode, uint8_t funct3, uint8_t funct7, FILE* output, char** args, size_t num_args) {
    /*  Perhaps perform some error checking? */
    int rd, rs, rt;
    uint32_t instruction = 0;
    if (num_args != 3)
        return -1; /* number of args error checking */

    /* add rd, rs, rt */
    rd = translate_reg(args[0]);
    rs = translate_reg(args[1]);
    rt = translate_reg(args[2]);
    /* REMOVE THE FOLLOWING LINE. IT'S FOR MAKING YOUR CODE TO COMPILE */
    /* YOUR CODE HERE */
    /* error checking for register ids */
    if (rd < 0 || rs < 0 || rt < 0) /* if translate_reg returns error */
        return -1;

    /* generate instruction */
    instruction = (funct7 << 25) | (rt << 20) |
                  (rs << 15) | (funct3 << 12) | (rd << 7) | opcode;

    write_inst_hex(output, instruction);
    return 0; /* success */
}

int write_itype_inverse(uint8_t opcode, uint8_t funct3, FILE* output, char** args, size_t num_args) {
    /* YOUR CODE HERE */
    int rd, rs; long imm64; int32_t imm;
    uint32_t instruction = 0;
    if (num_args != 3)
        return -1; /* number of args error checking */

    /* addi rd, rs, imm */
    rd = translate_reg(args[0]);
    if (strlen(args[1]) > 2 && args[1][0] == '0' && args[1][1] == 'x') { /* If hex: */
        if (translate_num(&imm64, args[1], 0, 4095)) { /* If not fit 12bit unsigned */
            if (translate_num(&imm64, args[1], 0, 4294967295)) { /* If not fit 32bit */
                return -1;
            } else { /* If fit 32-bit signed */
                imm = imm64;
                if (imm < -2048 || imm > 2047) return -1; /* If signed 32-bit not fit 12-bit */
            }
        } else { /* If fit 12bit unsigned */
            imm = imm64;
        }
    } else { /* If Decimal: */
        if (translate_num(&imm64, args[1], -2048, 2047)) return -1; /* If not fit 12bit */
        imm = imm64;
    }
    rs = translate_reg(args[2]);
    


    /* error checking for register ids */
    if (rd < 0 || rs < 0) /* if translate_reg returns error */
        return -1;

    /* generate instruction */
    instruction = (imm << 20) |
                  (rs << 15) | (funct3 << 12) | (rd << 7) | opcode;

    write_inst_hex(output, instruction);
    return 0; /* success */
}

int write_itype(uint8_t opcode, uint8_t funct3, FILE* output, char** args, size_t num_args) {
    /* YOUR CODE HERE */
    int rd, rs; long imm64; int32_t imm;
    uint32_t instruction = 0;
    if (num_args != 3)
        return -1; /* number of args error checking */

    /* addi rd, rs, imm */
    rd = translate_reg(args[0]);
    rs = translate_reg(args[1]);
    if (strlen(args[2]) > 2 && args[2][0] == '0' && args[2][1] == 'x') { /* If hex: */
        if (translate_num(&imm64, args[2], 0, 4095)) { /* If not fit 12bit unsigned */
            if (translate_num(&imm64, args[2], 0, 4294967295)) { /* If not fit 32bit */
                return -1;
            } else { /* If fit 32-bit signed */
                imm = imm64;
                if (imm < -2048 || imm > 2047) return -1; /* If signed 32-bit not fit 12-bit */
            }
        } else { /* If fit 12bit unsigned */
            imm = imm64;
        }
    } else { /* If Decimal: */
        if (translate_num(&imm64, args[2], -2048, 2047)) return -1; /* If not fit 12bit */
        imm = imm64;
    }


    /* error checking for register ids */
    if (rd < 0 || rs < 0) /* if translate_reg returns error */
        return -1;

    /* generate instruction */
    instruction = (imm << 20) |
                  (rs << 15) | (funct3 << 12) | (rd << 7) | opcode;

    write_inst_hex(output, instruction);
    return 0; /* success */
}

int write_stype(uint8_t opcode, uint8_t funct3, FILE* output, char** args, size_t num_args) {
    /* YOUR CODE HERE */
    int rs, rt; long imm64; int32_t imm;
    int im1, im2; /* to split immediate */
    uint32_t instruction = 0;
    if (num_args != 3)
        return -1; /* number of args error checking */

    /* sd rt, imm(rs) */
    rt = translate_reg(args[0]);
    if (strlen(args[1]) > 2 && args[1][0] == '0' && args[1][1] == 'x') { /* If hex: */
        if (translate_num(&imm64, args[1], 0, 4095)) { /* If not fit 12bit unsigned */
            if (translate_num(&imm64, args[1], 0, 4294967295)) { /* If not fit 32bit */
                return -1;
            } else { /* If fit 32-bit signed */
                imm = imm64;
                if (imm < -2048 || imm > 2047) return -1; /* If signed 32-bit not fit 12-bit */
            }
        } else { /* If fit 12bit unsigned */
            imm = imm64;
        }
    } else { /* If Decimal: */
        if (translate_num(&imm64, args[1], -2048, 2047)) return -1; /* If not fit 12bit */
        imm = imm64;
    }
    rs = translate_reg(args[2]);

    /* error checking for register ids */
    if (rs < 0 || rt < 0) /* if translate_reg returns error */
        return -1;

    /* split immediate */
    im1 = imm & ((1 << 5) - 1); /* [4:0] */
    im2 = (imm >> 5) & 0x7F;

    /* generate instruction */
    instruction = (im2 << 25) | (rt << 20) |
                  (rs << 15) | (funct3 << 12) | (im1 << 7) | opcode;

    write_inst_hex(output, instruction);
    return 0; /* success */
}

/* Hint: the way for branch to calculate relative address. e.g. bne
     bne rs rt label
   assume the byte_offset(addr) of label is L,
   current instruction byte_offset(addr) is A
   the relative address I for label satisfy:
     L = A + I
   so the relative address is
     I = L - A              */
int write_sbtype(uint8_t opcode, uint8_t funct3, FILE* output, char** args, size_t num_args,
    uint32_t addr, SymbolTable* symtbl) {
    /* YOUR CODE HERE */
    int rs, rt;
    int64_t label_addr;
    int offset;
    int im1, im2, im3, im4; /* to split address */
    uint32_t instruction = 0;
    if (num_args != 3)
        return -1; /* number of args error checking */

    /* beq rs, rt, symbol */
    rs = translate_reg(args[0]);
    rt = translate_reg(args[1]);
    label_addr = get_addr_for_symbol(symtbl, args[2]);

    /* error checking for register ids and addr */
    if (rs < 0 || rt < 0 || label_addr == -1) /* if error returned */
        return -1;

    offset = label_addr - addr;
    if (offset < -4096 || offset > 4095)
        return -1;

    /* split immediate */
    im1 = (offset >> 11) & 1; /* [11] */
    im2 = (offset >> 1) & ((1 << 4) - 1); /* [4:1] */
    im3 = (offset >> 5) & ((1 << 7) - 1); /* [10:5] */
    im4 = (offset >> 12) & 1; /* [12] */

    /* generate instruction */
    instruction = (im4 << 31) | (im3 << 25) | (rt << 20) |
                  (rs << 15) | (funct3 << 12) | (im2 << 8) | (im1 << 7) | opcode;

    write_inst_hex(output, instruction);
    return 0; /* success */
}

int write_utype(uint8_t opcode, FILE* output, char** args, size_t num_args) {
    /* YOUR CODE HERE */
    int rd; long imm;
    uint32_t instruction = 0;
    if (num_args != 2)
        return -1; /* number of args error checking */

    /* lui rd, imm */
    rd = translate_reg(args[0]);
    if (translate_num(&imm, args[1], 0, 1048575)) {
        return -1;
    }

    /* error checking for register ids */
    if (rd < 0) /* if translate_reg returns error */
        return -1;

    /* generate instruction */
    instruction = (imm << 12) |
                  (rd << 7) | opcode;

    write_inst_hex(output, instruction);
    return 0; /* success */
}

/* In this project there is no need to relocate labels,
   you may think about the reasons. */
int write_ujtype(uint8_t opcode, FILE* output, char** args, size_t num_args,
    uint32_t addr, SymbolTable* symtbl) {
    /* YOUR CODE HERE */
    int rd;
    int64_t label_addr;
    int offset;
    int im1, im2, im3, im4; /* to split address */
    uint32_t instruction = 0;
    if (num_args != 2)
        return -1; /* number of args error checking */

    /* jal rd, symbol */
    rd = translate_reg(args[0]);
    label_addr = get_addr_for_symbol(symtbl, args[1]);

    /* error checking for register ids and addr */
    if (rd < 0 || label_addr == -1) /* if error returned */
        return -1;

    offset = label_addr - addr;
    if (offset < -4096 || offset > 4095)
        return -1;

    /* split immediate */
    im1 = (offset >> 12) & ((1 << 8) - 1); /* [19:12] */
    im2 = (offset >> 11) & 1; /* [11] */
    im3 = (offset >> 1) & ((1 << 10) - 1); /* [10:1] */
    im4 = (offset >> 20) & 1; /* [20] */

    /* generate instruction */
    instruction = (im4 << 31) | (im3 << 21) | (im2 << 20) | (im1 << 12) |
                  (rd << 7) | opcode;

    write_inst_hex(output, instruction);
    return 0; /* success */
}
