`timescale 1ns/1ns
`default_nettype none

module fp_adder(
    input wire [31:0] a, 
    input wire [31:0] b, 
    output wire [31:0] s
);

// DEBUG
wire [7:0] EA = a[30:23];
wire [7:0] EB = b[30:23]; 

wire [22:0] FA = a[31] ? ~a[22:0] + 1'b1 : a[22:0];
wire [22:0] FB = b[31] ? ~b[22:0] + 1'b1 : b[22:0];
// DEBUG



wire [31:0] bigger;
wire [31:0] smaller;
wire S1, S2;        // 1 bigger sign, 2 smaller sign 
wire [7:0] E1, E2;  // 1 bigger exp,  2 smaller exp
wire [25:0] F1, F2; // 1 bigger frac, 2 smaller frac
wire small_borrow = (EB > EA);
wire [7:0] shift = E1 - E2;

assign bigger = small_borrow ? b : a;
assign smaller = small_borrow ? a : b;

assign S1 = bigger[31];
assign S2 = smaller[31];
assign E1 = (bigger[30:23] == 8'h00) ? (8'h01) : bigger[30:23];
assign F1 = (bigger[30:23] == 8'h00) ? ({1'b0, bigger[22:0], 2'b00}) : ({1'b1, bigger[22:0], 2'b00});
assign E2 = (smaller[30:23] == 8'h00) ? (8'h01) : smaller[30:23];
assign F2 = (smaller[30:23] == 8'h00) ? ({1'b0, smaller[22:0], 2'b00}) : ({1'b1, smaller[22:0], 2'b00});

wire [28:0] FF1, FF2;
wire [28:0] N1, N2;
wire SOR;
// DEBUG
wire [25:0] debug_SOR = (F2 << (26 - shift));
// DEBUG
assign SOR = ((F2 << (26 - shift)) != 26'b0);

assign FF1 = {2'b00, F1, 1'b0};
assign FF2 = {2'b00, (F2 >> shift), SOR};

assign N1 = S1 ? (~FF1 + 1'b1): FF1;
assign N2 = S2 ? (~FF2 + 1'b1): FF2;

wire [28:0] ALU = N1 + N2;
wire sgn = ALU[28];
wire [27:0] add = sgn ? ~ALU + 1'b1 : ALU;

wire [4:0] normalize_shift;
wire [4:0] raw_shift;
wire [4:0] first_one;

assign first_one =    add[27] ? 27 :
                            add[26] ? 26 :
                            add[25] ? 25 :
                            add[24] ? 24 :
                            add[23] ? 23 :
                            add[22] ? 22 :
                            add[21] ? 21 :
                            add[20] ? 20 :
                            add[19] ? 19 :
                            add[18] ? 18 :
                            add[17] ? 17 :
                            add[16] ? 16 :
                            add[15] ? 15 :
                            add[14] ? 14 :
                            add[13] ? 13 :
                            add[12] ? 12 :
                            add[11] ? 11 :
                            add[10] ? 10 :
                            add[9] ? 9 :
                            add[8] ? 8 :
                            add[7] ? 7 :
                            add[6] ? 6 :
                            add[5] ? 5 :
                            add[4] ? 4 :
                            add[3] ? 3 :
                            add[2] ? 2 :
                            add[1] ? 1 : 0;

assign raw_shift = 5'd27 - first_one;
assign normalize_shift = (E1 < raw_shift) ? E1 : raw_shift;

wire [27:0] shifted_result = add[27:0] << normalize_shift;
wire [7:0] E_shifted = E1 - normalize_shift + 1'b1;

// rounding process:
wire tie = shifted_result[4] ? 1'b1 : 1'b0;
wire round =    (~shifted_result[3]) ? (1'b0) :
                (shifted_result[0] | shifted_result[1] | shifted_result[2]) ? (1'b1) : tie;

wire [28:0] rounded = shifted_result[27:0] + {round, 3'b000};
wire [28:0] norm_rounded = rounded[28] ? rounded : (rounded << 1);
wire [7:0] E_final = norm_rounded[28] ? E_shifted + rounded[28]: 0;
wire [22:0] final = norm_rounded[27:5];

assign s = {sgn, E_final, final};
endmodule