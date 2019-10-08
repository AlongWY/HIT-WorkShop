`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/12/31 16:23:59
// Design Name: 
// Module Name: testbench
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

module testbench( );
parameter RUN_MODE = 2'h0;
parameter RESET_MODE = 2'h1;
parameter DOWNLOAD_MODE = 2'h2;
parameter UPLOAD_MODE = 2'h3;

reg clock = 0;
reg [2:0] mode = RESET_MODE;
reg [63:0]uaddr = 0;
reg [63:0]idata = 1;
wire [63:0]rax;
wire [63:0]rdx;

always begin
    clock = 1;
    # 5;
    clock = 0;
    # 5;
end
// | # prog1: Pad with 3 nop's
//30f20a00000000000000 |   irmovq $10,%rdx
//30f00300000000000000 |   irmovq  $3,%rax
//6020                 |   addq %rdx,%rax
//00                   |   halt

initial begin
    #20 mode <= DOWNLOAD_MODE;
    uaddr <= 0;
    idata <= 64'h00000000000af230;
    #20;
    uaddr <= 8;
    idata <= 64'h00000003f0300000;
    #20;
    uaddr <= 16;
    idata <= 64'h000206000000000;
    #20;
    uaddr <= 24;
    idata <= 64'h0000000000000000;
    #20 mode <= RESET_MODE;
    #20 mode <= RUN_MODE;
end

Processor p(mode,uaddr,idata,rax,rdx,clock);

endmodule
