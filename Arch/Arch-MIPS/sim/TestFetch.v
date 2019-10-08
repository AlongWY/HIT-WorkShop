`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2018/11/08 19:14:18
// Design Name: 
// Module Name: TestFetch
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module TestFetch();

reg clk;
reg reset;
wire [31:0]IR;
wire [31:0]NPC;
wire [31:0]RPC;

initial
begin
    clk = 0;
    reset = 0;
end

assign RPC = NPC > 255 ? 0 : NPC;

Fetch fetch(clk,reset,RPC,IR,NPC);

always
begin
#2 clk =  ~clk;
end

endmodule
