//Code_2_4
/*********************************************************************
* Description:
* This project demonstrates how to program the PIC32 microcontroller so as to show the state of a switch (attached to the RB0 pin) on an LED (attached to the RB8 pin), in real time.
********************************************************************/
#include <xc.h>
#include "configbits.c"
.global main 
.data
    ss_0:
    .word 0x003f
    ss_1:
    .word 0x0006
    ss_2:
    .word 0x005b
    ss_3:
    .word 0x004f
    ss_4:
    .word 0x0066
    ss_5:
    .word 0x006d
    ss_6:
    .word 0x007d
    ss_7:
    .word 0x0007
    ss_8:
    .word 0x007f
    ss_9:
    .word 0x006f
.text
.ent main
main:
la $8, ANSELB
sw $0, 0($8)


la $8, TRISB
sw $0, 0($8)

la $11, LATB

la $8, ss_0

    
ori $17, $0, 0x0200
ori $20, $0, 0x0100
ori $15, $0, 60
ori $23, $0, 10


j loop
logic:
or   $19, $0, $0

addi $14, $14, 1 // $14 -> total time in seconds
divu $0, $14, $15
mflo $3 // total minutes
mfhi $5 // total seconds
divu $0, $5, $23
mfhi $5 // seconds
mflo $2 // 10-seconds
divu $0, $3, $23
mfhi $3 // minute
mflo $4 // 10-minute

or   $12, $0, $4
sll  $3, $3, 4
or   $12, $12, $3
sll  $2, $2, 8
or   $12, $12, $2
sll  $5, $5, 12
or   $12, $12, $5

loop:
    
    
    ori $10, $0, 0x0000
    add $10, $10, $8
    lw $10, 0($10)
    ori $10, $10, 0x0f00
    sw $10, 0($11)
    
    or $18, $0, $0
    digit_1000:
	andi $16, $12, 0x000f
	sll  $16, $16, 2
	add  $10, $16, $8 
	lw   $10, 0($10)
	ori  $10, 0x0800
	sw   $10, 0($11)
	addi $18, $18, 1
	bne  $17, $18, digit_1000
    
    or $18, $0, $0
    digit_0100:
	srl  $13, $12, 4
	andi $16, $13, 0x000f
	sll  $16, $16, 2
	add  $10, $16, $8 
	lw   $10, 0($10)
	or   $10, $10, 0x0080
	ori  $10, 0x0400
	sw   $10, 0($11)
	addi $18, $18, 1
	bne  $17, $18, digit_0100
	
    or $18, $0, $0
    digit_0010:
	srl  $13, $12, 8
	andi $16, $13, 0x000f
	sll  $16, $16, 2
	add  $10, $16, $8 
	lw   $10, 0($10)
	ori  $10, 0x0200
	sw   $10, 0($11)
	addi $18, $18, 1
	bne  $17, $18, digit_0010
	
    or $18, $0, $0
    digit_0001:
	srl  $13, $12, 12
	andi $16, $13, 0x000f
	sll  $16, $16, 2
	add  $10, $16, $8 
	lw   $10, 0($10)
	ori  $10, 0x0100
	sw   $10, 0($11)
	addi $18, $18, 1
	bne  $17, $18, digit_0001
	
    addi  $19, $19, 1  
    beq   $19, $20, logic
    j loop
.end main