# $1 and $2 contain adresses 0x10010000 and 0x10010004
# $3 and $4 contain float1 and float2


ori $1, $0, 0
ori $2, $0, 0
lui $1, 0x1001
ori $1, $1, 0x0000
lui $2, 0x1001
ori $2, $2, 0x0004
lw  $3, 0($1)
lw  $4, 0($2)			# loading float 1 and 2

ori $9, $0, 0
lui $9, 0x8000	 		# $9  = 0x10000000
nor $10, $9, $0  		# $10 = 0x01111111
and $7, $3, $9
and $8, $4, $9			
srl $7, $7, 31
srl $8, $8, 31

# $7 and $8 contain sgn(float1) and sgn(float2)
slt $11, $8, $7			# sgn(float1) > sgn(float2) -> float2 > 0 , float1 < 0 -> $11 = 1
bne $11, $0, swap		# $11 = 1 -> swap
slt $11, $7, $8			# sgn(float2) > sgn(float1) -> float1 > 0, floa2 < 1 -> $11 = 1 
bne $11, $0, end

# if we get here it means the numbers have the same sign
and $5, $3, $10
and $6, $4, $10			# $5 and $6 contan  abs(float1) and abs(float2)
slt $11, $4, $3			# if $3 > $4 -> abs(f1) > abs(f2) -> $11 = 1
xor $11, $11, $7		# 0 means swap 1 means end
sll $11, $11, 31
bne $11, $0, end


swap:
or $9, $3, $0
or $3, $4, $0
or $4, $9, $0

end:
sw $3, 0($1)
sw $4, 0($2)