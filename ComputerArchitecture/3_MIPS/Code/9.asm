xor $8 $8 $8 
ori $8 $8 46 		# creating test n

xor $18 $18 $18
ori $18 $18 0x0001	# a = 1

xor $17 $17 $17
ori $17 $17 0x0001	# b = 1
			# $1 is temp
loop:
beq $8 $0 end
or $1 $17 $0		# temp = b
addu $17 $18 $17	# b = b + a
or $18 $1 $0		# a = t
subu $8 $8 1
j loop
 
end:

