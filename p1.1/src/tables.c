/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tables.h"
#include "utils.h"

const int SYMBOLTBL_NON_UNIQUE = 0;
const int SYMBOLTBL_UNIQUE_NAME = 1;

/*******************************
 * Helper Functions
 *******************************/

void allocation_failed() {
    write_to_log("Error: allocation failed\n"); /* Adding coment! */
    exit(1);
}

void addr_alignment_incorrect() {
    write_to_log("Error: address is not a multiple of 4.\n"); /* Adding coment! */
}

void name_already_exists(const char* name) {
    write_to_log("Error: name '%s' already exists in table.\n", name); /* Adding coment! */
}

void write_sym(FILE* output, uint32_t addr, const char* name) {
    fprintf(output, "%u\t%s\n", addr, name); /* Adding coment! */
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time.
   If memory allocation fails, you should call allocation_failed().
   Mode will be either SYMBOLTBL_NON_UNIQUE or SYMBOLTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable* create_table(int mode) {
    /* YOUR CODE HERE */
    SymbolTable* ret = (SymbolTable*)malloc(sizeof(SymbolTable)); /* Allocate memory for the table struct */
    if (!ret) {
        allocation_failed(); /* Throw an error if allocation failed */
        return NULL;
    }
    ret->head = ret->tail = NULL; /* Initialize the variables */
    ret->mode = mode;
    return ret; /* Return the address of the table */
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable* table) {
    /* YOUR CODE HERE */
    Symbol *it, *next;
    if (!table) return; /* In case the table is NULL */
    it = table->head; /* Set the iterator */
    while (it) {
        next = it->next;
        free(it->name); /* Free the memory of the name */
        free(it); /* Free the memory of the node */
        it = next;
    }
    free(table); /* Free the table struct */
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE.
   1. ADDR is given as the byte offset from the first instruction.
   2. The SymbolTable must be able to resize itself as more elements are added.

   3. Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   4. If ADDR is not word-aligned, you should call addr_alignment_incorrect()
   and return -1.

   5. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists
   in the table, you should call name_already_exists() and return -1.

   6.If memory allocation fails, you should call allocation_failed().

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable* table, const char* name, uint32_t addr) {
    /* YOUR CODE HERE */
    Symbol* node; /* Temp variable */
    if (!table || !name) { /* In case the inputs are NULL */
        return-1;
    }
    if (addr % 4 != 0) { /* Checks whether ADDR is word-aligned */
        addr_alignment_incorrect();
        return -1;
    }
    if (table->mode == SYMBOLTBL_UNIQUE_NAME && get_addr_for_symbol(table, name) != -1) { /* Check the uniqueness */
        name_already_exists(name);
        return -1;
    }
    node = (Symbol*)malloc(sizeof(Symbol)); /* Allocate memory for the Symbol node */
    if (!node) {
        allocation_failed(); /* In case the allocation is failed */
        return -1;
    }
    node->next = NULL; /* Initialize the node */
    node->name = (char*)malloc(sizeof(char) * (strlen(name) + 1)); /* Allocate memory for the name */
    if (!node->name) {
        allocation_failed(); /* In case the allocation is failed */
        free(node);
        return -1;
    }
    strcpy(node->name, name); /* Copy the name into the node */
    node->addr = addr;
    if (!table->head) {
        table->head = table->tail = node; /* If no symbol, set head and tail */
    } else {
        table->tail->next = node;
        table->tail = node; /* Set the pointers */
    }
    return 0;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable* table, const char* name) {
    /* YOUR CODE HERE */
    Symbol* it;
    if (!table || !name) return -1; /* In case the inputs are NULL */
    it = table->head;
    while (it) {
        if (!strcmp(name, it->name)) { /* Traverse the table and find the name */
            return it->addr;
        }
        it = it->next; /* Move the iterator */
    }
    return -1;
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_sym() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable* table, FILE* output) {
    /* YOUR CODE HERE */
    Symbol* it;
    if (!table || !output) return; /* In case the inputs are NULL */
    it = table->head;
    while (it) {
        write_sym(output, it->addr, it->name); /* Write each entry to output */
        it = it->next;
    }
}

/* Reads the symbol table file written by the `write_table` function, and
   restore the symbol table.
 */
SymbolTable* create_table_from_file(int mode, FILE* file) {
    /* YOUR CODE HERE */
    SymbolTable* ret; /* Pointer to table */
    uint32_t addr;
    char name[256]; /* Buffer for name */
    if (!file || !(ret = create_table(mode))) return NULL; /* In case the file invalid */
    while (fscanf(file, "%u\t%s\n", &addr, name) == 2) {
        add_to_table(ret, name, addr); /* Add entry */
    }
    return ret; /* Return table */
}
