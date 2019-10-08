`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/18 10:34:48
// Design Name: 
// Module Name: Fetch
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


module Fetch(
    input [63:0] pc,
    input [79:0] ibytes,
    input imem_error,
    output instr_valid,
    output [3:0] icode,
    output [3:0] ifun,
    output [3:0] rA,
    output [3:0] rB,
    output [63:0] valC,
    output [63:0] valP
    );
    parameter IHALT = 4'h0;
    parameter INOP = 4'h1;
    parameter IRRMOVQ = 4'h2;
    parameter IIRMOVQ = 4'h3;
    parameter IRMMOVQ = 4'h4;
    parameter IMRMOVQ = 4'h5;
    parameter IOPQ = 4'h6;
    parameter IJXX = 4'h7;
    parameter ICALL = 4'h8;
    parameter IRET = 4'h9;
    parameter IPUSHQ = 4'hA;
    parameter IPOPQ = 4'hB;
    parameter IIADDQ = 4'hC;
    
    wire need_regids;
    wire need_valC;
    
    assign rA = ibytes[15:12];
    assign rB = ibytes[11:8];
    assign ifun= ibytes[3:0];
    assign icode = imem_error ? 4'h1 : ibytes[7:4] ;
    assign need_valC = (icode == IIRMOVQ | icode == IRMMOVQ | icode == IMRMOVQ | icode == IJXX | icode == ICALL | icode == IIADDQ);
    assign need_regids =  icode == IRRMOVQ | icode == IOPQ | 
                                   icode == IPUSHQ | icode ==IPOPQ | 
                                   icode == IIRMOVQ | icode == IRMMOVQ |
                                   icode == IMRMOVQ | icode == IIADDQ;
    assign valC = need_regids ? ibytes[79:16]:ibytes[71:8];
    assign valP = pc + 1 + need_regids + 8 * need_valC;
    assign instr_valid =  icode == INOP |  icode == IHALT |
                                icode == IRRMOVQ |   icode == IIRMOVQ |  
                                icode == IRMMOVQ|icode == IMRMOVQ |  
                                icode == IOPQ    | icode == IJXX  |
                                icode == ICALL   |icode == IRET | 
                                icode == IPUSHQ  | icode == IPOPQ | icode == IIADDQ;
endmodule
