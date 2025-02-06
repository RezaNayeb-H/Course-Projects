`timescale 1ns/1ns
module multiplier_nb
#(
   parameter nb = 10
)
(
//---------------------Port directions and deceleration
   input clk,  
   input start,
   input [nb-1:0] A, 
   input [nb-1:0] B, 
   output reg [2*nb - 1:0] Product,
   output ready
    );



//----------------------------------------------------

//--------------------------------- register deceleration
reg [nb:0] Multiplicand;
reg [nb:0]  counter;
//-----------------------------------------------------

//----------------------------------- wire deceleration
wire [nb:0] adder_output;
wire [nb:0] sub_output;
//-------------------------------------------------------

//------------------------------------ combinational logic
assign adder_output = Multiplicand + {Product[2*nb-1], Product[2*nb-1:nb]};
assign sub_output = ~Multiplicand + {Product[2*nb-1], Product[2*nb-1:nb]} + 1'b1;
assign product_write_enable = Product[0];
assign ready = (counter == nb);
//-------------------------------------------------------

//------------------------------------- sequential Logic
always @ (posedge clk)

   if(start) begin
      counter <= {nb{1'b0}};
      Product <= {{nb{1'b0}}, B};
      Multiplicand <= {A[nb-1], A};
   end

   else if(!ready) begin
         counter <= counter + 1;
         if(product_write_enable) begin
            if(counter == (nb - 1))
               Product <= {sub_output[nb:0], Product[nb-1:1]};
            else
               Product <= {adder_output[nb:0], Product[nb-1:1]};
         end
         else
            Product <= {Product[2*nb -1], Product[2*nb - 1:1]};
   end   

endmodule
