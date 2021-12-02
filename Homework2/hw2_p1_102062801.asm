.data

PHello: .asciiz "This it the first part of output. (Load,Store,Add,Sub)\n\nA+B   A-B\n"
Pnewline: .asciiz "\n"

.text

#    Hello, this is the first MIPS assembly assignment.\nIn this assignment, you have to fill the code into the flower boxs.    (  ###############    blocks   )
#    Hope you enjoy in assembling.

la $a0, PHello	# load address of string to print
li $v0, 4	# ready to print string
syscall	# print

# =====================================================
# for grading . Do not write the code here 
li $t0, 45
sw $t0, 0($gp)
li $t1, 15
sw $t1, 4($gp)
# =====================================================


#    This block helps you proctice with the R-type instructions, including ADD and SUB.
#    Also  procticeing load and store, please read data form 0($gp) and 4($gp), 
#    and store 0($gp)+4($gp) to 8($gp)    store 0($gp)-4($gp) to 12($gp)    
#####################################################     write the code below
lw $t0, 0($gp) #保險起見，先把助教存入Memory的值Load出來到 $t0
lw $t1, 4($gp) #保險起見，先把助教存入Memory的值Load出來到 $t1

add $t2, $t0, $t1  #把 $t0 加上 $t1 後存入 $t2
sw $t2, 8($gp)     #把 $t2 存入 Memory
sub $t3, $t0, $t1  #把 $t0 減 $t1 後存入 $t3
sw $t3, 12($gp)    #把 $t3 存入 Memory
#####################################################


# =====================================================
# for grading . Do not write the code here 
lw $t0, 8($gp)
lw $t1, 12($gp)
li $v0, 1
move $a0, $t0
syscall	# print
la $a0, Pnewline	# load address of string to print
li $v0, 4	# ready to print string
syscall	# print
li $v0, 1
move $a0, $t1
syscall	# print
la $a0, Pnewline	# load address of string to print
li $v0, 4	# ready to print string
syscall	# print
# =====================================================
