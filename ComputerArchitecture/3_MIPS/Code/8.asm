# creating test
# creating test
ori $5 73893736
		
xor $1 $1 $1 # n = 0
addi $1 $1 0x0001 # n = 1
xor $15 $15 $15 # result = 0
xor $3 $3 $3
ori $3 $3 10		# $3 = 10

# num = $5
j start

loop:
sll $1 $1 4
start:
beq $5 $0 end		# see if we have any num left
divu $5 $3		# num / 10
mfhi $6			# $6 = num % 10
mflo $5			# $5 = num / 10
multu $6 $1		# $7 = shifted (num % 10) 
mflo $7
addu $15 $15 $7
j loop

end: