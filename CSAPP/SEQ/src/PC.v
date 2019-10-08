`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/23 20:07:48
// Design Name: 
// Module Name: PC
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


module PC(
    input clock,
    input  [3:0] icode,
    input  Cnd,
    input  [63:0] ValM,
    input  [63:0] ValC,
    input  [63:0] ValP,
    output reg[63:0] pc,
    input reset
    );
   // Instruction codes
   parameter IJXX = 4'h7;
   parameter ICALL = 4'h8;
   parameter IRET = 4'h9;
    
    always@(posedge clock)
    begin
    if(reset)
        pc=0;
    else
    pc = ((icode == ICALL) ? ValC :
         (icode == IJXX && Cnd) ? ValC :
         (icode == IRET) ? ValM :
          ValP);
    end
endmodule
