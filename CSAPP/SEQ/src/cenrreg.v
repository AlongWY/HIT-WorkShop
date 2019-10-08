`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/23 19:14:24
// Design Name: 
// Module Name: cenrreg
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


module cenrreg(out, in, Enable, Reset, Resetval, Clk);
    parameter width = 8;
    output [width-1:0] out;
    reg [width-1:0] out;
    input [width-1:0] in;
    input Enable;
    input Reset;
    input [width-1:0] Resetval;
    input Clk;
  
    always @(posedge Clk)
    begin
    if (Reset)
        out <= Resetval;
    else if (Enable)
     out <= in;
  end
endmodule
