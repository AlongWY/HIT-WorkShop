`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/25 14:44:02
// Design Name: 
// Module Name: processor
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


module Processor(
    input [1:0] mode,
    input [63:0] udaddr,
    input [63:0] idata,
    output [63:0] rax,
    output [63:0] rdx,
    input clock
    );
    parameter RUN_MODE = 2'h0;
    parameter RESET_MODE = 2'h1;
    parameter DOWNLOAD_MODE = 2'h2;
    parameter UPLOAD_MODE = 2'h3;
    
    wire resetting,uploading,downloading,running,getting_info,mem_read,mem_write,i_ok,m_ok,cnd,instr_valid;
    wire [63:0] mem_addr,mem_data,valA,valB,valC,valP,valE,valM,pc;
    wire [79:0] ibytes;
    wire [3:0]icode,ifun,rA,rB;
    wire [1:0] status;
    
    wire imem_error = ~i_ok;
    wire dmem_error = ~m_ok;
    assign resetting = (mode == RESET_MODE);
    assign uploading = (mode == UPLOAD_MODE);
    assign downloading = (mode == DOWNLOAD_MODE);
    assign running = (mode == RUN_MODE);
    
    BMemory mem(clock,
        (running & mem_read)  | uploading,
        (running & mem_write) | downloading,
        (downloading | uploading) ? udaddr : mem_addr,
        downloading ? idata : mem_data,
        valM,pc,ibytes,i_ok,m_ok);
    
    MemCtrl ctrl(icode,valE,valA,valP,instr_valid,
                     imem_error,dmem_error,mem_read,mem_write,
                     status,mem_addr,mem_data);

    Excute exe(clock,icode,ifun,valC,valA,valB,resetting,cnd,valE);
    
    wire [63:0]rcx;
    Decode decoder(clock,resetting,icode,rA,rB,cnd,valM,valE,valA,valB,rax,rcx,rdx);
    
    Fetch fetch(pc,ibytes,imem_error,instr_valid,icode,ifun,rA,rB,valC,valP); 
                     
    PC Pc(clock,icode,cnd,valM,valC,valP,pc,resetting);
    
endmodule
