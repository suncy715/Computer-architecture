#############################
#  �Ǹ�:   102062801                      #
#  �m�W:   �]�ԬR                            #
# CS4100 - �p������c HW#3-1  #
#############################
.data

str1:
.asciiz "input a: "
str2:
.asciiz "input b: "
str3:
.asciiz "input c: "
str4:
.asciiz "result = "

.text
#############################   �{���}�l
Main: 

#initial
add $s0, $zero, $zero # a = 0
add $s1, $zero, $zero # b = 0
add $s2, $zero, $zero # c = 0
add $s3, $zero, $zero # d = 0

#   �L�X input a:
li $v0, 4
la $a0, str1
syscall

#   INPUT   a    
li $v0, 5    
syscall
addi $s0, $v0, 0      #   ��$v0��s��$s0

#   �L�X input b:
li $v0, 4             
la $a0, str2
syscall 

#   INPUT   b   
li $v0, 5                
syscall
addi $s1, $v0, 0      #   ��$v0��s��$s1

#   �L�X input c:
li $v0, 4            
la $a0, str3
syscall 

#   INPUT   c 
li $v0, 5                
syscall
addi $s2, $v0, 0      #   ��$v0��s��$s2

#���Fa, b, c, d,  �}�l�p��

#���⥭��
jal square
#��power����
jal power
#��e���⪺�F��ۥ[
jal ad

 #   �L�Xresult =
li $v0, 4
la $a0, str4
syscall 

#   �L�X d ($s3) ����
li $v0, 1             
addi $a0, $s3, 0      
syscall

#   �������X�{��
j Exit                


#  �Ƶ{���p�⥭��
square:
#  �Nb*b��i$t0
mul $t0, $s1, $s1
#  ���^�h
jr $ra



#  �Ƶ{���p��power����
power:
#  �@�i�ӥ��ˬdc �O���O�t��
slti $t3, $s2, 0 
#  �O�t�Ƹ��hL2
bne $t3, $zero, L2 

#  ���O���ܡA���U�h����
#  ����ra�Pc��
addi $sp, $sp, -8
sw $ra, 4($sp)
sw $s2, 0($sp)

#  �ˬdc ���S���p��1
slti $t1, $s2, 1 
#  c>1�A���hL1
beq $t1, $zero, L1 

#  ����t2�]1
addi $t2, $zero, 1 
#  ��_sp
addi $sp, $sp, 8  
#  ���^ra
jr $ra 



#  �Ƶ{��L1�AC�j���1�i�J
L1:
#  ��c-1��Jc 
addi $s2, $s2, -1 
#  ���^�Ƶ{��power
jal power

#  �^�_�쥻c�PRA��
lw $s2, 0($sp)
lw $ra, 4($sp)
addi $sp, $sp, 8

#�⵪�׭�a
mul $t2, $t2, $s0
#  ���^�h�쥻����}
jr $ra



#  �Ƶ{���ۥ[
ad:
add $s3, $t2, $t0
jr $ra

#  �Yc�O�t�ƫhpower����0
L2:
add $t2, $zero, $zero
jr $ra

Exit:
