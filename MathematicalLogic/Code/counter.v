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
    input CLK,                //����ʱ��
    input CLR,                //��λ
    input EN,                 //ʹ�ܶ�,����Ч
    output reg[2:0]num           //��ǰ����
    );
    initial num = 0;
    
    always @(posedge CLK or negedge CLR)
      if(~CLR)
        num=0;
      else
        if(~EN)
          num = num + 1;
     
endmodule
