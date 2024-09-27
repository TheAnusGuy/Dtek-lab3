# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# Choose the macro syntax for RARS or DTEK-V board.         #
# The syntax for RARS is probably due to its MIPS heritage. #
#############################################################
###################
# Macros for RARS #
###################
#.macro	PUSH (%reg)
#	addi sp,sp,-4
#	sw	%reg, 0(sp) 
#.end_macro

#.macro	POP (%reg)
#	lw	%reg, 0(sp)
#	addi sp,sp,4
#.end_macro
###################
# Macros for DTEK #
###################
.macro	PUSH reg
	addi sp,sp,-4
	sw \reg, 0(sp) 
.endm

.macro	POP reg
	lw	\reg, 0(sp)
	addi	sp,sp,4
.endm
#############################################################
 
	.data
	.align 2
mytime:	.word 	0x5957
timstr:	.asciz 	"text more text lots of text\0"
	.text
	.globl timetemplate, tick, time2string
	.globl delay, display_string

# Function for displaying a string with a newline at the end	
display_string:	
	li a7,4
	ecall
	li a0, 10
	li a7,11
	ecall
	jr ra
	
timetemplate:
	la	a0, timstr
	jal     display_string
	
	# wait a little
	li	a0, 1000		# ms
	jal	delay
	
	# call tick
	la 	a0, mytime
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)
	jal	time2string
	
	# go back and do it all again
	j	timetemplate

	
# tick: update time pointed to by $a0
tick:	lw	t0, 0(a0)	# get time
	addi	t0, t0, 1	# increase
	andi	t1, t0, 0xf	# check lowest digit
	sltiu	t2, t1, 0xa	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x6	# adjust lowest digit
	
	andi	t1, t0, 0xf0	# check next digit
	sltiu	t2, t1, 0x60	# if digit < 6, okay
	bnez	t2, tiend
	addi	t0, t0, 0xa0	# adjust digit
	
	li	t3, 0xF
	slli	t3, t3, 0x8
	and	t1, t0, t3	# check minute digit
	addi	t3, x0, 0xA
	slli	t3, t3, 0x8
	slt	t2, t1, t3	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x600	# adjust digit - this one's okay, it's lower than 0x7FF 
	
	li	t3, 0xF
	slli	t3, t3, 0xC
	and	t1, t0, t3	# check last digit
	addi	t3, x0, 0x6
	slli	t3, t3, 0xC
	slt	t2, t1, t3	# if digit < 6, okay
	bnez	t2, tiend
	
	li	t3, 0xA
	slli	t3, t3, 0xC
	add	t0, t0, t3	# adjust last digit
tiend:	sw	t0,0(a0)	# save updated result
	jr	ra		# return

# Time2string function
time2string:
	
	PUSH s0
	PUSH s1
	PUSH s2
	
	mv s0, a0
	mv s1, a1
	mv s2, ra
	
	sb zero, 5(s0)
	
	mv a0, s1
	jal hexasc
	sb a0, 4(s0)
	
	srli a0, s1, 4
	jal hexasc
	sb a0, 3(s0)
	
	li a0, 0x3A
	sb a0, 2(s0)
	
	srli a0, s1, 8
	jal hexasc
	sb a0, 1(s0)
	
	srli a0, s1, 12
	jal hexasc
	sb a0, 0(s0)

	mv a0, s0
	mv a1, s1
	mv ra, s2
	
	POP s2
	POP s1
	POP s0

	jr ra

# Hexasc function	
hexasc:
	andi a0, a0, 0x00F
	li t0, 9
	bgt a0, t0, lettertranslator
	addi a0, a0, 0x30
	jr ra

lettertranslator:
	li t1, -9
	add a0, a0, t1
	addi a0, a0, 0x40
	jr ra
	
# Delay function
delay:	
	PUSH ra
	li t0, 0
	
while: 
	blez a0, end_while_loop
	
	addi a0, a0, -1
	li t1, 10000000

for_loop:
	addi t0, t0, 1
	bge t0, t1, end_for_loop
	
	j for_loop
	
end_for_loop:
	j while

end_while_loop:
	POP ra
	jr ra