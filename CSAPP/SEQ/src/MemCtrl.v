`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/25 11:35:45
// Design Name: 
// Module Name: MemCtrl
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


module MemCtrl(
    input [3:0]icode,
    input [63:0]valE,
    input [63:0]valA,
    input [63:0]valP,
    input instr_valid,
    input im_error,
    input dm_error,
    output readEn,
    output writeEn,
    output [1:0]status,
    output [63:0]mem_addr,
    output [63:0]mem_data
    );
    parameter IMRMOVQ = 4'h5;
    parameter IRET = 4'h9;
    parameter IPOPQ = 4'hB;
    parameter IRMMOVQ = 4'h4;
    parameter ICALL = 4'h8;
    parameter IPUSHQ = 4'hA;
    parameter SADR = 2'h0;
    parameter SINS = 2'h1;
    parameter SHLT = 2'h2;
    parameter SAOK = 2'h3;
    parameter IHALT = 4'h0;
    assign readEn = icode == IMRMOVQ | icode == IPOPQ | icode == IRET;
    assign writeEn = icode == IRMMOVQ | icode == IPUSHQ | icode == ICALL;
    assign mem_addr = (icode == IRMMOVQ | icode == IPUSHQ | icode == ICALL | icode == IMRMOVQ)?valE:
                                (icode == IPOPQ | icode == IRET)?valA:0;
    assign mem_data = (icode == IRMMOVQ | icode == IPUSHQ)?valA:
                               (icode == ICALL)?valP:0;
    assign status = (im_error | dm_error)?SADR:
                          (!instr_valid)?SINS:
                          (icode == IHALT)?SHLT:
                          SAOK;
endmodule
