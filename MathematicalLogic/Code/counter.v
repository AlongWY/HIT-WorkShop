`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/06/19 13:44:58
// Design Name: 
// Module Name: counter
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


module counter(
    input CLK,                //计数时钟
    input CLR,                //复位
    input EN,                 //使能端,低有效
    output reg[2:0]num           //当前计数
    );
    initial num = 0;
    
    always @(posedge CLK or negedge CLR)
      if(~CLR)
        num=0;
      else
        if(~EN)
          num = num + 1;
     
endmodule
