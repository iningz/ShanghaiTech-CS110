.section .text
.globl Draw_scoreboard
.type Draw_scoreboard, @function
Draw_scoreboard:
	#deal with stack
	addi	sp, sp, -4
	sw	ra, 0(sp)

	#set arguments for score
	mv	a2, a0
	li	a0, 80
	li	a1, 2
	li	a3, 3
	li	a4, 0xFFE0
	#call draw function
	jal	LCD_ShowNum

	#set arguments for score info
	li	a0, 60
	li	a1, 10
	la	a2, score
	li	a3, 0xFFE0
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for title
	li	a0, 80
	li	a1, 30
	la	a2, info
	li	a3, 0xFFE0
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for play
	li	a0, 50
	li	a1, 60
	la	a2, retry
	li	a3, 0xFFFF
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for help
	li	a0, 110
	li	a1, 60
	la	a2, quit
	li	a3, 0xFFFF
	#call draw function
	jal	ShowString_PivotCenter

	#set arguments for selection box
	li	a0, 28
	li	a1, 50
	li	a2, 72
	li	a3, 70
	li	a4, 0xFFFF
	#call draw function
	jal	LCD_DrawRectangle

	#set arguments for selection box 2
	li	a0, 92
	li	a1, 50
	li	a2, 128
	li	a3, 70
	li	a4, 0X8430
	#call draw function
	jal	LCD_DrawRectangle

	#deal with stack
	lw	ra, 0(sp)
	addi	sp, sp, 4

	ret
	
.data
score:
	.string	"SCORE:"
info:
	.string	"GAME OVER"
retry:
	.string	"RETRY"
quit:
	.string	"QUIT"