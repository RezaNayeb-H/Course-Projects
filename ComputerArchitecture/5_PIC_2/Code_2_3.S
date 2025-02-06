//Code_2_3
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

lui $12, 0x9620
ori $12, $12, 0x1104
lui $27, 0x4000
ori $27, 0x0000
    
ori $17, $17, 0x0400
ori $20, $0, 0x0100



j loop
shift:
or   $19, $0, $0

srl  $25, $12, 4
or   $25, $25, $27
sll  $26, $12, 28
or   $27, $26, $0
or   $12, $25, $0
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
    beq   $19, $20, shift
    j loop
.end main