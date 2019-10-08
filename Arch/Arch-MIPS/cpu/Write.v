module Write(
  input [31:0] IR,
  input [31:0] LMD,
  input [31:0] Res,
  output [31:0] Reg
);

wire [5:0] Op;
assign Op = IR[31:26];
assign Reg = Op[5:4] == 2'b00 ? Res : LMD;

endmodule