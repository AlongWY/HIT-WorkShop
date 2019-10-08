`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/31 20:51:27
// Design Name: 
// Module Name: testMem
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


module testMem();
reg clock,mem_read,mem_write;
reg[63:0] mem_addr, mem_data, pc;
wire[79:0] ibytes;
wire[63:0] valM;
wire m_ok,i_ok; 

BMemory bmem(clock,mem_read,mem_write,mem_addr,mem_data,valM,pc,ibytes,i_ok,m_ok);

initial begin
    mem_addr<=0;
    mem_data<= 64'h0123456789abcdef;
    pc<=64'b0;
    mem_write<=1;
    mem_read<=0;
    #10;
    mem_addr<=2;
    mem_data<= 64'h0123456789abcdef;
    #10;
    mem_addr<=3;
    mem_data<= 64'h0123456789abcdef;
    #10
    mem_write <= 0;
    mem_read <=1;
    pc<=64'b0;
    mem_addr<=2;
end

always begin;
    clock<=1;
    #5;
    clock<=0;
    #5;
end
endmodule
