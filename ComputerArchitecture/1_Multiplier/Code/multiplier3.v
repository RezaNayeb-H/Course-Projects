`timescale 1ns/1ns
module multiplier3(
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
reg [8:0] Multiplicand;
reg [3:0]  counter;
//-----------------------------------------------------

//----------------------------------- wire deceleration
wire [8:0] adder_output;
wire [8:0] sub_output;
//-------------------------------------------------------

//------------------------------------ combinational logic
assign adder_output = Multiplicand + {Product[15], Product[15:8]};
assign sub_output = ~Multiplicand + {Product[15], Product[15:8]} + 8'h1;
assign product_write_enable = Product[0];
assign ready = counter[3];
//-------------------------------------------------------

//------------------------------------- sequential Logic
always @ (posedge clk)

   if(start) begin
      counter <= 4'h0;
      Product <= {8'h00, B};
      Multiplicand <= {A[7], A};
   end

   else if(!ready) begin
         counter <= counter + 1;
         if(product_write_enable) begin
            if(counter == 4'h7)
               Product <= {sub_output[8:0], Product[7:1]};
            else
               Product <= {adder_output[8:0], Product[7:1]};
         end
         else
            Product <= {Product[15], Product[15:1]};
   end   

endmodule
