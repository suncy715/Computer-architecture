###########################
#  學號:   102062801                  #
#  姓名:   孫勤昱                        #
# CS4100 - 計算機結構 HW#2 #
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


#######################   程式開始
li $v0, 4             #   印出 Please enter an integer= 
la $a0, str1
syscall 


li $v0, 5             #   INPUT          
syscall
addi $t0, $v0, 0      #   把$v0轉存到$t0


                      #    if ($t0 <= 0 || $t0 > 10) 
slt $t1, $zero, $t0   #   比較 $t0有沒有大於 0
beq $t1, $zero, L1    #   沒有的話跳到 L1 
slti $t1, $t0, 11     #   比較 $t0有沒有小於 11
beq $t1,$zero, L1     #   沒有的話一樣跳到 L1
j L2                  #   以上都沒成立跳去 L2
#######################

#######################    L1: 代表小於0與大於10要做的事情: 印出$0 = 多少?
L1: 

li $v0, 4             #   印出 $t0 is 字串
la $a0, str2
syscall 

li $v0, 1             #   印出 $t0 的值
addi $a0, $t0, 0      #   把$t0放入$a0，印出
syscall

j Exit                #   完成跳出程式
#######################

#######################   L2: 代表 1 - 10要做的事情: 印出***$t0***
L2:
add $t3, $zero, $zero #   setup i = $t3 = 0

Loop:                 #   設定一個 Loop 回來

slt $t2, $t3, $t0     #   檢查輸入的 $t0 跟 i = $t3
beq $zero, $t2, Exit  #   如果當 i 與 $t0 一樣大時候，跳出Loop並結束程式


li $v0, 4             #   印出***
la $a0, str3
syscall 

li $v0, 1             #   印出***後面的數字
addi $a0, $t3, 0 
syscall

li $v0, 4             #   印出後半段的***
la $a0, str3
syscall 

la $a0, Pnewline      #    需要一個換行
li $v0, 4	
syscall	

addi $t3, $t3, 1      #   完成以上動作 i ++

j Loop                #   i ++ 後，還要繼續 Loop
#######################

#######################  程式結束
Exit:
