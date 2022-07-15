/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/utils.h"
#include "src/tables.h"
#include "src/translate_utils.h"
#include "src/translate.h"

#include "assembler.h"

#define MAX_ARGS 3
#define BUF_SIZE 1024
const char* IGNORE_CHARS = " \f\n\r\t\v,()";

/*******************************
 * Helper Functions
 *******************************/

/* you should not be calling this function yourself. */
static void raise_label_error(uint32_t input_line, const char* label) {
    write_to_log("Error - invalid label at line %d: %s\n", input_line, label);
}

/* call this function if more than MAX_ARGS arguments are found while parsing
   arguments.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.

   EXTRA_ARG should contain the first extra argument encountered.
 */
static void raise_extra_argument_error(uint32_t input_line, const char* extra_arg) {
    write_to_log("Error - extra argument at line %d: %s\n", input_line, extra_arg);
}

/* You should call this function if write_pass_one() or translate_inst()
   returns -1.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
static void raise_instruction_error(uint32_t input_line, const char* name, char** args,
    int num_args) {

    write_to_log("Error - invalid instruction at line %d: ", input_line);
    log_inst(name, args, num_args);
}

/* Truncates the string at the first occurrence of the '#' character. */
static void skip_comments(char* str) {
    char* comment_start = strchr(str, '#');
    if (comment_start) {
        *comment_start = '\0';
    }
}

/* Reads STR and determines whether it is a label (ends in ':'), and if so,
   whether it is a valid label, and then tries to add it to the symbol table.

   INPUT_LINE is which line of the input file we are currently processing. Note
   that the first line is line 1 and that empty lines are included in this count.

   BYTE_OFFSET is the offset of the NEXT instruction (should it exist).

   Four scenarios can happen:
    1. STR is not a label (does not end in ':'). Returns 0.
    2. STR ends in ':', but is not a valid label. Returns -1.
    3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int add_if_label(uint32_t input_line, char* str, uint32_t byte_offset,
    SymbolTable* symtbl) { /* Decide how to operate on a label */

    size_t len = strlen(str);
    if (str[len - 1] == ':') { /* If really a label */
        str[len - 1] = '\0';
        if (is_valid_label(str)) { /* Test validity */
            if (add_to_table(symtbl, str, byte_offset) == 0) {
                return 1;
            } else { /* Failed to add */
                return -1;
            }
        } else {
            raise_label_error(input_line, str); /* Raise error */
            return -1;
        }
    } else { /* Not a label */
        return 0;
    }
}

/*******************************
 * Implement the Following
 *******************************/

/* First pass of the assembler. You should implement pass_two() first.

   This function should read each line, strip all comments, scan for labels,
   and pass instructions to write_pass_one(). The symbol table should also
   been built and written to specified file. The input file may or may not
   be valid. Here are some guidelines:

    1. Only one label may be present per line. It must be the first token present.
        Once you see a label, regardless of whether it is a valid label or invalid
        label, treat the NEXT token as the beginning of an instruction.
    2. If the first token is not a label, treat it as the name of an instruction.
        DO NOT try to check it is a valid instruction in this pass.
    3. Everything after the instruction name should be treated as arguments to
        that instruction. If there are more than MAX_ARGS arguments, call
        raise_extra_argument_error() and pass in the first extra argument. Do not
        write that instruction to the output file (eg. don't call write_pass_one())
    4. Only one instruction should be present per line. You do not need to do
        anything extra to detect this - it should be handled by guideline 3.
    5. A line containing only a label is valid. The address of the label should
        be the byte offset of the next instruction, regardless of whether there
        is a next instruction or not.
    6. If an instruction contains an immediate, you should output it AS IS.
    7. Comments should always be skipped before any further process.

   Just like in pass_two(), if the function encounters an error it should NOT
   exit, but process the entire file and return -1. If no errors were encountered,
   it should return 0.
 */
int pass_one(FILE* input, FILE* inter, FILE* symtbl) {
    /* YOUR CODE HERE */
    char* tokenStart;
    uint32_t lineNumber, offset;
    int labelRet, error;
    unsigned int lineWritten; /* Record the lines written */
    SymbolTable* table;

    /* A buffer for line parsing. */
    char buf[BUF_SIZE] __attribute__((unused));

    /* Variables for argument parsing. */
    char *args[MAX_ARGS] __attribute__((unused));
    int num_args __attribute__((unused));

    /* For each line, there are some hints of what you should do:
        1. Skip all comments
        2. Use `strtok()` to read next token
        3. Deal with labels
        4. Parse the instruction
     */
    if (!input || !inter || !symtbl) return -1; /* In case the inputs are invalid */
    table = create_table(SYMBOLTBL_UNIQUE_NAME);

    error = 0; /* Initialize the variables */
    lineNumber = 0;
    offset = 0;
    while (fgets(buf, BUF_SIZE, input)) { /* While get a line */
        char name[256];
        lineNumber += 1; /* This is the new line, thus line number + 1 */
        skip_comments(buf); /* Strip comments */
        tokenStart = strtok(buf, IGNORE_CHARS); /* Try read first token */
        if (tokenStart == NULL) continue; /* If no content (empty line), read next line */
        labelRet = add_if_label(lineNumber, tokenStart, offset, table); /* Check label */
        if (labelRet == -1) {
            error = -1;
            continue;
        } /* If error raised, ready to return -1 */
        if (labelRet != 0) { /* Label processed, read next as the name */
            tokenStart = strtok(NULL, IGNORE_CHARS); /* Read the next as the name */
            if (tokenStart == NULL) continue; /* If no content (line with a label only), read next line */
        }
        strcpy(name, tokenStart); /* Set the name as the current token */
        tokenStart = strtok(NULL, IGNORE_CHARS); /* Read the next token */
        num_args = 0; /* Reset the arg count */
        while (tokenStart != NULL) { /* While there are contents */
            if (num_args >= MAX_ARGS) { /* If currently there are already 3 args */
                raise_extra_argument_error(lineNumber, tokenStart); /* Raise the error with lineNumber and the extra */
                error = -1; /* Ready to return -1 */
                num_args = -1; /* To tell following line not to write intermediate */
                break; /* Stop reading args */
            }
            args[num_args] = tokenStart; /* Save the arg */
            num_args += 1; /* Add to arg count */
            tokenStart = strtok(NULL, IGNORE_CHARS); /* Read the next token(arg) */
        }
        if (num_args != -1) { /* If no extra args */
            lineWritten = write_pass_one(inter, name, args, num_args);
            if (lineWritten == 0) { /* Check how many lines are written */
                raise_instruction_error(lineNumber, name, args, num_args);
                error = -1;
            }
            offset += 4 * lineWritten; /* Add to the address */
        }
    }
    write_table(table, symtbl);
    free_table(table);

    return error; /* Return -1 if error */
}

/* Second pass of the assembler.

   This function should read an intermediate file and the corresponding symbol table
   file, translates it into machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table file is well formatted
    6. The symbol table file contains all the symbol required for translation
   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered. */
int pass_two(FILE* inter, FILE* symtbl, FILE* output) {
    /* YOUR CODE HERE */
    int curr_lineno = 1;
    int error_risen = 0;
    SymbolTable* table;
    uint32_t addr = 0;

    /* A buffer for line parsing. */
    char buf[BUF_SIZE] __attribute__((unused));

    /* Variables for argument parsing. */
    char *args[MAX_ARGS]__attribute__((unused));
    int num_args = 0;
    
    if (!inter || !symtbl || !output) return -1; /* In case the inputs are invalid */

    /* Write text segment. */
    fprintf(output, ".text\n");

    /* Create symbol table here by reading the symbol table file. */
     table = create_table_from_file(SYMBOLTBL_UNIQUE_NAME, symtbl);

    /* For each line, there are some hints of what you should do:
        1. Get instruction name.
        2. Parse instruction arguments; Extra arguments should be filtered out in
         `pass_one()`, so you don't need to worry about that here.
        3. Use `translate_inst()` to translate the instruction and write to the
         output file;
        4. Or if an error occurs, call `raise_instruction_error()` instead of write
         the instruction.
     */
    while (fgets(buf, BUF_SIZE, inter)) {
        char name[256];
        char* token = strtok(buf, IGNORE_CHARS); /* readline into a buffer */
        strcpy(name, token); /* save the instruction name */
        num_args = 0;

        token = strtok(NULL, IGNORE_CHARS); /* subsequent calls */
        while(token) {
            args[num_args++] = token;
            token = strtok(NULL, IGNORE_CHARS); /* subsequent calls */
        }

        if(translate_inst(output, name, args, num_args, addr, table) != 0) {
            error_risen = 1; /* set the flag */
            raise_instruction_error(curr_lineno, name, args, num_args);
        }
        else
            addr += 4; /* write only inst correct */

        ++curr_lineno;
    }

    /* Write symbol segment. */
    fprintf(output, "\n.symbol\n");
    /* Write symbols here by calling `write_table()` */
    write_table(table, output);
    free_table(table); /* free the memory */

    if (!error_risen)
        return 0; /* success */
    return -1;
}

/*******************************
 * Do Not Modify Code Below
 *******************************/

static int open_files_pass_one(FILE** input, FILE** inter, FILE** symtbl,
    const char* input_name, const char* inter_name, const char* symtbl_name) {

    *input = fopen(input_name, "r");
    if (!*input) { /* Adding coment! */
        write_to_log("Error: unable to open input file: %s\n", input_name);
        return -1;
    } /* Adding coment! */
    *inter = fopen(inter_name, "w");
    if (!*inter) {
        write_to_log("Error: unable to open intermediate file: %s\n", inter_name);
        fclose(*input); /* Adding coment! */
        return -1;
    }
    *symtbl = fopen(symtbl_name, "w");
    if (!*symtbl) { /* Adding coment! */
        write_to_log("Error: unable to open symbol table file: %s\n", symtbl_name);
        fclose(*input);
        fclose(*inter);
        return -1;
    } /* Adding coment! */
    return 0;
}

static int open_files_pass_two(FILE** inter, FILE** symtbl, FILE** output,
    const char* inter_name, const char* symtbl_name, const char* output_name) { /* Adding coment! */

    *inter = fopen(inter_name, "r");
    if (!*inter) { /* Adding coment! */
        write_to_log("Error: unable to open intermediate file: %s\n", inter_name);
        return -1;
    } /* Adding coment! */
    *symtbl = fopen(symtbl_name, "r");
    if (!*symtbl) {
        write_to_log("Error: unable to open symbol table file: %s\n", inter_name);
        fclose(*inter); /* Adding coment! */
        return -1;
    }
    *output = fopen(output_name, "w");
    if (!*output) { /* Adding coment! */
        write_to_log("Error: unable to open output file: %s\n", output_name);
        fclose(*inter);
        fclose(*symtbl); /* Adding coment! */
        return -1;
    }
    return 0; /* Adding coment! */
}

static void close_files(FILE* file1, FILE* file2, FILE* file3) {
    fclose(file1);
    fclose(file2); /* Adding coment! */
    fclose(file3);
}

/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
   and pass_two().
 */
int assemble(const char *in, const char *out, const char *int_, const char *sym) {
    FILE *input, *inter, *symtbl, *output;
    int err = 0; /* Declare variables */

    if (in) {
        printf("Running pass one: %s -> %s, %s\n", in, int_, sym);
        if (open_files_pass_one(&input, &inter, &symtbl, in, int_, sym) != 0) { /* Pass one */
            exit(1);
        }

        if (pass_one(input, inter, symtbl) != 0) { /* Check error */
            err = 1;
        }
        close_files(input, inter, symtbl); /* Close file */
    }

    if (out) {
        printf("Running pass two: %s, %s -> %s\n", int_, sym, out);
        if (open_files_pass_two(&inter, &symtbl, &output, int_, sym, out) != 0) { /* Pass two */
            exit(1);
        }

        if (pass_two(inter, symtbl, output) != 0) { /* Check error */
            err = 1;
        }

        close_files(inter, symtbl, output); /* Close file */
    }

    return err;
}

static void print_usage_and_exit() {
    printf("Usage:\n"); /* Adding coment! */
    printf("  Runs both passes: assembler <input file> <intermediate file> <symbol table file> <output file>\n");
    printf("  Run pass #1:      assembler -p1 <input file> <intermediate file> <symbol table file>\n");
    printf("  Run pass #2:      assembler -p2 <intermediate file> <symbol table> <output file>\n");
    printf("Append -log <file name> after any option to save log files to a text file.\n");
    exit(0); /* Adding coment! */
}

int main(int argc, char **argv) {
    int mode = 0; /* Adding coment! */
    char *input, *inter, *output, *symtbl;
    int err;

    if (argc != 5 && argc != 7) {
        print_usage_and_exit(); /* Adding coment! */
    }

    if (strcmp(argv[1], "-p1") == 0) {
        mode = 1; /* Adding coment! */
    } else if (strcmp(argv[1], "-p2") == 0) {
        mode = 2;
    }

    if (mode == 1) {
        input = argv[2]; /* Adding coment! */
        inter = argv[3];
        symtbl = argv[4];
        output = NULL;
    } else if (mode == 2) {
        input = NULL; /* Adding coment! */
        inter = argv[2];
        symtbl = argv[3];
        output = argv[4];
    } else {
        input = argv[1]; /* Adding coment! */
        inter = argv[2];
        symtbl = argv[3];
        output = argv[4];
    }

    if (argc == 7) {
        if (strcmp(argv[5], "-log") == 0) { /* Adding coment! */
            set_log_file(argv[6]);
        } else {
            print_usage_and_exit(); /* Adding coment! */
        }
    }

    err = assemble(input, output, inter, symtbl); /* Adding coment! */

    if (err) {
        write_to_log("One or more errors encountered during assembly operation.\n");
    } else {
        write_to_log("Assembly operation completed successfully!\n"); /* Adding coment! */
    }

    if (is_log_file_set()) {
        printf("Results saved to %s\n", argv[5]); /* Adding coment! */
    }

    return err;
}
