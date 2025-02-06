`timescale 1ns/1ns
module multiplier_b_nb
#(
   parameter nb = 10
)
(
//---------------------Port directions and deceleration
   input clk,  
   input start,
   input [nb-1:0] A, 
   input [nb-1:0] B, 
   output reg [2*nb-1:0] Product,
   output ready
    );



//----------------------------------------------------
reg odd = nb % 2;
//--------------------------------- register deceleration
reg [nb:0]  counter;
reg last_bit;
reg [nb+1:0] adder_input;
reg [2:0] determiner;
//-----------------------------------------------------

//----------------------------------- wire deceleration
wire [nb+1:0] adder_output;
reg [nb+1:0] Multiplicand;
wire [nb+1:0] two_Multiplicand;
wire [nb+1:0] neg_Multiplicand;
wire [nb+1:0] neg_two_Multiplicand;
//-------------------------------------------------------

//------------------------------------ combinational logic
assign adder_output = adder_input + { {2{Product[2*nb-1]}} , Product[2*nb-1:nb]};
assign ready = odd ? (counter == (nb/2 + 1)) : (counter == (nb/2));
assign two_Multiplicand = Multiplicand << 1;
assign neg_Multiplicand = ~Multiplicand + 1'b1;
assign neg_two_Multiplicand = neg_Multiplicand << 1;

//-------------------------------------------------------

//------------------------------------- sequential Logic
always @ (posedge clk) begin

   if(start) begin
      counter <= {nb{1'b0}};
      Product <= {{nb{1'b0}}, B};
      Multiplicand <= { {2{A[nb-1]}}, A};
      last_bit <= 1'b0;
   end

   else if(!ready) begin
      last_bit <= Product[1];
      counter <= counter + 1;
      
      Product <= {adder_output[nb+1:0], Product[nb-1:2]};

         if( (counter == (nb/2)) && (odd) )
            Product <= {adder_output[nb:0], Product[nb-1:1]};
         else
            Product <= {adder_output[nb+1:0], Product[nb-1:2]};
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


always@(*) begin
   if( (counter == (nb/2)) && (odd) )
      determiner[2:0] = {{2{Product[0]}}, last_bit};
   else
      determiner[2:0] = {Product[1:0], last_bit};
end
endmodule
