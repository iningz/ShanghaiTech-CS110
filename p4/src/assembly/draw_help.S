.section .text
.globl Draw_help
.type Draw_help, @function
Draw_help:
	#deal with stack
	addi	sp, sp, -4
	sw	ra, 0(sp)

	#set arguments for line 1
	li	a0, 80	#set x coord
	li	a1, 8	#set y coord
	la	a2, help1	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for line 2
	li	a0, 80	#set x coord
	li	a1, 24	#set y coord
	la	a2, help2	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for food
	li	a0, 20	#set start x
	li	a1, 20	#set start y
	li	a2, 28	#set end x
	li	a3, 28	#set end y
	li	a4, 0xFFFF	#set to white
	#call draw function
	jal	LCD_Fill

	#set arguments for posion
	li	a0, 20	#set start x
	li	a1, 36	#set start y
	li	a2, 28	#set end x
	li	a3, 44	#set end y
	li	a4, 0XFC07	#set to maroon
	#call draw function
	jal	LCD_Fill

	#set arguments for line 3
	li	a0, 80	#load x coord
	li	a1, 40	#load y coord
	la	a2, help3	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for line 4
	li	a0, 80	#set x coord
	li	a1, 56	#set y coord
	la	a2, help4	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for line 5
	li	a0, 80	#set x coord
	li	a1, 72	#set y coord
	la	a2, help5	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#deal with stack
	lw	ra, 0(sp)
	addi	sp, sp, 4

	ret

.data
help1:	#first line text
	.string	"Try grow longer!"
help2:	#second line text
	.string ": length + 1"
help3:	#third line text
	.string	": length - 1"
help4:	#fourth line text
	.string	"Don't hit yourself"
help5:	#fifth line text
	.string	"PRESS ANY BUTTON"