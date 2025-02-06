# creating test
lui $5 0x1842
ori $5 0x3382

xor $1 $1 $1 # n = 0
addi $1 $1 0x0001 # n = 1
xor $15 $15 $15 # result = 0
xor $3 $3 $3
ori $3 $3 10		# $3 = 10
# bcd[i] = $5
j start

loop:
multu $1 $3		# n = n * 10
mflo $1			# load it again in $1
srl $5 $5 4		# bcd = bcd >> 1
start:
beq $5 $0 end		# see if we have any bcd left	
andi $6 $5 0x000F	# find bcd[i]
multu $1 $6		# create n * bcd[i]
mflo $7			# store n * bcd[i] in $7
addu $15 $15 $7		# result = result + n * bcd[i]
j loop

end: