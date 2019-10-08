`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/23 20:32:30
// Design Name: 
// Module Name: Memory
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


module Decode(
    input clock,
    input reset,
    input [3:0] icode,
    input [3:0] rA,
    input [3:0] rB,
    input cnd,
    input [63:0] valM,
    input [63:0] valE,
    output [63:0] valA,
    output [63:0] valB,
    output [63:0] rax,
    output [63:0] rcx,
    output [63:0] rdx,
    output [63:0] rbx,
    output [63:0] rsp,
    output [63:0] rbp,
    output [63:0] rsi,
    output [63:0] rdi,
    output [63:0] r8,
    output [63:0] r9,
    output [63:0] r10,
    output [63:0] r11,
    output [63:0] r12,
    output [63:0] r13,
    output [63:0] r14
    );
    wire [3:0] srcA, srcB, dstE, dstM;
    
    parameter IRRMOVQ = 4'h2;
    parameter IIRMOVQ = 4'h3;
    parameter IRMMOVQ = 4'h4;
    parameter IMRMOVQ = 4'h5;
    parameter IOPQ = 4'h6;
    parameter ICALL = 4'h8;
    parameter IRET = 4'h9;
    parameter IPUSHQ = 4'hA;
    parameter IPOPQ = 4'hB;
    parameter IIADDQ = 4'hC;
    parameter RSP = 4'h4;
    parameter RNONE = 4'hf;
    
    assign srcA = (icode == IRRMOVQ || icode == IRMMOVQ || icode == IOPQ || icode == IPUSHQ) ? rA :
                  (icode == IPOPQ || icode == IRET) ? RSP :
                  RNONE;
    assign srcB = (icode == IMRMOVQ || icode == IRMMOVQ || icode == IOPQ || icode == IIADDQ) ? rB :
                  (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET) ? RSP :
                  RNONE;
    assign dstE = (icode == IRRMOVQ) && cnd ? rB :
                  (icode == IIRMOVQ || icode == IOPQ || icode == IIADDQ) ? rB :
                  (icode == IPUSHQ || icode == IPOPQ || icode == ICALL || icode == IRET) ? RSP :
                  RNONE;
    assign dstM = (icode == IMRMOVQ || icode == IPOPQ) ? rA : RNONE;
    
    Regfile R(dstE, dstM, valE, valM, srcA, srcB, reset, clock,
              valA, valB, rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi,
              r8, r9, r10, r11, r12, r13, r14);
endmodule
