`timescale 1ns/1ns
`default_nettype none

module double_adder(
    input wire [63:0] a, 
    input wire [63:0] b, 
    output wire [63:0] s
);

// DEBUG
wire [10:0] EA = a[62:52];
wire [10:0] EB = b[62:52]; 

wire [51:0] FA = a[63] ? ~a[51:0] + 1'b1 : a[51:0];
wire [51:0] FB = b[63] ? ~b[51:0] + 1'b1 : b[51:0];
// DEBUG



wire [63:0] bigger;
wire [63:0] smaller;
wire S1, S2;        // 1 bigger sign, 2 smaller sign 
wire [10:0] E1, E2;  // 1 bigger exp,  2 smaller exp
wire [54:0] F1, F2; // 1 bigger frac, 2 smaller frac
wire small_borrow = (EB > EA);
wire [10:0] shift = E1 - E2;

assign bigger = small_borrow ? b : a;
assign smaller = small_borrow ? a : b;

assign S1 = bigger[63];
assign S2 = smaller[63];
assign E1 = (bigger[62:52] == 11'h00) ? (11'h01) : bigger[62:52];
assign F1 = (bigger[62:52] == 11'h00) ? ({1'b0, bigger[51:0], 2'b00}) : ({1'b1, bigger[51:0], 2'b00});
assign E2 = (smaller[62:52] == 11'h00) ? (11'h01) : smaller[62:52];
assign F2 = (smaller[62:52] ==11'h00) ? ({1'b0, smaller[51:0], 2'b00}) : ({1'b1, smaller[51:0], 2'b00});

wire [57:0] FF1, FF2;
wire [57:0] N1, N2;
wire SOR;
// DEBUG
wire [54:0] debug_SOR = (F2 << (55 - shift));
// DEBUG
assign SOR = ((F2 << (55 - shift)) != 55'b0);

assign FF1 = {2'b00, F1, 1'b0};
assign FF2 = {2'b00, (F2 >> shift), SOR};

assign N1 = S1 ? (~FF1 + 1'b1): FF1;
assign N2 = S2 ? (~FF2 + 1'b1): FF2;

wire [57:0] ALU = N1 + N2;
wire sgn = ALU[57];
wire [56:0] add = sgn ? ~ALU + 1'b1 : ALU;

wire [5:0] normalize_shift;
wire [5:0] raw_shift;
wire [5:0] first_one;
//                             add[27] ? 27 : => add[56]
assign first_one =    add[56] ? 56 :
                            add[55] ? 55 :
                            add[54] ? 54 :
                            add[53] ? 53 :
                            add[52] ? 52 :
                            add[51] ? 51 :
                            add[50] ? 50 :
                            add[49] ? 49 :
                            add[48] ? 48 :
                            add[47] ? 47 :
                            add[46] ? 46 :
                            add[45] ? 45 :
                            add[44] ? 44 :
                            add[43] ? 43 :
                            add[42] ? 42 :
                            add[41] ? 41 :
                            add[40] ? 40 :
                            add[39] ? 39 :
                            add[38] ? 38 :
                            add[37] ? 37 :
                            add[36] ? 36 :
                            add[35] ? 35 :
                            add[34] ? 34 :
                            add[33] ? 33 :
                            add[32] ? 32 :
                            add[31] ? 31 :
                            add[30] ? 30 :
                            add[29] ? 29 :
                            add[28] ? 28 :
                            add[27] ? 27 :
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

assign raw_shift = 6'd56 - first_one;
assign normalize_shift = (E1 < raw_shift) ? E1 : raw_shift;

wire [56:0] shifted_result = add[56:0] << normalize_shift;
wire [10:0] E_shifted = E1 - normalize_shift + 1'b1;

// rounding process:
wire tie = shifted_result[4] ? 1'b1 : 1'b0;
wire round =    (~shifted_result[3]) ? (1'b0) :
                (shifted_result[0] | shifted_result[1] | shifted_result[2]) ? (1'b1) : tie;

wire [57:0] rounded = shifted_result[56:0] + {round, 3'b000};
wire [57:0] norm_rounded = rounded[57] ? rounded : (rounded << 1);
wire [10:0] E_final = norm_rounded[57] ? E_shifted + rounded[57]: 0;
wire [51:0] final = norm_rounded[56:5];

assign s = {sgn, E_final, final};
endmodule