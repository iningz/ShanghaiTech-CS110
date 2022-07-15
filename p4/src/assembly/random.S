.section .text
.globl Random_percentage
.type Random_percentage, @function
Random_percentage:
	#deal with stack
	addi	sp, sp, -8
	sw	s0, 4(sp)
	sw	ra, 0(sp)

	#save argument
	mv	s0, a0

	#calculate random
	jal	rand

	#modulo
	li	t0, 100
	rem	a0, a0, t0

	bge	a0, s0, no
yes:
	li	a0, 1
	j	return
no:
	li	a0, 0
return:

	#deal with stack
	lw	s0, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 8

	ret