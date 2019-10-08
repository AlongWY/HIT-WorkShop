`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/06/19 13:19:20
// Design Name: 
// Module Name: pwdprocess
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


module pwdprocess(
    input [1:0] keypwd,             //������������
    input [1:0] cmppwd,             //��ȷ����
    input keypress,                 //�������壬�����ش���
    output res                      //�����������
    );
    wire cmp;
    assign cmp = keypwd == cmppwd;
    assign res = cmp && keypress;
endmodule
