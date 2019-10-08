module TestExecute();

//module Execute(
//  input [5:0] Op,
//  input [31:0] NPC,
//  input [31:0] A,
//  input [31:0] B,
//  input [31:0] Imm,
//  output cond,
//  output [31:0]Res
//);

reg [5:0] Op;
reg [31:0] NPC;
reg [31:0] A;
reg [31:0] B;
reg [31:0] Imm;

wire cond;
wire [31:0]Res;

Execute execute(Op, NPC, A, B, Imm, cond, Res);

initial
begin
    Op = 0;
    NPC = 1;
    A = 1;
    B = 2;
    Imm = 3;
end

always
begin
#10 Op = Op + 1;
end


endmodule