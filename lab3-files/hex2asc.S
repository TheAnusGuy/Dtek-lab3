  # hexmain.S
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text

 	.globl hex2asc
 	.global hexasc


hex2asc:
	li	a0, 17		# test number (from 0 to 15)
	
	addi    sp,sp,-4
	sw      ra,0(sp)
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout
	ecall

	lw      ra,0(sp)
	addi    sp,sp,4
	jr      ra
	
hexasc:

			

  # You can write your own code for hexasc here
  #
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
	
	
