`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/23 22:27:25
// Design Name: 
// Module Name: Mem
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

module ram(clock,
    addrA,wEnA,wDatA,rEnA,rDatA,
    addrB,wEnB,wDatB,rEnB,rDatB
    );
    parameter wordsize = 8;
    parameter wordcount = 512;
    parameter addrsize = 9;
    input clock,wEnA,rEnA,wEnB,rEnB;
    reg [wordsize-1:0] rDatA,rDatB;
    reg [wordsize-1:0] mem[wordcount-1:0];
    
    input [addrsize-1:0] addrA;
    input [wordsize-1:0] wDatA; 
    output [wordsize-1:0] rDatA;
   
    always @(negedge clock)
    begin
      if (wEnA)
        mem[addrA] <= wDatA;
      if (rEnA)
        rDatA <= mem[addrA];
    end
    
    input [addrsize-1:0] addrB;
    input [wordsize-1:0] wDatB; 
    output [wordsize-1:0] rDatB;
    always @(negedge clock)
    begin
      if (wEnB)
        mem[addrB] <= wDatB;
      if (rEnB)
        rDatB <= mem[addrB];
    end
endmodule
