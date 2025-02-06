xor $22 $22 $22
lui $22 0x1234 
ori $22 $22 0x5678		# creating test a = 0x12345678

xor $30 $30 $30			# b = 0
xor $1 $1 $1			# one = 0 (hahaha)

xor $2 $2 $2			# i = 0
ori $2 $2 32			# i = 32

loop:
beq $2 $0 end			# if i = 0: end
sll $30	$30 1			# shift left b
andi $1 $22 0x0001		# n = a | 0x0001
srl $22	$22 1			# shift right a
or $30 $30 $1			# b = b | n
subiu $2 $2 1			# i--
j loop

end: