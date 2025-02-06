ori $8 19		# num = 10 (test)
addiu $15 $8 1		# for loop check

xor $1 $1 $1
lui $1 0x1001		# $1 - base (base adress)

xor $2 $2 $2		# $2 = p = number of primes found so far
# $3 = n = remainder
xor $3 $3 $3
ori $3 $3 0x0002	# i = 2

xor $4 $4 $4
ori $4 $4 0x0004	#  4

loop1:
beq $3 $15 ending		# i == num + 1: ending
xor $6 $6 $6		# j = 0
	loop2:
	multu $4 $6
	mflo  $7		# $7 = 4j
	beq   $7 $2  cont	# p = 4j: continue
	addu $5 $7 $1		# $5 = base adress + 4j = primes[j]
	lw $9 0($5)		# $9 = primes[j]
	
	divu $3 $9		# i / primes[j]
	mfhi $10		# n = i % primes[j]
	beq $10 $0 skip		# if(n == 0): skip
	addiu $6 $6 1		# j++
	j loop2
cont:
addu $5 $2 $1		# $5 = base adress + p = primes[p]
sw $3 0($5)		# store in primes[p]
addiu $2 $2 0x0004	# p++
skip:
addiu $3 $3 1		# i++
j loop1

ending:
addu $5 $2 $1		# $5 = base adress + p = primes[p]
lw $3 -4($5)		# $3 = primes[p]
xor $9 $9 $9		# result = 0
beq $3 $8 prime		# primes[9] == num : num was prime
j end

prime:
ori $9 $9 0x0001
end:
