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


multu $t0 $t2
mflo  $t4
mfhi  $t5

multu $t1 $t3
mflo  $t6
mfhi  $t7

multu $t1 $t2
mflo  $t8
addu $t5,$t8,$t5
sltu $t9,$t5,$t8
addu $t6,$t6,$t9
sltu $t9,$t6,$t9
addu $t7,$t7,$t9

multu $t3 $t0
mflo  $t8
addu $t5,$t8,$t5
sltu $t9,$t5,$t8
addu $t6,$t6,$t9
sltu $t9,$t6,$t9
addu $t7,$t7,$t9
