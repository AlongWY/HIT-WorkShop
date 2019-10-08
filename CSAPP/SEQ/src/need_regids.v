`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/18 10:05:35
// Design Name: 
// Module Name: need_regids
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


module Need_regids(
    input [3:0] icode,
    output need_regids
    );
   // Instruction codes
   parameter IRRMOVQ = 4'h2;
   parameter IIRMOVQ = 4'h3;
   parameter IRMMOVQ = 4'h4;
   parameter IMRMOVQ = 4'h5;
   parameter IOPQ = 4'h6;
   parameter IPUSHQ = 4'hA;
   parameter IPOPQ = 4'hB;
   parameter IIADDQ = 4'hC;
    assign need_regids =  (icode == IRRMOVQ | icode == IOPQ | icode == IPUSHQ | icode ==
    IPOPQ | icode == IIRMOVQ | icode == IRMMOVQ | icode == IMRMOVQ | icode == IIADDQ);
endmodule
