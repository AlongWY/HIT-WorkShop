`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2018/11/08 22:43:43
// Design Name: 
// Module Name: TestCPU
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


module TestCPU();

reg clk;
reg reset;

CPU cpu(clk,reset);

initial
begin
clk = 0;
reset = 1;
#10
reset = 0;
end

always
begin
#10 clk = ~clk;
end

endmodule
