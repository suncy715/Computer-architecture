###########################
#  �Ǹ�:   102062801                  #
#  �m�W:   �]�ԬR                        #
# CS4100 - �p������c HW#2 #
###########################
.data

str1:
.asciiz "Please enter a int: "
str2:
.asciiz "$t0 is "
str3:
.asciiz "***"
Pnewline: 
.asciiz "\n"
.text


#######################   �{���}�l
li $v0, 4             #   �L�X Please enter an integer= 
la $a0, str1
syscall 


li $v0, 5             #   INPUT          
syscall
addi $t0, $v0, 0      #   ��$v0��s��$t0


                      #    if ($t0 <= 0 || $t0 > 10) 
slt $t1, $zero, $t0   #   ��� $t0���S���j�� 0
beq $t1, $zero, L1    #   �S�����ܸ��� L1 
slti $t1, $t0, 11     #   ��� $t0���S���p�� 11
beq $t1,$zero, L1     #   �S�����ܤ@�˸��� L1
j L2                  #   �H�W���S���߸��h L2
#######################

#######################    L1: �N��p��0�P�j��10�n�����Ʊ�: �L�X$0 = �h��?
L1: 

li $v0, 4             #   �L�X $t0 is �r��
la $a0, str2
syscall 

li $v0, 1             #   �L�X $t0 ����
addi $a0, $t0, 0      #   ��$t0��J$a0�A�L�X
syscall

j Exit                #   �������X�{��
#######################

#######################   L2: �N�� 1 - 10�n�����Ʊ�: �L�X***$t0***
L2:
add $t3, $zero, $zero #   setup i = $t3 = 0

Loop:                 #   �]�w�@�� Loop �^��

slt $t2, $t3, $t0     #   �ˬd��J�� $t0 �� i = $t3
beq $zero, $t2, Exit  #   �p�G�� i �P $t0 �@�ˤj�ɭԡA���XLoop�õ����{��


li $v0, 4             #   �L�X***
la $a0, str3
syscall 

li $v0, 1             #   �L�X***�᭱���Ʀr
addi $a0, $t3, 0 
syscall

li $v0, 4             #   �L�X��b�q��***
la $a0, str3
syscall 

la $a0, Pnewline      #    �ݭn�@�Ӵ���
li $v0, 4	
syscall	

addi $t3, $t3, 1      #   �����H�W�ʧ@ i ++

j Loop                #   i ++ ��A�٭n�~�� Loop
#######################

#######################  �{������
Exit:
