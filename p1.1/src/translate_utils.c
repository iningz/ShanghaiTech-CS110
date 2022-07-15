/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "translate_utils.h"

void write_inst_string(FILE* output, const char* name, char** args, int num_args) {
    int i;

    fprintf(output, "%s", name);
    for (i = 0; i < num_args; i++) { /* Adding coment! */
        fprintf(output, " %s", args[i]);
    }
    fprintf(output, "\n"); /* Adding coment! */
}

void write_inst_hex(FILE *output, uint32_t instruction) {
    fprintf(output, "%08x\n", instruction); /* Adding coment! */
}

int is_valid_label(const char* str) {
    int first = 1; /* Adding coment! */
    if (!str) {
        return 0;
    }

    while (*str) { /* Adding coment! */
        if (first) {
            if (!isalpha((int) *str) && *str != '_') {
              return 0;   /* does not start with letter or underscore */
            } else {
                first = 0;
            }
        } else if (!isalnum((int) *str) && *str != '_') {
          return 0;       /* subsequent characters not alphanumeric */
        }
        str++;
    }
    return first ? 0 : 1;   /* empty string is invalid  */
}

/* Translate the input string into a signed number. The number is then
   checked to be within the correct range (note bounds are INCLUSIVE)
   ie. NUM is valid if LOWER_BOUND <= NUM <= UPPER_BOUND.

   The input may be in either positive or negative, and be in either
   decimal or hexadecimal format. It is also possible that the input is not
   a valid number. Fortunately, the library function strtol() can take
   care of all that (with a little bit of work from your side of course).
   Please read the documentation for strtol() carefully. Do not use strtoul()
   or any other variants.

   You should store the result into the location that OUTPUT points to. The
   function returns 0 if the conversion proceeded without errors, or -1 if an
   error occurred.
 */
int translate_num(long int* output, const char* str, long int lower_bound,
    long int upper_bound) {

    long int temp = 0; /* ????YOUR CODE HERE? */
    char* endptr = NULL;
    if (!str || !output) {
        return -1;
    }
    /* YOUR CODE HERE */
    /*if (strlen(str) > 2 && str[0] == '0' && str[1] == 'x'){
        upper_bound -= lower_bound;
    }*/
    temp = strtol(str, &endptr, 0); /* Get the endptr to decide whether the conversion is a success */
    if (endptr == str || temp < lower_bound || temp > upper_bound) { /* Check the validity of NUM */
        return -1;
    }
    *output = temp; /* Output the result */
    return 0;
}
/* Translates the register name to the corresponding register number. Please
   see the RISC-V Green Sheet for information about register numbers.

   Returns the register number of STR or -1 if the register name is invalid.
 */

int digit(const char* str) {
    if (str && '0' <= str[0] && str[0] <= '9') {
        if (strlen(str) == 1)
            return atoi(str); /* 1 digit */
        if (strlen(str) == 2 && '0' <= str[1] && str[1] <= '9')
            return atoi(str); /* 2 digits */
    }
    return -1; /* return on error */
}

int translate_reg(const char* str) {
    char c = str[0];
    long num;
    /* int num = digit(str + 1); */

    if (!strcmp(str, "zero"))       return 0; /* zero */
    if (!strcmp(str, "ra"))         return 1; /* return address */
    if (!strcmp(str, "sp"))         return 2; /* stack pointer */
    if (!strcmp(str, "gp"))         return 3; /* global pointer */
    if (!strcmp(str, "tp"))         return 4; /* thread pointer */
    if (!strcmp(str, "s0") ||                 /* saved register */
        !strcmp(str, "fp"))         return 8; /* frame pointer */
    if (!strcmp(str, "s1"))         return 9; /* saved register */
    if (translate_num(&num, str + 1, 0, 31)) { /* Read number */
        return -1;
    }
    if (c == 'a' &&                           /* function arguments */
        0 <= num && num <= 7)       return 10 + num;
    if (c == 's' &&                           /* saved registers */
        2 <= num && num <= 11)      return 16 + num;
    if (c == 't'){                            /* temporaries */
        if (0 <= num && num <= 2)   return 5 + num;
        if (3 <= num && num <= 6)   return 25 + num;
    }
    if (c == 'x' &&                           /* registers */
        0 <= num && num <= 31)      return num;

    return -1; /* return on error */
}
