.text
	la $a0, array		
	la $a1, array_size	
	lw $a1, 0($a1)		
loop:
	sll $t1, $t0, 2		
	add $t2, $a0, $t1	
	sw $t0, 0($t2)		
	addi $t0, $t0, 1	
	add $t4, $t4, $t0
	slt $t3, $t0, $a1	
	bne $t3, $zero, loop	
	la $a0, message		
	add $a0, $t4, $zero	
	nop			

.data
	array: .word 0:10
	array_size: .word 10
	message: .asciiz "The sum of numbers in array is: "