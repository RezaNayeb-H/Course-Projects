
`timescale 1ns/100ps

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
   `define _jump  6'b000010
   `define _jal  6'b000011

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
   `define _mflo  6'b010010
   `define _mfhi  6'b010000

module multi_cycle_mips(

   input clk,
   input reset,

   // Memory Ports
   output  [31:0] mem_addr,
   input   [31:0] mem_read_data,
   output  [31:0] mem_write_data,
   output         mem_read,
   output         mem_write
);

   // Data Path Registers
   reg MRE, MWE;
   reg [31:0] A, B, PC, IR, MDR, MAR;
   // DEBUG
   wire [5:0] op = IR[31:26];
   wire [5:0] funct = IR[5:0];
   wire [4:0] rs = IR[25:21];
   wire [4:0] rt = IR[20:16];
   wire [4:0] rd = IR[15:11];
   wire [15:0] imm = IR[15:0];
   // DEBUG

   // multiplier wires and regs
   reg startMult;
   wire [63:0] Product;
   wire multReady;
   reg mfhi, mflo;

   // Data Path Control Lines, don't forget, regs are not always register !!
   reg setMRE, clrMRE, setMWE, clrMWE;
   reg Awrt, Bwrt, RFwrt, PCwrt, IRwrt, MDRwrt, MARwrt;
   reg PCJump, PCJR;
   reg link;

   // Memory Ports Bindings
   assign mem_addr = MAR;
   assign mem_read = MRE;
   assign mem_write = MWE;
   assign mem_write_data = B;

   // Mux & ALU Control Line
   reg [3:0] aluOp;
   reg [1:0] aluSelB;
   reg SgnExt, aluSelA, MemtoReg, RegDst, IorD;
   

   // Wiring
   wire aluZero;
   wire [31:0] aluResult, rfRD1, rfRD2;

   // branching condition
   wire isBeq = (IR[31:26] == `_beq) ?  1'b1 : (IR[31:26] == `_bne) ? 1'b0 : 1'bx;
   // Clocked Registers
   always @( posedge clk ) begin
      if( reset )
         PC <= #0.1 32'h00000000;
      else if( PCwrt )
         PC <= #0.1 aluResult;
      else if( PCJump )
         PC <= #0.1 {PC[31:28], IR[25:0], 2'b00};
      else if( PCJR )
         PC <= #0.1 A;

      if( Awrt ) A <= #0.1 rfRD1;
      if( Bwrt ) B <= #0.1 rfRD2;

      if( MARwrt ) MAR <= #0.1 PCJump ? {PC[31:28], IR[25:0], 2'b00}  : PCJR ? A : IorD ? aluResult : PC;

      if( IRwrt ) IR <= #0.1 mem_read_data;
      if( MDRwrt ) MDR <= #0.1 mem_read_data;

      if( reset | clrMRE ) MRE <= #0.1 1'b0;
          else if( setMRE) MRE <= #0.1 1'b1;

      if( reset | clrMWE ) MWE <= #0.1 1'b0;
          else if( setMWE) MWE <= #0.1 1'b1;

   end

   // Register File
   reg_file rf(
      .clk( clk ),
      .write( RFwrt | link ),

      .RR1( IR[25:21] ),
      .RR2( IR[20:16] ),
      .RD1( rfRD1 ),
      .RD2( rfRD2 ),

      .WR( link ? 5'd31 : RegDst ? IR[15:11] : IR[20:16] ),
      .WD( link ? PC : MemtoReg ? MDR : mfhi ? Product[63:32] : mflo ? Product[31:0] : aluResult )
   );

   // Sign/Zero Extension
   wire [31:0] SZout = SgnExt ? {{16{IR[15]}}, IR[15:0]} : {16'h0000, IR[15:0]};

   // ALU-A Mux
   wire [31:0] aluA = aluSelA ? A : PC;

   // ALU-B Mux
   reg [31:0] aluB;
   always @(*)
   case (aluSelB)
      2'b00: aluB = B;
      2'b01: aluB = 32'h4;
      2'b10: aluB = SZout;
      2'b11: aluB = SZout << 2;
   endcase

   my_alu alu(
      .A( aluA ),
      .B( aluB ),
      .Op( aluOp ),

      .X( aluResult ),
      .Z( aluZero )
   );


   my_multiplier multiplier(
      .clk( clk ),
      .start( startMult ),
      .A( A ),
      .B( B ),
      .Product( Product ),
      .ready( multReady )
    );


   // Controller Starts Here

   // Controller State Registers
   reg [4:0] state, nxt_state;

   // State Names & Numbers
   localparam
      RESET = 0, FETCH1 = 1, FETCH2 = 2, FETCH3 = 3, DECODE = 4,
      EX_ALU_R = 7, EX_ALU_I = 8, EX_JUMP = 9, EX_JR = 10,
      EX_LW_1 = 11, EX_LW_2 = 12, EX_LW_3 = 13, EX_LW_4 = 14, EX_LW_5 = 15,
      EX_SW_1 = 21, EX_SW_2 = 22, EX_SW_3 = 23,
      EX_BRA_1 = 25, EX_BRA_2 = 26, EX_MULTU = 27, WAIT_MULTU = 28, EX_MF = 29;

   // State Clocked Register 
   always @(posedge clk)
      if(reset)
         state <= #0.1 RESET;
      else
         state <= #0.1 nxt_state;

   task PrepareFetch;
      begin
         IorD = 0;
         setMRE = 1;
         MARwrt = 1;
         nxt_state = FETCH1;
      end
   endtask

   // State Machine Body Starts Here
   always @( * ) begin

      nxt_state = 'bx;

      SgnExt = 'bx; IorD = 'bx;
      MemtoReg = 'bx; RegDst = 'bx;
      aluSelA = 'bx; aluSelB = 'bx; aluOp = 'bx;

      PCwrt = 0;
      Awrt = 0; Bwrt = 0;
      RFwrt = 0; IRwrt = 0;
      MDRwrt = 0; MARwrt = 0;
      setMRE = 0; clrMRE = 0;
      setMWE = 0; clrMWE = 0;
      PCJump = 0; PCJR = 0;
      startMult = 0; mfhi = 0; mflo = 0;
      link = 0;

      case(state)

         RESET:
            PrepareFetch;

         FETCH1:
            nxt_state = FETCH2;

         FETCH2:
            nxt_state = FETCH3;

         FETCH3: begin
            IRwrt = 1;
            PCwrt = 1;
            clrMRE = 1;
            aluSelA = 0;     // A = PC
            aluSelB = 2'b01; // B = 4
            aluOp = `ADD;
            nxt_state = DECODE;
         end

         DECODE: begin
            Awrt = 1;
            Bwrt = 1;
            case( IR[31:26] )
               `_Rfrmt: begin             // R-format
                  case( IR[5:3] )
                     3'b000: ;
                     3'b001: begin 
                        nxt_state = EX_JR;
                        if(IR[2:0] == 3'b001) // jalr
                           link = 1;
                     end
                     3'b010: nxt_state = EX_MF;
                     3'b011: nxt_state = EX_MULTU; //multu
                     3'b101: nxt_state = EX_ALU_R;
                     3'b100: nxt_state = EX_ALU_R;
                     3'b110: ;
                     3'b111: ;
                  endcase
               end
               `_lui,
               `_addi,             // addi
               `_addiu,             // addiu
               `_slti,             // slti
               `_sltiu,             // sltiu
               `_andi,             // andi
               `_ori,             // ori
               `_xori:             // xori
                  nxt_state = EX_ALU_I;

               `_lw:
                  nxt_state = EX_LW_1;

               `_sw:
                  nxt_state = EX_SW_1;

               `_beq,
               `_bne:
                  nxt_state = EX_BRA_1;
               `_jump:
                  nxt_state = EX_JUMP;
               `_jal: begin
                  nxt_state = EX_JUMP;
                  link = 1;
               end
               default:
                  nxt_state = 31;
                  
                  

               // rest of instructiones should be decoded here

            endcase
         end

         EX_ALU_R: begin
            RFwrt = 1'b1;
            aluSelA = 1'b1;   // ALU input1 = A
            aluSelB = 2'b00;  // ALU input2 = B
            case( IR[5:0] )
               `_add : aluOp = `ADD;
               `_addu: aluOp = `ADD;
               `_sub : aluOp = `SUB;
               `_subu: aluOp = `SUB;
               `_and : aluOp = `AND;
               `_or  : aluOp = `OR;
               `_xor : aluOp = `XOR;
               `_nor : aluOp = `NOR;
               `_slt : aluOp = `SLT;
               `_sltu: aluOp = `SLTU;
            endcase
            RegDst = 1'b1;    // RegDst = IR[15:11]
            MemtoReg = 1'b0; 
            PrepareFetch;
         end

         EX_ALU_I: begin
            RFwrt = 1'b1;
            aluSelA = 1'b1;  // ALU input1 = A
            aluSelB = 2'b10; // ALU input2 = immediate
            case( IR[31:26] )
               `_addi,
               `_addiu:
               begin
                  aluOp = `ADD;
                  SgnExt = 1;
               end

               `_slti:
               begin
                  aluOp = `SLT;
                  SgnExt = 1;
               end

               `_sltiu:
               begin
                  aluOp = `SLTU;
                  SgnExt = 0;
               end

               `_andi:
               begin
                  aluOp = `AND;
                  SgnExt = 0;
               end

               `_ori:
               begin
                  aluOp = `OR;
                  SgnExt = 0;
               end

               `_xori:
               begin
                  aluOp = `XOR;
                  SgnExt = 0;
               end
               `_lui:
               begin
                  aluOp = `LUI;
                  //SZEn = 1'bx; makes no difference!
               end
               
            endcase
            RegDst = 1'b0;    // RegDst = IR[20:16]
            MemtoReg = 1'b0;
            PrepareFetch;
         end

         EX_LW_1: begin
            nxt_state = EX_LW_2;
            aluSelA = 1'b1;      // ALU input1 = A
            aluSelB = 2'b10;     // ALU input2 = immediate
            aluOp = `ADD;
            SgnExt = 1'b1;   
            IorD = 1'b1;         // set MAR from ALU out
            MARwrt = 1'b1;       // enable MAR write
            setMRE = 1'b1;       // enable reading from memory
         end

         EX_LW_2: begin
            nxt_state = EX_LW_3;    // 2.5ns has passed
         end

         EX_LW_3: begin
            nxt_state = EX_LW_4;    // 5ns has passed 
         end

         EX_LW_4: begin
            nxt_state = EX_LW_5;    // 7.5 ns has passed -> data is ready to read(memory read delay = 7ns)
            clrMRE = 1'b1;          // disable reading from memory(MAR won't be changed anymore)
            MDRwrt = 1'b1;          // read data from memory
         end

         EX_LW_5: begin
            RFwrt = 1'b1;
            MemtoReg = 1'b1;
            RegDst = 1'b0;          // destination = IR[20:16] aka rt
            PrepareFetch;
         end

         EX_SW_1: begin
            nxt_state = EX_SW_2;
            aluSelA = 1'b1;         // ALU input1 = A
            aluSelB = 2'b10;        // ALU input2 = immediate
            aluOp = `ADD;
            SgnExt = 1'b1;
            IorD = 1'b1;            // set MAR from ALU out
            MARwrt = 1'b1;          // enable MAR write
            setMWE = 1'b1;          // enable writing to memory
         end

         EX_SW_2: begin
            nxt_state = EX_SW_3;    // 2.5ns has passed -> data from ALU is ready and placed in MAR and B is written in Memory
            clrMWE = 1'b1;          // disable writing to memory to prevent damaging data
         end

         EX_SW_3: begin
            PrepareFetch;
         end

         EX_BRA_1: begin
            aluSelA = 1'b1;      // ALU input 1 = A
            aluSelB = 2'b00;     // ALU input 2 = B
            aluOp = `SUB;
            if(aluZero & isBeq)
               nxt_state = EX_BRA_2;
            else if(~aluZero & ~isBeq) begin
               nxt_state = EX_BRA_2;
            end else
               PrepareFetch;
         end

         EX_BRA_2: begin
            nxt_state = FETCH1;
            aluSelA = 1'b0;      // ALU input 1 = PC
            aluSelB = 2'b11;     // ALU input 2 = sgn(imm) << 2
            SgnExt = 1'b1;
            aluOp = `ADD;
            PCwrt = 1'b1;        // write in PC from ALU result
            MARwrt = 1'b1;       // enable writing in MAR
            IorD = 1'b1;         // set MAR from ALU result(set it same as pc)
            setMRE = 1'b1;       // enable reading from memory
         end

         EX_JUMP: begin
            nxt_state = FETCH1;
            PCJump = 1'b1;       // change pc according to jump instruction
            MARwrt = 1'b1;       // enable writing in MAR
            setMRE = 1'b1;       // enable reading from memory
         end

         EX_JR: begin
            nxt_state = FETCH1;
            PCJR = 1'b1;         // change pc according to jump reg instruction
            MARwrt = 1'b1;       // enable writing in MAR
            setMRE = 1'b1;       // enable reading from memory
         end

         EX_MULTU: begin
            nxt_state = WAIT_MULTU;
            startMult = 1'b1;
         end

         WAIT_MULTU: begin
            if(multReady)
               PrepareFetch;
            else
               nxt_state = WAIT_MULTU;
         end

         EX_MF: begin
            RFwrt = 1'b1;     // let writing in register file
            RegDst = 1'b1;    // RegDst = IR[15:11]
            MemtoReg = 1'b0;
            if(IR[5:0] == `_mfhi)
               mfhi = 1;
            if(IR[5:0] == `_mflo)
               mflo = 1;
            PrepareFetch;
         end
      endcase

   end

endmodule

//==============================================================================

module my_alu(
   input [3:0] Op,
   input [31:0] A,
   input [31:0] B,

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
         `AND : x =   A & B;
         `OR  : x =   A | B;
         `NOR : x = ~(A | B);
         `XOR : x =   A ^ B;
         `LUI : x = {B[15:0], 16'h0};
         default : x = 32'hxxxxxxxx;
      endcase

   assign #2 X = x;
   assign #2 Z = x == 32'h00000000;

endmodule

//==============================================================================

module reg_file(
   input clk,
   input write,
   input [4:0] WR,
   input [31:0] WD,
   input [4:0] RR1,
   input [4:0] RR2,
   output [31:0] RD1,
   output [31:0] RD2
);

   reg [31:0] rf_data [0:31];

   assign #2 RD1 = rf_data[ RR1 ];
   assign #2 RD2 = rf_data[ RR2 ];   

   always @( posedge clk ) begin
      if ( write )
         rf_data[ WR ] <= WD;

      rf_data[0] <= 32'h00000000;
   end

endmodule

//==============================================================================
// using session 1 multiplier for multiplier block:
module my_multiplier(
//---------------------Port directions and deceleration
   input wire clk,  
   input wire start,
   input wire [31:0] A, 
   input wire [31:0] B, 
   output reg [63:0] Product,
   output ready
    );



//----------------------------------------------------

//--------------------------------- register deceleration
reg [31:0] Multiplicand ;
reg [5:0]  counter;
//-----------------------------------------------------

//----------------------------------- wire deceleration
wire [32:0] adder_output;
//-------------------------------------------------------

//------------------------------------ combinational logic
assign adder_output = Multiplicand + Product[63:32];
assign product_write_enable = Product[0];
assign ready = counter[5];
//-------------------------------------------------------

//------------------------------------- sequential Logic
always @ (posedge clk)

   if(start) begin
      counter <= 6'h0;
      Product <= {32'b0, B};
      Multiplicand <= A;
   end

   else if(!ready) begin
         counter <= counter + 1;
         if(product_write_enable) begin
            Product <= (Product >> 1);
            Product[63:31] <= adder_output[32:0];
         end
         else
            Product <= (Product >> 1);
   end   

endmodule


//==============================================================================