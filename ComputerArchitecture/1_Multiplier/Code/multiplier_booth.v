`timescale 1ns/1ns
module multiplier_booth(
//---------------------Port directions and deceleration
   input clk,  
   input start,
   input [7:0] A, 
   input [7:0] B, 
   output reg [15:0] Product,
   output ready
    );



//----------------------------------------------------

//--------------------------------- register deceleration
reg [2:0]  counter;
reg last_bit;
reg [9:0] adder_input;
//-----------------------------------------------------

//----------------------------------- wire deceleration
wire [9:0] adder_output;
wire [2:0] determiner = {Product[1:0], last_bit};
reg [9:0] Multiplicand;
wire [9:0] two_Multiplicand;
wire [9:0] neg_Multiplicand;
wire [9:0] neg_two_Multiplicand;
//-------------------------------------------------------

//------------------------------------ combinational logic
assign adder_output = adder_input + { {2{Product[15]}} , Product[15:8]};
assign ready = counter[2];
assign two_Multiplicand = Multiplicand << 1;
assign neg_Multiplicand = ~Multiplicand + 10'b1;
assign neg_two_Multiplicand = neg_Multiplicand << 1;

//-------------------------------------------------------

//------------------------------------- sequential Logic
always @ (posedge clk) begin

   if(start) begin
      counter <= 4'h0;
      Product <= {8'h00, B};
      Multiplicand <= {{2{A[7]}}, A};
      last_bit <= 1'b0;
   end

   else if(!ready) begin
      last_bit <= Product[1];
      counter <= counter + 1;

      Product <= {adder_output[9:0], Product[7:2]};
   end
end
always @(*) begin
   case (determiner)
      3'b00_0: adder_input = 10'b0;
      3'b00_1: adder_input = Multiplicand;
      3'b01_0: adder_input = Multiplicand;
      3'b01_1: adder_input = two_Multiplicand;
      3'b10_0: adder_input = neg_two_Multiplicand;
      3'b10_1: adder_input = neg_Multiplicand;
      3'b11_0: adder_input = neg_Multiplicand;
      3'b11_1: adder_input = 10'b0;
      default: adder_input = 10'b0;
   endcase
end

endmodule
