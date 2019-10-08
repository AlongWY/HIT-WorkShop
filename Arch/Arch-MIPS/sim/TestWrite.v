module TestWrite();

//module Write(
//  input [5:0] Op,
//  input [31:0] LMD,
//  input [31:0] Res,
//  output [31:0] Reg
//);

reg [5:0] Op;
reg [31:0] LMD;
reg [31:0] Res;
wire [31:0] Reg;

Write write(Op, LMD, Res, Reg);

initial
begin
    Op = 0;
    LMD = 0;
    Res = 1;
end

always
begin
#10    Op = Op + 1;
end

endmodule