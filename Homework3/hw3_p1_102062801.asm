#############################
#  學號:   102062801                      #
#  姓名:   孫勤昱                            #
# CS4100 - 計算機結構 HW#3-1  #
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
#############################   程式開始
Main: 

#initial
add $s0, $zero, $zero # a = 0
add $s1, $zero, $zero # b = 0
add $s2, $zero, $zero # c = 0
add $s3, $zero, $zero # d = 0

#   印出 input a:
li $v0, 4
la $a0, str1
syscall

#   INPUT   a    
li $v0, 5    
syscall
addi $s0, $v0, 0      #   把$v0轉存到$s0

#   印出 input b:
li $v0, 4             
la $a0, str2
syscall 

#   INPUT   b   
li $v0, 5                
syscall
addi $s1, $v0, 0      #   把$v0轉存到$s1

#   印出 input c:
li $v0, 4            
la $a0, str3
syscall 

#   INPUT   c 
li $v0, 5                
syscall
addi $s2, $v0, 0      #   把$v0轉存到$s2

#有了a, b, c, d,  開始計算

#先算平方
jal square
#算power次方
jal power
#把前面算的東西相加
jal ad

 #   印出result =
li $v0, 4
la $a0, str4
syscall 

#   印出 d ($s3) 的值
li $v0, 1             
addi $a0, $s3, 0      
syscall

#   完成跳出程式
j Exit                


#  副程式計算平方
square:
#  將b*b放進$t0
mul $t0, $s1, $s1
#  跳回去
jr $ra



#  副程式計算power次方
power:
#  一進來先檢查c 是不是負數
slti $t3, $s2, 0 
#  是負數跳去L2
bne $t3, $zero, L2 

#  不是的話，接下去執行
#  紀錄ra與c值
addi $sp, $sp, -8
sw $ra, 4($sp)
sw $s2, 0($sp)

#  檢查c 有沒有小於1
slti $t1, $s2, 1 
#  c>1，跳去L1
beq $t1, $zero, L1 

#  先把t2設1
addi $t2, $zero, 1 
#  恢復sp
addi $sp, $sp, 8  
#  跳回ra
jr $ra 



#  副程式L1，C大於於1進入
L1:
#  把c-1放入c 
addi $s2, $s2, -1 
#  跳回副程式power
jal power

#  回復原本c與RA值
lw $s2, 0($sp)
lw $ra, 4($sp)
addi $sp, $sp, 8

#把答案乘a
mul $t2, $t2, $s0
#  跳回去原本的位址
jr $ra



#  副程式相加
ad:
add $s3, $t2, $t0
jr $ra

#  若c是負數則power等於0
L2:
add $t2, $zero, $zero
jr $ra

Exit:
