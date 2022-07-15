.section .text
.globl ShowString_PivotCenter
.type ShowString_PivotCenter, @function
ShowString_PivotCenter:
	#save return address
	addi	sp, sp, -20
	sw	s0, 16(sp)
	sw	s1, 12(sp)
	sw	s2, 8(sp)
	sw	s3, 4(sp)
	sw	ra, 0(sp)
	#save registers to save arguments
	
	#save arguments
	mv	s0, a0
	mv	s1, a1
	mv	s2, a2
	mv	s3, a3

	#calculate strlen
	mv	a0, a2
	jal	strlen

	#calculate x
	slli	a0, a0, 2
	sub	a0, s0, a0

	#calculate y
	addi	a1, s1, -8

	#pass arguments
	mv	a2, s2
	mv	a3, s3

	#call draw function
	jal	LCD_ShowString

	#deal with stack
	lw	s0, 16(sp)
	lw	s1, 12(sp)
	lw	s2, 8(sp)
	lw	s3, 4(sp)
	lw	ra, 0(sp)
	addi	sp, sp, 20
	#load save registers for caller

	ret