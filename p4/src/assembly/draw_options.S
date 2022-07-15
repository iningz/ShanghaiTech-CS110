.section .text
.globl Draw_options
.type Draw_options, @function
Draw_options:
	#deal with stack
	addi	sp, sp, -4
	sw	ra, 0(sp)

	#set arguments for title
	li	a0, 80	#set x coord
	li	a1, 10	#set y coord
	la	a2, optionTitle	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for speed
	li	a0, 50	#set x coord
	li	a1, 30	#set y coord
	la	a2, optionSpeed	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for difficulty
	li	a0, 50	#set x coord
	li	a1, 50	#set y coord
	la	a2, optionDifficulty	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for return
	li	a0, 80	#set x coord
	li	a1, 70	#set y coord
	la	a2, optionReturn	#load text
	li	a3, 0xFFFF	#set to white
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for selecton box 1
	li	a0, 10	#set start x
	li	a1, 21	#set start y
	li	a2, 150	#set end x
	li	a3, 39	#set end y
	li	a4, 0xFFFF	#set to white
	#call draw function
	jal	LCD_DrawRectangle

	#set arguments for selection box 2
	li	a0, 10	#set start x
	li	a1, 41	#set start y
	li	a2, 150	#set end x
	li	a3, 59	#set end y
	li	a4, 0X8430	#set to gray
	#call draw function
	jal	LCD_DrawRectangle

	#set arguments for selection box 3
	li	a0, 10	#set start x
	li	a1, 61	#set start y
	li	a2, 150	#set end x
	li	a3, 79	#set end y
	li	a4, 0x8430	#set to gray
	#call draw function
	jal	LCD_DrawRectangle

	#deal with stack
	lw	ra, 0(sp)
	addi	sp, sp, 4

	ret

.data
optionTitle:
	.string	"OPTIONS"
optionSpeed:
	.string	"SPEED:"
optionDifficulty:
	.string	"POISON:"
optionReturn:
	.string	"RETURN"