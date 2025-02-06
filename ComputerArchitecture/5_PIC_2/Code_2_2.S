//Code_2_2
/*********************************************************************
* Description:
* This project demonstrates how to program the PIC32 microcontroller so as to show the state of a switch (attached to the RB0 pin) on an LED (attached to the RB8 pin), in real time.
********************************************************************/
#include <xc.h>
#include "configbits.c"
.global main 
.data
    ss____0:
    .word 0x013f
    ss___0_:
    .word 0x023f
    ss__0__:
    .word 0x043f
    ss_0___:
    .word 0x083f
    ss____1:
    .word 0x0106
    ss___1_:
    .word 0x0206
    ss__1__:
    .word 0x0406
    ss_1___:
    .word 0x0806
    ss____2:
    .word 0x015b
    ss___2_:
    .word 0x025b
    ss__2__:
    .word 0x045b
    ss_2___:
    .word 0x085b
    ss____3:
    .word 0x014f
    ss___3_:
    .word 0x024f
    ss__3__:
    .word 0x044f
    ss_3___:
    .word 0x084f
    ss____4:
    .word 0x0166
    ss___4_:
    .word 0x0266
    ss__4__:
    .word 0x0466
    ss_4___:
    .word 0x0866
    ss____5:
    .word 0x016d
    ss___5_:
    .word 0x026d
    ss__5__:
    .word 0x046d
    ss_5___:
    .word 0x086d
    ss____6:
    .word 0x017d
    ss___6_:
    .word 0x027d
    ss__6__:
    .word 0x047d
    ss_6___:
    .word 0x087d
    ss____7:
    .word 0x0107
    ss___7_:
    .word 0x0207
    ss__7__:
    .word 0x0407
    ss_7___:
    .word 0x0807
    ss____8:
    .word 0x017f
    ss___8_:
    .word 0x027f
    ss__8__:
    .word 0x047f
    ss_8___:
    .word 0x087f
    ss____9:
    .word 0x016f
    ss___9_:
    .word 0x026f
    ss__9__:
    .word 0x046f
    ss_9___:
    .word 0x086f
.text
.ent main
main:
la $8, ANSELB
sw $0, 0($8)


la $8, TRISB
sw $0, 0($8)

la $11, LATB

ori $10, $10, 0x0fff
    
loop:
    la $8, ss_4___
    lw $8, 0($8)
    sw $8, 0($11)
    
    and $9, $0, $0
    wait_1:
    addi $9, $9, 0x0001
    bne $9, $10, wait_1
    
    la $8, ss__1__
    lw $8, 0($8)
    sw $8, 0($11)
    
    and $9, $0, $0
    wait_2:
    addi $9, $9, 0x0001
    bne $9, $10, wait_2
    
    la $8, ss___5_
    lw $8, 0($8)
    sw $8, 0($11)
    
    and $9, $0, $0
    wait_3:
    addi $9, $9, 0x0001
    bne $9, $10, wait_3
    
    la $8, ss____9
    lw $8, 0($8)
    sw $8, 0($11) 
    
    and $9, $0, $0
    wait_4:
    addi $9, $9, 0x0001
    bne $9, $10, wait_4
    
    j loop
.end main