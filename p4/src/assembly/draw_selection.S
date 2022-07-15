.section .text
.globl Draw_selection
.type Draw_selection, @function
Draw_selection:
	#deal with stack
	addi	sp, sp, -4
	sw	ra, 0(sp)

	beq	a4, zero, draw_gray
draw_white:
	li	a4, 0xFFFF
	j draw
draw_gray:
	li	a4, 0X8430
draw:
	jal	LCD_DrawRectangle

	#deal with stack
	lw	ra, 0(sp)
	addi	sp, sp, 4

	ret
