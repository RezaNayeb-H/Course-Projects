//===========================================================//
//
//			Name & Student ID Reza Nayeb Habib 401102694
//
//			Implemented Instructions are:
//			R format:  add(u), sub(u), and, or, xor, nor, slt, sltu;
//			I format:  beq, bne, lw, sw, addi(u), slti, sltiu, andi, ori, xori, lui.
//
//===========================================================//

`timescale 1ns/1ns

   // codes for ALU functions
   `define ADD  4'h0
   `define SUB  4'h1
   `define SLT  4'h2
   `define SLTU 4'h3
   `define AND  4'h4
   `define OR   4'h5
   `define NOR  4'h6
   `define XOR  4'h7
   `define LUI  4'h8

   // op section codes
   `define _Rfrmt 6'b000000
   `define _addi  6'b001000
   `define _addiu 6'b001001
   `define _slti  6'b001010
   `define _sltiu 6'b001011
   `define _andi  6'b001100
   `define _ori   6'b001101
   `define _xori  6'b001110
   `define _lui   6'b001111
   `define _beq   6'b000100
   `define _bne   6'b000101
   `define _lw    6'b100011
   `define _sw    6'b101011

   // funct section codes
   `define _add   6'b100000
   `define _sub   6'b100010
   `define _addu  6'b100001
   `define _subu  6'b100011
   `define _and   6'b100100
   `define _or    6'b100101
   `define _xor   6'b100110
   `define _nor   6'b100111
   `define _slt   6'b101010
   `define _sltu  6'b101011


module single_cycle_mips 
(
	input clk,
	input reset
);
 
	initial begin
		$display("Single Cycle MIPS Implemention");
		$display("by Reza Nayeb Habib 401102694");
	end

	reg [31:0] PC;          // Keep PC as it is, its name is used in higher level test bench

   wire [31:0] instr;
   wire [ 5:0] op   = instr[31:26];
   wire [ 5:0] func = instr[ 5: 0];
   wire [15:0] imm  = instr[15: 0];
   wire [ 4:0] rd   = instr[15:11];
   wire [ 4:0] rt   = instr[20:16];
   wire [ 4:0] rs   = instr[25:21];


   wire [31:0] RD1, RD2, AluResult, MemReadData;

   wire AluZero;

   // Control Signals
   reg Branch;
   reg BranchCondition;
   ///////////////
   wire PCSrc = Branch & BranchCondition;
   reg SZEn, ALUSrc, RegDst, MemtoReg, RegWrite, MemWrite;


   reg [3:0] AluOP;

	
	// CONTROLLER COMES HERE

   always @(*) begin
      SZEn = 1'bx;
      AluOP = 4'hx;
      ALUSrc = 1'b1;          // it is defaulted to immediate and it will be changed in r formats and branches
      RegDst = 1'b0;          // it is defaulted to 1'b0 so it writes in reg adressed by rt
      MemtoReg = 1'bx;
      RegWrite = 1'b0;
      MemWrite = 1'b0;
      Branch = 1'b0;          // again defaulted to 0
      BranchCondition = 1'bx;

      case(op)
         `_Rfrmt:
         begin
            RegDst = 1'b1;    // write in reg adressed by rd
            ALUSrc = 1'b0;    // ALU source is register addressed by rt
            RegWrite = 1'b1;
            MemtoReg = 1'b0;
            case(func)
               `_add:
               begin
                  AluOP = `ADD;
               end
               `_sub:
               begin
                  AluOP = `SUB;
               end
               `_addu:
               begin
                  AluOP = `ADD;
               end
               `_subu:
               begin
                  AluOP = `SUB;
               end
               `_and:
               begin
                  AluOP = `AND;
               end
               `_or:
               begin
                  AluOP = `OR;
               end
               `_xor:
               begin
                  AluOP = `XOR;
               end
               `_nor:
               begin
                  AluOP = `NOR;
               end
               `_slt:
               begin
                  AluOP = `SLT;
               end
               `_sltu:
               begin
                  AluOP = `SLTU;
               end
            endcase
         end


         `_addi:
         begin
            RegWrite = 1'b1;
            AluOP = `ADD;
            MemtoReg = 1'b0;
            SZEn = 1'b1;   // sign extend
         end
         `_addiu:
         begin
            RegWrite = 1'b1;
            AluOP = `ADD;
            MemtoReg = 1'b0;
            SZEn = 1'b1;
         end
         `_slti:
         begin
            RegWrite = 1'b1;
            AluOP = `SLT;
            MemtoReg = 1'b0;
            SZEn = 1'b1;
         end
         `_sltiu:
         begin
            RegWrite = 1'b1;
            AluOP = `SLTU;
            MemtoReg = 1'b0;
            SZEn = 1'b0;   // zero extend
         end
         `_andi:
         begin
            RegWrite = 1'b1;
            AluOP = `AND;
            MemtoReg = 1'b0;
            SZEn = 1'b0;   // zero extend
         end
         `_ori:
         begin
            RegWrite = 1'b1;
            AluOP = `OR;
            MemtoReg = 1'b0;
            SZEn = 1'b0;   // zero extend
         end
         `_xori:
         begin
            RegWrite = 1'b1;
            AluOP = `XOR;
            MemtoReg = 1'b0;
            SZEn = 1'b0;   // zero extend
         end
         `_lui:
         begin
            RegWrite = 1'b1;
            AluOP = `LUI;
            MemtoReg = 1'b0;
            //SZEn = 1'bx; makes no difference!
         end
         `_beq:
         begin
            Branch = 1'b1;
            ALUSrc = 1'b0;
            RegWrite = 1'b0;
            AluOP = `XOR;
            MemtoReg = 1'b0;
            BranchCondition = AluZero;
            SZEn = 1'b1;   // sign extend
         end
         `_bne:
         begin
            Branch = 1'b1;
            ALUSrc = 1'b0;
            RegWrite = 1'b0;
            AluOP = `XOR;
            MemtoReg = 1'b0;
            BranchCondition = ~AluZero;
            SZEn = 1'b1;   // sign extend 
         end
         `_lw:
         begin
            RegWrite = 1'b1;
            AluOP = `ADD;
            MemtoReg = 1'b1;
            SZEn = 1'b1;   // sign extend imm
         end
         `_sw:
         begin
            RegWrite = 1'b0;
            AluOP = `ADD;
            MemWrite = 1'b1;
            MemtoReg = 1'b0;
            SZEn = 1'b1;   // sign extend imm
         end


      endcase
      
//      YOU COMPLETE THE REST


   end


	// DATA PATH STARTS HERE

   wire [31:0] Imm32 = SZEn ? {{16{instr[15]}},instr[15:0]} : {16'h0, instr[15:0]};     // ZSEn: 1 sign extend, 0 zero extend

   wire [31:0] PCplus4 = PC + 4'h4;

   wire [31:0] PCbranch = PCplus4 + (Imm32 << 2);

   always @(posedge clk)
      if(reset)
         PC <= 32'h0;
      else
         PC <= PCSrc ? PCbranch : PCplus4;


//==========================================================//
//	instantiated modules
//==========================================================//

// Register File

   reg_file rf
   (
      .clk   ( clk ),
      .write ( RegWrite ),
      .WR    ( RegDst   ? instr[15:11] : instr[20:16]),
      .WD    ( MemtoReg ? MemReadData  : AluResult),
      .RR1   ( instr[25:21] ),
      .RR2   ( instr[20:16] ),
      .RD1   ( RD1 ),
      .RD2   ( RD2 )
	);

   my_alu alu
   (
      .Op( AluOP ),
      .A ( RD1 ),
      .B ( ALUSrc ? Imm32 : RD2),
      .X ( AluResult ),
      .Z ( AluZero )
   );
   


//	Instruction Memory
	async_mem imem			// keep the exact instance name
	(
		.clk		   (1'b0),
		.write		(1'b0),		// no write for instruction memory
		.address	   ( PC ),		   // address instruction memory with pc
		.write_data	(32'bx),
		.read_data	( instr )
	);
	
// Data Memory
	async_mem dmem			// keep the exact instance name
	(
		.clk		   ( clk ),
		.write		( MemWrite ),
		.address	   ( AluResult ),
		.write_data	( RD2 ),
		.read_data	( MemReadData )
	);

endmodule


//==============================================================================//

module my_alu(
   input  [3:0] Op,
   input  [31:0] A,
   input  [31:0] B,
   output [31:0] X,
   output        Z
   );

   wire sub = Op != `ADD;
   wire [31:0] bb = sub ? ~B : B;
   wire [32:0] sum = A + bb + sub;
   wire sltu = ! sum[32];

   wire v = sub ?
            ( A[31] != B[31] && A[31] != sum[31] )
          : ( A[31] == B[31] && A[31] != sum[31] );

   wire slt = v ^ sum[31];

   reg [31:0] x;

   always @( * )
      case( Op )
         `ADD : x = sum;
         `SUB : x = sum;
         `SLT : x = slt;
         `SLTU: x = sltu;
         `AND : x = A & B;
         `OR  : x = A | B;
         `NOR : x = ~(A | B);
         `XOR : x = A ^ B;
         `LUI : x = {B[15:0], 16'h0};
         default : x = 32'hxxxxxxxx;
      endcase

   assign X = x;
   assign Z = x == 32'h00000000;

endmodule

//============================================================================//
