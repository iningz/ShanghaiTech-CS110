.data

# Constant integer specifying the lines of machine codes

# DO NOT MODIFY THIS VARIABLE
.globl lines_of_machine_codes
lines_of_machine_codes:
    .word 19


# 32-bits machine codes
# A 32-bits hexadecimal number represents one line of machine code.
# You can suppose all of the input machine codes are valid. 

# DO NOT MODIFY THIS VARIABLE
.globl machine_codes
machine_codes:
    .word 0x00208033
    .word 0x005261B3
    .word 0x0083A333
    .word 0x00B534B3
    .word 0x00E69633
    .word 0xFFB807E7
    .word 0x40090893
    .word 0xC00AEA13
    .word 0x02CB8B03
    .word 0xFE4CCC03
    .word 0x1BCDAD03
    .word 0xC9CE8423
    .word 0x37EFAC23
    .word 0x18628A63
    .word 0xE68396E3
    .word 0x7CA4C863
    .word 0x82C5D8E3
    .word 0x7551B6EF
    .word 0xF4240737
    #.word 0x000502B3    # add  t0, a0, x0
    #.word 0x00100313    # addi t1, x0, 1
    #.word 0x00028863    # beq  t0, x0, 16
    #.word 0x01DE13B3    # sll  t2, t3, t4
    #.word 0xFFF28293    # addi t0, t0, -1
    #.word 0xFF5FF06F    # jal  x0, -12
    #.word 0x00600533    # add  a0, x0, t1
    #.word 0x00008067    # jalr x0, ra, 0
