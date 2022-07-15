
#                               Project 1.2 RISC-V disassembler

#   You can suppose you will get the number of machine codes you should disassemble from 
#   "lines_of_machine_codes" and get the 32 bits-machine code from "machine_codes". 
#   Both of them are stored in file "input.S"

#   When we test your project, we will change the input files content.

# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

#Grade me

.data
# You can define your own data such as ".word", ".asciiz", ".byte"... here.
add_name:	.asciiz	"add"
or_name:	.asciiz	"or"
slt_name:	.asciiz	"slt"
sltu_name:	.asciiz	"sltu"
sll_name:	.asciiz	"sll"
jalr_name:	.asciiz	"jalr"
addi_name:	.asciiz	"addi"
ori_name:	.asciiz	"ori"
lb_name:	.asciiz	"lb"
lbu_name:	.asciiz	"lbu"
lw_name:	.asciiz	"lw"
sb_name:	.asciiz	"sb"
sw_name:	.asciiz	"sw"
beq_name:	.asciiz	"beq"
bne_name:	.asciiz	"bne"
blt_name:	.asciiz	"blt"
bge_name:	.asciiz	"bge"
jal_name:	.asciiz	"jal"
lui_name:	.asciiz	"lui"

registers:
	.asciiz	"x0 " #x0
	.asciiz	"ra "
	.asciiz	"sp "
	.asciiz	"gp "
	.asciiz	"tp "
	.asciiz	"t0 " #x5
	.asciiz	"t1 "
	.asciiz	"t2 "
	.asciiz	"s0 "
	.asciiz	"s1 "
	.asciiz	"a0 " #x10
	.asciiz	"a1 "
	.asciiz	"a2 "
	.asciiz	"a3 "
	.asciiz	"a4 "
	.asciiz	"a5 " #x15
	.asciiz	"a6 "
	.asciiz	"a7 "
	.asciiz	"s2 "
	.asciiz	"s3 "
	.asciiz	"s4 " #x20
	.asciiz	"s5 "
	.asciiz	"s6 "
	.asciiz	"s7 "
	.asciiz	"s8 "
	.asciiz	"s9 " #x25
	.asciiz	"s10"
	.asciiz	"s11"
	.asciiz	"t3 "
	.asciiz	"t4 "
	.asciiz	"t5 " #x30
	.asciiz	"t6 "

.text
.globl main
main:
	la	s0, lines_of_machine_codes
	lw	s0, 0(s0)		#s0: lines of machine code
	la	s1, machine_codes	#s1: address of machine codes
	addi	s2, x0, 0		#s2: current index of machine code
main_loop:
	#Calculate address of current machine code
	slli	t0, s2, 2
	add	t0, t0, s1
	lw	a0, 0(t0)
	#disassemble(inst);
	jal	disassemble
	#i++;
	addi	s2, s2, 1
	#loop when i < lines
	blt	s2, s0, main_loop

	#Exit loop, exit with code 0
	add	a1, x0, x0
	addi	a0, x0, 17
	ecall

#void disassemble(int inst);
disassemble:
	#Save ra
	addi	sp, sp, -4
	sw	ra, 0(sp)
	#t0 = opcode
	andi	t0, a0, 0x7F
check_rtype:
	#Check if is rtype
	addi	t1, x0, 0x33
	#If not, continue checking
	bne	t0, t1, check_itype
	#If rtype, call rtype(machine_code)
	jal	rtype
	#End checking
	j	check_end

check_itype:
	#Check if is itype
	addi	t1, x0, 0x13
	#If not, continue checking
	bne	t0, t1, check_itype_jalr
	#If itype, call itype(inst)
	jal	itype
	#End checking
	j	check_end

check_itype_jalr:
	#Check if is itype_jalr
	addi	t1, x0, 0x67
	#If not, continue checking
	bne	t0, t1, check_itype_load
	#If itype_jalr, call itype_jalr(inst)
	jal	itype_jalr
	#End checking
	j	check_end

check_itype_load:
	#Check if is itype_load
	addi	t1, x0, 0x03
	#If not, continue checking
	bne	t0, t1, check_stype
	#If itype_load, call itype_load(inst)
	jal	itype_load
	#End checking
	j	check_end

check_stype:
	#Check if is stype
	addi	t1, x0, 0x23
	#If not, continue checking
	bne	t0, t1, check_sbtype
	#If stype, call stype(inst)
	jal	stype
	#End checking
	j	check_end

check_sbtype:
	#Check if is sbtype
	addi	t1, x0, 0x63
	#If not, continue checking
	bne	t0, t1, check_utype
	#If sbtype, call sbtype(inst)
	jal	sbtype
	#End checking
	j	check_end

check_utype:
	#Check if is utype
	addi	t1, x0, 0x37
	#If not, continue checking
	bne	t0, t1, check_ujtype
	#If utype, call utype(inst)
	jal	utype
	#End checking
	j	check_end

check_ujtype:
	#Check if is ujtype
	addi	t1, x0, 0x6F
	#If not, exit with error
	bne	t0, t1, exit_error
	#If ujtype, call ujtype(inst)
	jal	ujtype
	#End checking
	j	check_end

check_end:
	#Print endl
	jal	print_endl
	
	#Load ra
	lw	ra, 0(sp)
	addi	sp, sp, 4
	ret

#void rtype(int inst);
rtype:
	#Save ra, s0, s1, s2
	addi	sp, sp, -16
	sw	s0, 12(sp)
	sw	s1, 8(sp)
	sw	s2, 4(sp)
	sw	ra, 0(sp)

	#Get rd => s0
	srli	s0, a0, 7
	andi	s0, s0, 0x1F

	#Get rs1 => s1
	srli	s1, a0, 15
	andi	s1, s1, 0x1F

	#Get rs2 => s2
	srli	s2, a0, 20
	andi	s2, s2, 0x1F

	#Get funct3
	srli	t0, a0, 12
	andi	t0, t0, 0x7

check_r_add:
	#Check if is add
	addi	t1, x0, 0x0
	#If not, continue checking
	bne	t0, t1, check_r_or
	#If true, load name of inst
	la	a0, add_name
	j	check_r_end

check_r_or:
	#Check if is or
	addi	t1, x0, 0x6
	#If not, continue checking
	bne	t0, t1, check_r_slt
	#If true, load name of inst
	la	a0, or_name
	j	check_r_end

check_r_slt:
	#Check if is slt
	addi	t1, x0, 0x2
	#If not, continue checking
	bne	t0, t1, check_r_sltu
	#If true, load name of inst
	la	a0, slt_name
	j	check_r_end

check_r_sltu:
	#Check if is sltu
	addi	t1, x0, 0x3
	#If not, continue checking
	bne	t0, t1, check_r_sll
	#If true, load name of inst
	la	a0, sltu_name
	j	check_r_end

check_r_sll:
	#Check if is sll
	addi	t1, x0, 0x1
	#If not, exit with error
	bne	t0, t1, exit_error
	#If true, load name of inst
	la	a0, sll_name
	j	check_r_end

check_r_end:
	#Print name using address in a0
	jal	print_name
	#Print a space
	jal	print_space
	#Load and print rd
	mv	a0, s0
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print rs1
	mv	a0, s1
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print rs2
	mv	a0, s2
	jal	print_register

	#Load ra, s0, s1, s2
	lw	s0, 12(sp)
	lw	s1, 8(sp)
	lw	s2, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 16
	ret

#void itype(int inst);
itype:
	#Save ra, s0, s1, s2
	addi	sp, sp, -16
	sw	s0, 12(sp)
	sw	s1, 8(sp)
	sw	s2, 4(sp)
	sw	ra, 0(sp)

	#Get rd => s0
	srli	s0, a0, 7
	andi	s0, s0, 0x1F

	#Get rs1 => s1
	srli	s1, a0, 15
	andi	s1, s1, 0x1F

	#Get funct3
	srli	t0, a0, 12
	andi	t0, t0, 0x7

	#Get imm => s2
	jal	immgen_i
	mv	s2, a0

check_i_addi:
	#Check if is addi
	addi	t1, x0, 0x0
	#If not, continue checking
	bne	t0, t1, check_i_ori
	#If true, load name of inst
	la	a0, addi_name
	j	check_i_end

check_i_ori:
	#Check if is ori
	addi	t1, x0, 0x6
	#If not, exit with error
	bne	t0, t1, exit_error
	#If true, load name of inst
	la	a0, ori_name
	j	check_i_end

check_i_end:
	#Print name using address in a0
	jal	print_name
	#Print a space
	jal	print_space
	#Load and print rd
	mv	a0, s0
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print rs1
	mv	a0, s1
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print imm
	mv	a0, s2
	jal	print_imm

	#Load ra, s0, s1, s2
	lw	s0, 12(sp)
	lw	s1, 8(sp)
	lw	s2, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 16
	ret

#void itype_jalr(int inst);
itype_jalr:
	#Save ra, s0, s1, s2
	addi	sp, sp, -16
	sw	s0, 12(sp)
	sw	s1, 8(sp)
	sw	s2, 4(sp)
	sw	ra, 0(sp)

	#Get rd => s0
	srli	s0, a0, 7
	andi	s0, s0, 0x1F

	#Get rs1 => s1
	srli	s1, a0, 15
	andi	s1, s1, 0x1F

	#Get funct3
	srli	t0, a0, 12
	andi	t0, t0, 0x7

	#Get imm => s2
	jal	immgen_i
	mv	s2, a0

check_i_jalr_jalr:
	#Check if is jalr
	addi	t1, x0, 0x0
	#If not, exit with error
	bne	t0, t1, exit_error
	#If true, load name of inst
	la	a0, jalr_name
	j	check_i_jalr_end

check_i_jalr_end:
	#Print name using address in a0
	jal	print_name
	#Print a space
	jal	print_space
	#Load and print rd
	mv	a0, s0
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print rs1
	mv	a0, s1
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print imm
	mv	a0, s2
	jal	print_imm

	#Load ra, s0, s1, s2
	lw	s0, 12(sp)
	lw	s1, 8(sp)
	lw	s2, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 16
	ret

#void itype_load(int inst);
itype_load:
	#Save ra, s0, s1, s2
	addi	sp, sp, -16
	sw	s0, 12(sp)
	sw	s1, 8(sp)
	sw	s2, 4(sp)
	sw	ra, 0(sp)

	#Get rd => s0
	srli	s0, a0, 7
	andi	s0, s0, 0x1F

	#Get rs1 => s1
	srli	s1, a0, 15
	andi	s1, s1, 0x1F

	#Get funct3
	srli	t0, a0, 12
	andi	t0, t0, 0x7

	#Get imm => s2
	jal	immgen_i
	mv	s2, a0

check_i_load_lb:
	#Check if is lb
	addi	t1, x0, 0x0
	#If not, continue checking
	bne	t0, t1, check_i_load_lbu
	#If true, load name of inst
	la	a0, lb_name
	j	check_i_load_end

check_i_load_lbu:
	#Check if is lbu
	addi	t1, x0, 0x4
	#If not, continue checking
	bne	t0, t1, check_i_load_lw
	#If true, load name of inst
	la	a0, lbu_name
	j	check_i_load_end

check_i_load_lw:
	#Check if is lw
	addi	t1, x0, 0x2
	#If not, exit with error
	bne	t0, t1, exit_error
	#If true, load name of inst
	la	a0, lw_name
	j	check_i_load_end

check_i_load_end:
	#Print name using address in a0
	jal	print_name
	#Print a space
	jal	print_space
	#Load and print rd
	mv	a0, s0
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print imm
	mv	a0, s2
	jal	print_imm
	#Print left bracket
	jal	print_lb
	#Load and print rs1
	mv	a0, s1
	jal	print_register
	#Print right bracket
	jal	print_rb

	#Load ra, s0, s1, s2
	lw	s0, 12(sp)
	lw	s1, 8(sp)
	lw	s2, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 16
	ret

#void stype
stype:
	#Save ra, s0, s1, s2, s3
	addi	sp, sp, -20
	sw	s0, 16(sp)
	sw	s1, 12(sp)
	sw	s2, 8(sp)
	sw	s3, 4(sp)
	sw	ra, 0(sp)

	#Get funct3 => s0
	srli	s0, a0, 12
	andi	s0, s0, 0x7

	#Get rs1 => s1
	srli	s1, a0, 15
	andi	s1, s1, 0x1F

	#Get rs2 => s2
	srli	s2, a0, 20
	andi	s2, s2, 0x1F

	#Get imm => s3
	jal	immgen_s
	mv	s3, a0

check_s_sb:
	#Check if is sb
	addi	t0, x0, 0x0
	#If not, continue checking
	bne	t0, s0, check_s_sw
	#If true, load name of sb
	la	a0, sb_name
	j	check_s_end

check_s_sw:
	#Check if is sw
	addi	t0, x0, 0x2
	#If not, exit with error
	bne	t0, s0, exit_error
	#If true, load name of sw
	la	a0, sw_name
	j	check_s_end

check_s_end:
	#Print name using address in a0
	jal	print_name
	#Print a space
	jal	print_space
	#Load and print rs2
	mv	a0, s2
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print imm
	mv	a0, s3
	jal	print_imm
	#Print left bracket
	jal	print_lb
	#Load and print rs1
	mv	a0, s1
	jal	print_register
	#Print right bracket
	jal	print_rb

	#Load ra, s0, s1, s2, s3
	lw	s0, 16(sp)
	lw	s1, 12(sp)
	lw	s2, 8(sp)
	lw	s3, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 20
	ret

#void sbtype(int inst);
sbtype:
	#Save ra, s0, s1, s2, s3
	addi	sp, sp, -20
	sw	s0, 16(sp)
	sw	s1, 12(sp)
	sw	s2, 8(sp)
	sw	s3, 4(sp)
	sw	ra, 0(sp)

	#Get funct3 => s0
	srli	s0, a0, 12
	andi	s0, s0, 0x7

	#Get rs1 => s1
	srli	s1, a0, 15
	andi	s1, s1, 0x1F

	#Get rs2 => s2
	srli	s2, a0, 20
	andi	s2, s2, 0x1F

	#Get imm => s3
	jal	immgen_sb
	mv	s3, a0

check_sb_beq:
	#Check if is beq
	addi	t0, x0, 0x0
	#If not, continue checking
	bne	t0, s0, check_sb_bne
	#If true, load name of beq
	la	a0, beq_name
	j	check_sb_end

check_sb_bne:
	#Check if is bne
	addi	t0, x0, 0x1
	#If not, continue checking
	bne	t0, s0, check_sb_blt
	#If true, load name of bne
	la	a0, bne_name
	j	check_sb_end

check_sb_blt:
	#Check if is blt
	addi	t0, x0, 0x4
	#If not, continue checking
	bne	t0, s0, check_sb_bge
	#If true, load name of blt
	la	a0, blt_name
	j	check_sb_end

check_sb_bge:
	#Check if is bge
	addi	t0, x0, 0x5
	#If not, exit with error
	bne	t0, s0, exit_error
	#If true, load name of bge
	la	a0, bge_name
	j	check_sb_end

check_sb_end:
	#Print name using address in a0
	jal	print_name
	#Print a space
	jal	print_space
	#Load and print rs1
	mv	a0, s1
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print rs2
	mv	a0, s2
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print imm
	mv	a0, s3
	jal	print_imm

	#Load ra, s0, s1, s2, s3
	lw	s0, 16(sp)
	lw	s1, 12(sp)
	lw	s2, 8(sp)
	lw	s3, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 20
	ret

#void utype(int inst);
utype:
	#Save ra, s0, s1
	addi	sp, sp, -12
	sw	s0, 8(sp)
	sw	s1, 4(sp)
	sw	ra, 0(sp)

	#Get rd => s0
	srli	s0, a0, 7
	andi	s0, s0, 0x1F

	#Get imm => s1
	jal	immgen_u
	mv	s1, a0

	#Load name of lui
	la	a0, lui_name
	#Print name
	jal	print_name	
	#Print a space
	jal	print_space
	#Load and print rd
	mv	a0, s0
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print imm
	mv	a0, s1
	jal	print_imm

	#Load ra, s0, s1
	lw	s0, 8(sp)
	lw	s1, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 12
	ret

#void ujtype(int inst);
ujtype:
	#Save ra, s0, s1
	addi	sp, sp, -12
	sw	s0, 8(sp)
	sw	s1, 4(sp)
	sw	ra, 0(sp)

	#Get rd => s0
	srli	s0, a0, 7
	andi	s0, s0, 0x1F

	#Get imm => s1
	jal	immgen_uj
	mv	s1, a0

	#Load name of jal
	la	a0, jal_name
	#Print name
	jal	print_name
	#Print a space
	jal	print_space
	#Load and print rd
	mv	a0, s0
	jal	print_register
	#Print a comma
	jal	print_comma
	#Load and print imm
	mv	a0, s1
	jal	print_imm

	#Load ra, s0, s1
	lw	s0, 8(sp)
	lw	s1, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 12
	ret

#void print_endl();
print_endl:
	addi	a0, x0, 11
	addi	a1, x0, 10
	ecall
	ret

#void print_space();
print_space:
	addi	a0, x0, 11
	addi	a1, x0, 32
	ecall
	ret

#void print_comma();
print_comma:
	addi	a0, x0, 11
	addi	a1, x0, 44
	ecall
	ret

#void print_lb();
print_lb:
	addi	a0, x0, 11
	addi	a1, x0, 40
	ecall
	ret

#void print_rb();
print_rb:
	addi	a0, x0, 11
	addi	a1, x0, 41
	ecall
	ret

#void print_register(int num);
print_register:
	#Load register name address
	la	a1, registers
	slli	a0, a0, 2
	#Calculate address of register name
	add	a1, a1, a0
	addi	a0, x0, 4
	ecall
	ret

#void print_name(char* addr);
print_name:
	mv	a1, a0
	addi	a0, x0, 4
	ecall
	ret

#void print_imm(int imm);
print_imm:
	mv	a1, a0
	addi	a0, x0, 1
	ecall
	ret

exit_error:
	#Exit with code -1
	addi	a0, x0, 17
	addi	a1, x0, -1
	ecall

#int immgen_i(int inst);
immgen_i:
	srai	a0, a0, 20
	ret

#int immgen_s(int inst);
immgen_s:
	#Returning value
	addi	t0, x0, 0

	#Add imm[11:5] and sign extend
	srai	t1, a0, 25
	slli	t1, t1, 5
	add	t0, t0, t1

	#Add imm[4:0]
	srli	t1, a0, 7
	andi	t1, t1, 0x1F
	add	t0, t0, t1

	mv	a0, t0
	ret

#int immgen_sb(int inst);
immgen_sb:
	#Returning value
	addi	t0, x0, 0

	#Add imm[12] and sign extend
	srai	t1, a0, 31
	slli	t1, t1, 12
	add	t0, t0, t1

	#Add imm[10:5]
	srli	t1, a0, 20
	andi	t1, t1, 0x7E0
	add	t0, t0, t1

	#Add imm[4:1]
	srli	t1, a0, 7
	andi	t1, t1, 0x1E
	add	t0, t0, t1

	#Add imm[11]
	andi	t1, a0, 0x80
	slli	t1, t1, 4
	add	t0, t0, t1

	mv	a0, t0
	ret

#int immgen_u(int inst);
immgen_u:
	srli	a0, a0, 12
	ret
	
#int immgen_uj(int inst);
immgen_uj:
	#Returning value
	addi	t0, x0, 0

	#Add imm[20] and sign extend
	srai	t1, a0, 31
	slli	t1, t1, 20
	add	t0, t0, t1

	#Add imm[10:1]
	srli	t1, a0, 20
	andi	t1, t1, 0x7FE
	add	t0, t0, t1

	#Add imm[11]
	srli	t1, a0, 20
	andi	t1, t1, 0x1
	slli	t1, t1, 11
	add	t0, t0, t1

	#Add imm[19:12]
	srli	t1, a0, 12
	andi	t1, t1, 0xFF
	slli	t1, t1, 12
	add	t0, t0, t1

	mv	a0, t0
	ret