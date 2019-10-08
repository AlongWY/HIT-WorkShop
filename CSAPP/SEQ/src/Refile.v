`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/31 19:16:48
// Design Name: 
// Module Name: Refile
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


module Regfile(dstE, dstM, valE, valM, srcA, srcB, Reset, Clk,
               valA, valB, rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi,
               r8, r9, r10, r11, r12, r13, r14);
  input [3:0] dstE, dstM;
  input [63:0] valE, valM;
  input [3:0] srcA, srcB;
  input Reset, Clk;
  output [63:0] valA, valB;
  output [63:0] rax, rcx, rdx, rbx, rsp, rbp, rsi, rdi;
  output [63:0] r8, r9, r10, r11, r12, r13, r14;
  
  parameter RAX = 4'h0;
  parameter RCX = 4'h1;
  parameter RDX = 4'h2;
  parameter RBX = 4'h3;
  parameter RSP = 4'h4;
  parameter RBP = 4'h5;
  parameter RSI = 4'h6;
  parameter RDI = 4'h7;
  parameter R8 = 4'h8;
  parameter R9 = 4'h9;
  parameter R10 = 4'ha;
  parameter R11 = 4'hb;
  parameter R12 = 4'hc;
  parameter R13 = 4'hd;
  parameter R14 = 4'he;
  parameter RNONE = 4'hf;
  
  wire [63:0] rax_dat, rcx_dat, rdx_dat, rbx_dat,
               rsp_dat, rbp_dat, rsi_dat, rdi_dat,
               r8_dat, r9_dat, r10_dat, r11_dat, r12_dat, r13_dat, r14_dat;
  wire rax_wrt, rcx_wrt, rdx_wrt, rbx_wrt, rsp_wrt, rbp_wrt, rsi_wrt, rdi_wrt,
        r8_wrt, r9_wrt, r10_wrt, r11_wrt, r12_wrt, r13_wrt, r14_wrt;
        
  cenrreg #(64) rax_reg(rax, rax_dat, rax_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) rcx_reg(rcx, rcx_dat, rcx_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) rdx_reg(rdx, rdx_dat, rdx_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) rbx_reg(rbx, rbx_dat, rbx_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) rsp_reg(rsp, rsp_dat, rsp_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) rbp_reg(rbp, rbp_dat, rbp_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) rsi_reg(rsi, rsi_dat, rsi_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) rdi_reg(rdi, rdi_dat, rdi_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) r8_reg(r8, r8_dat, r8_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) r9_reg(r9, r9_dat, r9_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) r10_reg(r10, r10_dat, r10_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) r11_reg(r11, r11_dat, r11_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) r12_reg(r12, r12_dat, r12_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) r13_reg(r13, r13_dat, r13_wrt, Reset, 64'b0, Clk);
  cenrreg #(64) r14_reg(r14, r14_dat, r14_wrt, Reset, 64'b0, Clk);
  
  assign valA = srcA == RAX ? rax :
                srcA == RCX ? rcx :
                srcA == RDX ? rdx :
                srcA == RBX ? rbx :
                srcA == RSP ? rsp :
                srcA == RBP ? rbp :
                srcA == RSI ? rsi :
                srcA == RDI ? rdi :
                srcA == R8 ? r8 :
                srcA == R9 ? r9 :
                srcA == R10 ? r10 :
                srcA == R11 ? r11 :
                srcA == R12 ? r12 :
                srcA == R13 ? r13 :
                srcA == R14 ? r14 :
                0;
                
  assign valB = srcB == RAX ? rax :
                srcB == RCX ? rcx :
                srcB == RDX ? rdx :
                srcB == RBX ? rbx :
                srcB == RSP ? rsp :
                srcB == RBP ? rbp :
                srcB == RSI ? rsi :
                srcB == RDI ? rdi :
                srcB == R8 ? r8 :
                srcB == R9 ? r9 :
                srcB == R10 ? r10 :
                srcB == R11 ? r11 :
                srcB == R12 ? r12 :
                srcB == R13 ? r13 :
                srcB == R14 ? r14 :
                0;
                
  assign rax_dat = dstM == RAX ? valM : valE;
  assign rcx_dat = dstM == RCX ? valM : valE;
  assign rdx_dat = dstM == RDX ? valM : valE;
  assign rbx_dat = dstM == RBX ? valM : valE;
  assign rsp_dat = dstM == RSP ? valM : valE;
  assign rbp_dat = dstM == RBP ? valM : valE;
  assign rsi_dat = dstM == RSI ? valM : valE;
  assign rdi_dat = dstM == RDI ? valM : valE;
  assign r8_dat = dstM == R8 ? valM : valE;
  assign r9_dat = dstM == R9 ? valM : valE;
  assign r10_dat = dstM == R10 ? valM : valE;
  assign r11_dat = dstM == R11 ? valM : valE;
  assign r12_dat = dstM == R12 ? valM : valE;
  assign r13_dat = dstM == R13 ? valM : valE;
  assign r14_dat = dstM == R14 ? valM : valE;
  
  assign rax_wrt = dstM == RAX | dstE == RAX;
  assign rcx_wrt = dstM == RCX | dstE == RCX;
  assign rdx_wrt = dstM == RDX | dstE == RDX;
  assign rbx_wrt = dstM == RBX | dstE == RBX;
  assign rsp_wrt = dstM == RSP | dstE == RSP;
  assign rbp_wrt = dstM == RBP | dstE == RBP;
  assign rsi_wrt = dstM == RSI | dstE == RSI;
  assign rdi_wrt = dstM == RDI | dstE == RDI;
  assign r8_wrt = dstM == R8 | dstE == R8;
  assign r9_wrt = dstM == R9 | dstE == R9;
  assign r10_wrt = dstM == R10 | dstE == R10;
  assign r11_wrt = dstM == R11 | dstE == R11;
  assign r12_wrt = dstM == R12 | dstE == R12;
  assign r13_wrt = dstM == R13 | dstE == R13;
  assign r14_wrt = dstM == R14 | dstE == R14;
endmodule
