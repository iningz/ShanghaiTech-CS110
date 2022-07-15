.section .text
.globl ShowNum_PivotCenter
.type ShowNum_PivotCenter, @function
ShowNum_PivotCenter:
	#save return address
	addi	sp, sp, -4
	sw	ra, 0(sp)

	#calculate x
	slli	t0, a3, 2
	sub	a0, a0, t0

	#calculate y
	addi	a1, a1, -8

	#call draw function
	jal	LCD_ShowNum

	#deal with stack
	lw	ra, 0(sp)
	addi	sp, sp, 4

	ret