.section .text
.globl Draw_menu
.type Draw_menu, @function
Draw_menu:
	#deal with stack
	addi	sp, sp, -4
	sw	ra, 0(sp)

	#set arguments for title
	li	a0, 80	#set x coord
	li	a1, 30	#set y coord
	la	a2, title	#load text
	li	a3, 0xFFE0	#set to yellow
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for play
	li	a0, 30	#set x coord
	li	a1, 60	#set y coord
	la	a2, play	#load text
	li	a3, 0xFFFF	#set to yellow
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for options
	li	a0, 80
	li	a1, 60
	la	a2, options
	li	a3, 0xFFFF
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for help
	li	a0, 130	#set x coord
	li	a1, 60	#set y coord
	la	a2, help	#load text
	li	a3, 0xFFFF	#set to yellow
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for selection box
	li	a0, 12	#set start x
	li	a1, 50	#set start y
	li	a2, 48	#set end x
	li	a3, 70	#set end y
	li	a4, 0xFFFF	#set to white
	#call draw function
	jal	LCD_DrawRectangle

	#set arguments for selection box 1.5
	li	a0, 50	#set start x
	li	a1, 50	#set start y
	li	a2, 110	#set end x
	li	a3, 70	#set end y
	li	a4, 0x8430	#set to gray
	#call draw function
	jal	LCD_DrawRectangle

	#set arguments for selection box 2
	li	a0, 112	#set start x
	li	a1, 50	#set start y
	li	a2, 148	#set end x
	li	a3, 70	#set end y
	li	a4, 0X8430	#set to gray
	#call draw function
	jal	LCD_DrawRectangle

	#deal with stack
	lw	ra, 0(sp)
	addi	sp, sp, 4

	ret

.data
title:	#title text
	.string	"RETRO SNAKE GAME"
play:	#play button text
	.string	"PLAY"
options:	#options button text
	.string	"OPTIONS"
help:	#help button text
	.string	"HELP"