.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra
write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra
fact:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
addi $fp, $sp, 8
li $t0, 1
beq $a0, $t0, label0
j label1
label0:
move $v0, $a0
lw $ra, -4($fp)
lw $fp, -8($fp)
jr $ra
j label2
label1:
addi $t1, $a0, -1
addi $sp, $sp, -4
sw $t1, 0($sp)
lw $t0, -4($fp)
move $a0, $t0
jal fact
addi $sp, $sp, 4
lw $t1, -4($fp)
move $t1, $v0
mul $t2, $a0, $t1
move $v0, $t2
lw $ra, -4($fp)
lw $fp, -8($fp)
jr $ra
label2:
main:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
addi $fp, $sp, 8
jal read
move $t3, $v0
move $t4, $t3
li $t5, 1
bgt $t4, $t5, label3
j label4
label3:
addi $sp, $sp, -4
sw $t0, 0($sp)
addi $sp, $sp, -4
sw $t1, 0($sp)
addi $sp, $sp, -4
sw $t2, 0($sp)
addi $sp, $sp, -4
sw $t3, 0($sp)
addi $sp, $sp, -4
sw $t4, 0($sp)
lw $t0, -28($fp)
move $a0, $t0
jal fact
addi $sp, $sp, 20
lw $t4, -28($fp)
lw $t3, -24($fp)
lw $t2, -20($fp)
lw $t1, -16($fp)
lw $t0, -12($fp)
move $t1, $v0
move $t2, $t1
j label5
label4:
li $t2, 1
label5:
move $a0, $t2
jal write
li $t3, 0
move $v0, $t3
lw $ra, -4($fp)
lw $fp, -8($fp)
jr $ra
