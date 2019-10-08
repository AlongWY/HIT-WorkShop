`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/23 14:36:38
// Design Name: 
// Module Name: Excute
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


module Excute(
    input clock,
    input  [3:0] icode,
    input  [3:0] ifun,
    input  [63:0]valC,
    input  [63:0]valA,
    input  [63:0]valB,
    input  reset,
    output Cnd,
    output [63:0]valE
    );
    
    wire set_cc;
    wire [2:0] new_CC;
    reg [2:0] CC;
      
      always @(posedge clock)
        begin
        if(reset)
          CC <= 3'b100;
        else if(set_cc)
          CC <= new_CC;
        end
    
      wire [63:0] aluA;
      wire [63:0] aluB;
      wire [3:0] alufun;
      wire ZF = CC[2];
      wire SF = CC[1];
      wire OF = CC[0];
      
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
      parameter aluADD = 4'h0;
      parameter aluSUB = 4'h1;
      parameter aluAND = 4'h2;
      parameter aluXOR = 4'h3;
      parameter C_YES = 4'h0;
      parameter C_LE = 4'h1;
      parameter C_L = 4'h2;
      parameter C_E = 4'h3;
      parameter C_NE = 4'h4;
      parameter C_GE = 4'h5;
      parameter C_G = 4'h6;
      
      assign aluA = (icode == IRRMOVQ | icode == IOPQ) ? valA :
                    (icode == IIRMOVQ | icode == IRMMOVQ |
                    icode == IMRMOVQ | icode == IIADDQ) ? valC :
                    (icode == ICALL | icode == IPUSHQ) ? -8 :
                    (icode == IRET | icode == IPOPQ) ? 8 :
                    0;
      assign aluB = (icode == IRMMOVQ | icode == IMRMOVQ |
                    icode == IOPQ | icode == ICALL |
                    icode == IPUSHQ | icode == IRET |
                    icode == IPOPQ | icode == IIADDQ) ? valB :
                    (icode == IRRMOVQ | IIRMOVQ) ? 0 :
                    1;
      assign alufun = icode == IOPQ ? ifun : aluADD;
      assign set_cc = icode == IOPQ;
      assign valE = alufun == aluADD ? aluA + aluB :
                    alufun == aluSUB ? aluA - aluB :
                    alufun == aluAND ? aluA & aluB :
                    aluA ^ aluB;
      assign new_CC[2] = valE == 0;
      assign new_CC[1] = valE[63];
      assign new_CC[0] = alufun == aluADD ? (aluA[63] == aluB[63]) & (aluA[63] != valE[63]) :
                         alufun == aluSUB ? (~aluA[63] == aluB[63]) & (aluA[63] != valE[63]) :
                         0;
      assign Cnd = (ifun == C_YES) |
                   (ifun == C_LE & ((SF^OF)|ZF)) |
                   (ifun == C_L & (SF^OF)) |
                   (ifun == C_E & ZF) |
                   (ifun == C_NE & ~ZF) |
                   (ifun == C_GE & ((~SF^OF)|ZF)) |
                   (ifun == C_G & (~SF^OF));
endmodule
