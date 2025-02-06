lui $t0 0x1001
ori $t0 0x0000

lui $t1 0x1001
ori $t1 0x0004

lui $t2 0x1001
ori $t2 0x0008

lui $t3 0x1001
ori $t3 0x000C

lw $t0 0($t0)
lw $t1 0($t1)
lw $t2 0($t2)
lw $t3 0($t3)

subu $t4,$t0,$t2
sub $t5,$t1,$t3
sltu $t6,$t0,$t4
sub $t5,$t5,$t6