
`timescale 1ns/1ns

module double_adder__tb2();

  reg [63:0] s_true, a, b;

  real ar,br;
  int errors, no_of_tests;

  initial begin
  
    errors = 0;
    no_of_tests = 2000000;
    for (int i = 0; i < no_of_tests; i++) begin
      a = {$random(), $random()};
      b = {$random(), $random()};
      ar = $bitstoreal(a);
      br = $bitstoreal(b);
      s_true = $realtobits(ar+ br);
      #10;
      if (uut.s != s_true && s_true[62:52] != 11'b11111111111 && a[62:52] != 11'b11111111111 && b[62:52] != 11'b11111111111) begin
        $write("\tError: %16x + %16x, expected: %16x, but got: %16x\n", a, b, s_true, uut.s);
        errors++;
        $stop;  
      end
      if(i && i % 20000 == 0)
         $display("No. of random tests applied: %0d", i);
    end

    $display("%d (%%%0d) errors are found.\n", errors, (errors*100.0 + no_of_tests/2)/(no_of_tests + 1));
    $stop;  
  end

  double_adder uut( .a(a), .b(b), .s());

endmodule