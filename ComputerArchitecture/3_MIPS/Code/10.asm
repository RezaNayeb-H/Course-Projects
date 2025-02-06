xor $14 $14 $14
lui $14 0xdead 
ori $14 $14 0xbeef		# creating test a = 0xdeadbeef

xor $24 $24 $24			# b = 0
xor $1 $1 $1		# one = 0 (hahaha)

loop:

beq $14 $0 end			# if a = 0: end
andi $1 $14 0x0001		# n = a | 0x0001
srl $14	$14 1			# shift right a
addu $24 $24 $1			# b = b + n
j loop

end: