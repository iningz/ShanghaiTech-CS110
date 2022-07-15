# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.import input.S

.text
.globl	main

insert:
	# a0: address of heap
	# a1: maximum size
	# a2: address of current len
	# a3: input value
	addi	sp, sp, -36		# push regs on to the stack
	sw	ra, 0(sp)		# store return address
	sw	s1, 4(sp)		# store
	sw	s2, 8(sp)		# store
	sw	s3, 12(sp)		# store
	sw	s4, 16(sp)		# store
	sw	s5, 20(sp)		# store
	sw	s6, 24(sp)		# store
	sw	s7, 28(sp)		# store
	sw	s8, 32(sp)		# store

	mv	s1, a0			# s1 = heap address
	mv	s2, a1			# s2 = maximum size
	lw	s3, 0(a2)		# s3 = current len
	mv	s4, a3			# s4 = input value
	bgt	s3, s2, insert_err	# overflow
	slli	s5, s3, 2		# get the memory location (*4)
	add	s5, s5, s1		# s5 = address of the new item
	sw	s4, 0(s5)		# store the input
	addi	s3, s3, 1		# increment size
	mv	s6, s3			# s6 = current index
	sw	s3, 0(a2)		# update heap size

swim:
	srli	s7, s6, 1		# s7 = parent index (/2)
	beq	s7, zero, insert_done	# if s6 already root node
	slli	s8, s7, 2		# get the memory location (*4)
	addi	s8, s8, -4		# go back one word
	add	s8, s8, s1		# s8 = parent address
	lw	t3, 0(s5)		# t3 = value at current address
	lw	t4, 0(s8)		# t4 = value at parent address
	ble	t4, t3, insert_done	# if parent <= current stop swimming
	sw	t3, 0(s8)		# swap at parent address
	sw	t4, 0(s5)		# swap at current address
	mv	s6, s7			# update current index with parent
	mv	s5, s8			# update current address with parent
	j	swim			# recurse

insert_err:
	lw	ra, 0(sp)		# load return address
	lw	s1, 4(sp)		# load
	lw	s2, 8(sp)		# load
	lw	s3, 12(sp)		# load
	lw	s4, 16(sp)		# load
	lw	s5, 20(sp)		# load
	lw	s6, 24(sp)		# load
	lw	s7, 28(sp)		# load
	lw	s8, 32(sp)		# load
	addi	sp, sp, 36		# revert stack
	li	a1, -1			# error code -1
	j	exit			# exit

insert_done:
	lw	ra, 0(sp)		# load return address
	lw	s1, 4(sp)		# load
	lw	s2, 8(sp)		# load
	lw	s3, 12(sp)		# load
	lw	s4, 16(sp)		# load
	lw	s5, 20(sp)		# load
	lw	s6, 24(sp)		# load
	lw	s7, 28(sp)		# load
	lw	s8, 32(sp)		# load
	addi	sp, sp, 36		# revert stack
	j	loop			# return

pop:
	# ARGS:
	#	a0: address of heap
	#	a1: maximum size
	#	a2: address of current len
	# RETURNS:
	#	a1: the popped (smallest) item
	addi	sp, sp, -40		# push regs on to the stack
	sw	ra, 0(sp)		# store return address
	sw	s1, 4(sp)		# store
	sw	s2, 8(sp)		# store
	sw	s3, 12(sp)		# store
	sw	s4, 16(sp)		# store
	sw	s5, 20(sp)		# store
	sw	s6, 24(sp)		# store
	sw	s7, 28(sp)		# store
	sw	s8, 32(sp)		# store
	sw	s9, 36(sp)		# store

	mv	s1, a0			# s1 = heap address
	mv	s2, a1			# s2 = maximum size
	lw	s3, 0(a2)		# s3 = current len
	beq	s3, zero, pop_err	# underflow
	lw	a1, 0(s1)		# a1 = the popped (smallest) item
	li	a0, 1			# print
	ecall
	slli	s4, s3, 2		# get the memory location (*4)
	addi	s4, s4, -4		# go back one word
	add	s4, s4, s1		# s4 = address of the last item
	lw	t3, 0(s4)		# t3 = the last item
	sw	t3, 0(s1)		# store at the top
	addi	s3, s3, -1		# decrement size
	sw	s3, 0(a2)		# update heap size
	mv	s5, s1			# s5 = current address
	li	s6, 1			# s6 = current index

pre_sink:
	slli	s7, s6, 1		# s7 = Left child index (*2)
	bgt	s7, s3, pop_done	# if already no left child
	slli	s8, s7, 2		# get the memory location (*4)
	addi	s8, s8, -4		# go back one word
	add	s8, s8, s1		# s8 = left child address
	lw	s9, 0(s8)		# s9 = value of the left child
	beq	s7, s3, sink		# if left child is the last
	addi	t3, s7, 1		# t3 = right child index
	slli	t4, t3, 2		# get the memory location (*4)
	addi	t4, t4, -4		# go back one word
	add	t4, t4, s1		# t4 = right child address
	lw	t5, 0(t4)		# t5 = value of the right child
	ble	s9, t5, sink		# if left smaller continue, else…
	mv	s7, t3			# s7 = smaller child index
	mv	s8, t4			# s8 = smaller child address
	mv	s9, t5			# s9 = value of smaller child

sink:
	lw	t3, 0(s5)		# s10 = value at current address
	bge	s9, t3, pop_done	# if smaller child >= current stop sinking
	sw	t3, 0(s8)		# swap at smaller child address
	sw	s9, 0(s5)		# swap at current address
	mv	s5, s8			# update current address with smaller child
	mv	s6, s7			# update current index with smaller child
	j	pre_sink		# recurse

pop_err:
	lw	ra, 0(sp)		# load return address
	lw	s1, 4(sp)		# load
	lw	s2, 8(sp)		# load
	lw	s3, 12(sp)		# load
	lw	s4, 16(sp)		# load
	lw	s5, 20(sp)		# load
	lw	s6, 24(sp)		# load
	lw	s7, 28(sp)		# load
	lw	s8, 32(sp)		# load
	lw	s9, 36(sp)		# load
	addi	sp, sp, 40		# revert stack
	li	a1, -1			# error code -1
	j	exit			# exit

pop_done:
	lw	ra, 0(sp)		# load return address
	lw	s1, 4(sp)		# load
	lw	s2, 8(sp)		# load
	lw	s3, 12(sp)		# load
	lw	s4, 16(sp)		# load
	lw	s5, 20(sp)		# load
	lw	s6, 24(sp)		# load
	lw	s7, 28(sp)		# load
	lw	s8, 32(sp)		# load
	lw	s9, 36(sp)		# load
	addi	sp, sp, 40		# revert stack
	li	a1, '\n'		# newline to print
	li	a0, 11			# print
	ecall
	j	loop			# return

print_heap:
	# a0: adress of heap
	# a2: address of current len
	addi	sp, sp, -12		# push regs to the stack
	sw	ra, 0(sp)		# store return address
	sw	s1, 4(sp)		# store
	sw	s2, 8(sp)		# store
	mv	s1, a0			# s1 = heap address
	lw	s2, 0(a2)		# s2 = current len
	beq	s2, zero, print_done	# nothing to print
	mv	t3, s1			# t3 = current item address

print_loop:
	lw	a1, 0(t3)		# a1 = value to print
	li	a0, 1			# print
	ecall
	addi	s2, s2, -1		# s2 = number of remaining items
	beq	s2, zero, print_done	# printed all
	li	a1, ' '			# space to print
	li	a0, 11			# print
	ecall
	addi	t3, t3, 4		# next item
	j	print_loop		# next step

print_done:
	lw	ra, 0(sp)		# load return address
	lw	s1, 4(sp)		# load
	lw	s2, 8(sp)		# load
	addi	sp, sp, 12		# revert stack
	li	a1, '\n'		# newline to print
	li	a0, 11			# print
	ecall
	j	loop

main:
	lw	t0, input_len		# get length of input
	mv	t1, zero		# initialize t0 for iterating
	la	t2, input		# get input

loop:
	la	a0, heap		# a0 = address of heap
	li	a1, 32			# a1 = maximum size
	la	a2, len			# a2 = address of current len
	lw	a3, 0(t2)		# a3 = next input
	addi	t1, t1, 1		# iterate
	bgt	t1, t0, done		# already processed the last
	addi	t2, t2, 4		# next word
	li	t3, -1			# t3 = pop code
	li	t4, -2			# t4 = print code
	bge	a3, zero, insert	# insert a3
	beq	a3, t3, pop		# pop
	beq	a3, t4, print_heap	# print

done:
	li	a1, 0			# 0 to return

exit:
	li	a0, 17			# return with code in a1
	ecall