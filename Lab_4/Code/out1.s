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
main:
addi $sp, $sp, -4
sw $ra, 0($sp)
addi $sp, $sp, -4
sw $fp, 0($sp)
addi $fp, $sp, 8
li $t0, 0
li $t1, 1
li $t2, 0
jal read
move $t3, $v0
move $t4, $t3
label0:
blt $t2, $t4, label1
j label2
label1:
add $t5, $t0, $t1
move $t6, $t5
move $a0, $t1
jal write
move $t0, $t1
move $t1, $t6
addi $t7, $t2, 1
move $t2, $t7
j label0
label2:
li $s0, 0
move $v0, $s0
lw $ra, -4($fp)
lw $fp, -8($fp)
jr $ra
