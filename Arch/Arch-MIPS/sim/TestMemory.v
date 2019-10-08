module TestMemory();

//module Memory(
//  input clk,
//  input cond,
//  input [5:0] Op,
//  input [31:0] NPC,
//  input [31:0] Res,
//  input [31:0] Data,
//  output [31:0] RPC,
//  output [31:0] LMD
//);

reg clk;
reg cond;
reg [5:0]Op;
reg [31:0] NPC;
reg [31:0] Res;
reg [31:0] Data;
wire [31:0] RPC;
wire [31:0] LMD;

Memory memoey(clk,cond,Op,NPC,Res,Data,RPC,LMD);

initial
begin
    clk = 0;
    cond = 0;
    Op = 0;
    NPC = 0;
    Res = 0;
    Data = 0;
end

always
begin
#10    clk = ~clk;
end

endmodule