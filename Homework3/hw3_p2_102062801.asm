#############################
#  �Ǹ�:   102062801                      #
#  �m�W:   �]�ԬR                            #
# CS4100 - �p������c HW#3-2  #
#############################
.data

str1:
.asciiz "input n: "
str2:
.asciiz "fibird: "

.text



#######################   �{���}�l
Main: 

#n = 0;
add $s0, $zero, $zero
#�ƥΤ@��$s1�Ȧs�����s2 (�Φb���k)
addi $s1, $s1, 2

#   �L�X input n:
li $v0, 4             
la $a0, str1
syscall 

#   INPUT   n  
li $v0, 5 
syscall
addi $s0, $v0, 0      #   ��$v0��s��$s0

#���ˬd��J���S���p�󵥩�0�A�����ܸ��hL2
slti $t0, $s0, 1
bne $t0, $zero, L2

#��J���`�i�Jrecur�Ƶ{��
jal recur

#   �L�X fibird:
li $v0, 4
la $a0, str2
syscall 

#   �L�X s2
li $v0, 1             
addi $a0, $s2, 0 
syscall

#  ���ᵲ��
j exit



#  Recur�Ƶ{���}�l
recur:

#  ����ra�Pn����
addi $sp, $sp, -8
sw $ra, 4($sp)
sw $s0, 0($sp)

#  �ˬdn���S���p��2 (����1)
slti $t0, $s0, 2

#  �S�����ܸ��hL1
beq $t0, $zero, L1 #�j�󪺮ɭԸ��hL1

#  �����ܱ���ۺ�A��1��Js2
addi $s2, $zero, 1

#  ��_stack
addi $sp, $sp, 8

#  ���^�����L�Ӫ��U�@�ӫ��O��}
jr $ra



#  L1�Ƶ{���}�l
L1:

#  ��n-1
addi $s0, $s0, -1
#  ���^recur�Ƶ{���~�����
jal recur

#  ��_ra�Pn��
lw $s0, 0($sp)
lw $ra, 4($sp)
#  ��_stack
addi $sp, $sp, 8

#  ����2*recur (n-1) 
mul $s2, $s2, $s1
#  �A��+n
add $s2, $s2, $s0

#  ���^�����L�Ӫ��U�@�ӫ��O��}
jr $ra



#  �Ƶ{��L2
L2:
#  ������0��Js2
add $s2, $zero, $zero

#   �L�X fibird:
li $v0, 4           
la $a0, str2
syscall 

#   �L�X s2
li $v0, 1             
addi $a0, $s2, 0      
syscall

#  ���ᵲ��
j exit


#  �{������
exit: