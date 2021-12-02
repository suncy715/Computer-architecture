#############################
#  學號:   102062801                      #
#  姓名:   孫勤昱                            #
# CS4100 - 計算機結構 HW#3-2  #
#############################
.data

str1:
.asciiz "input n: "
str2:
.asciiz "fibird: "

.text



#######################   程式開始
Main: 

#n = 0;
add $s0, $zero, $zero
#備用一個$s1暫存器內存2 (用在乘法)
addi $s1, $s1, 2

#   印出 input n:
li $v0, 4             
la $a0, str1
syscall 

#   INPUT   n  
li $v0, 5 
syscall
addi $s0, $v0, 0      #   把$v0轉存到$s0

#先檢查輸入有沒有小於等於0，有的話跳去L2
slti $t0, $s0, 1
bne $t0, $zero, L2

#輸入正常進入recur副程式
jal recur

#   印出 fibird:
li $v0, 4
la $a0, str2
syscall 

#   印出 s2
li $v0, 1             
addi $a0, $s2, 0 
syscall

#  之後結束
j exit



#  Recur副程式開始
recur:

#  紀錄ra與n的值
addi $sp, $sp, -8
sw $ra, 4($sp)
sw $s0, 0($sp)

#  檢查n有沒有小於2 (等於1)
slti $t0, $s0, 2

#  沒有的話跳去L1
beq $t0, $zero, L1 #大於的時候跳去L1

#  有的話接續著算，把1放入s2
addi $s2, $zero, 1

#  恢復stack
addi $sp, $sp, 8

#  跳回剛剛跳過來的下一個指令位址
jr $ra



#  L1副程式開始
L1:

#  把n-1
addi $s0, $s0, -1
#  跳回recur副程式繼續執行
jal recur

#  恢復ra與n值
lw $s0, 0($sp)
lw $ra, 4($sp)
#  恢復stack
addi $sp, $sp, 8

#  先做2*recur (n-1) 
mul $s2, $s2, $s1
#  再做+n
add $s2, $s2, $s0

#  跳回剛剛跳過來的下一個指令位址
jr $ra



#  副程式L2
L2:
#  直接把0放入s2
add $s2, $zero, $zero

#   印出 fibird:
li $v0, 4           
la $a0, str2
syscall 

#   印出 s2
li $v0, 1             
addi $a0, $s2, 0      
syscall

#  之後結束
j exit


#  程式結束
exit: